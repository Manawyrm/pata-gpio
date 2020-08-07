#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/libata.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/printk.h>
#include <scsi/scsi_host.h>

// CS0 High / CS1 Low
#define REG_CMD		
#define REG_DATA		0x00
#define REG_ERROR		0x01
#define REG_FEATURE		0x01
#define REG_NSECT		0x02
#define REG_LBAL		0x03
#define REG_LBAM		0x04
#define REG_LBAH		0x05
#define REG_DEVICE		0x06
#define REG_STATUS		0x07
#define REG_COMMAND		0x07

// CS1 High / CS0 Low
#define REG_ALTSTATUS	0x10
#define REG_CTL			0x10

struct pata_gpio {
	struct device *dev;
	struct gpio_descs *databus_gpios;
	struct gpio_desc *reset_gpio;
	struct gpio_descs *cs_gpios;
	struct gpio_descs *address_gpios;
	struct gpio_desc *strobe_write_gpio;
	struct gpio_desc *strobe_read_gpio;
};

static int pata_gpio_set_register(struct pata_gpio *pata, unsigned long reg)
{
	int err;
	unsigned long cs_state = 0b01;

	if (reg & 0xF0)
		cs_state = 0b10;

	err = gpiod_set_array_value(pata->cs_gpios->ndescs,
										 pata->cs_gpios->desc,
										 pata->cs_gpios->info,
										 &cs_state);
	if (err)
		return err;

	return gpiod_set_array_value(pata->address_gpios->ndescs,
										 pata->address_gpios->desc,
										 pata->address_gpios->info,
										 &reg);
}

static int pata_gpio_read16(struct pata_gpio *pata, u8 reg, u16 *result)
{
	u8 i;
	int err;
	unsigned long value = 0;

	for (i = 0; i < pata->databus_gpios->ndescs; i++) {
		err = gpiod_direction_input(pata->databus_gpios->desc[i]);
		if (err)
			return err;
	}

	err = pata_gpio_set_register(pata, reg);
	if (err)
		return err;

	gpiod_set_value(pata->strobe_read_gpio, 1);
	ndelay(165); // PIO-0 ATA Interface Reference Manual, Rev. C, P. 66 "DIOR–/DIOW– pulse width 16-bit"

	err = gpiod_get_array_value(pata->databus_gpios->ndescs,
				   pata->databus_gpios->desc,
				   pata->databus_gpios->info,
				   &value);

	gpiod_set_value(pata->strobe_read_gpio, 0);

	if (!err)
		*result = value;

	return err;
}

static int pata_gpio_write16(struct pata_gpio *pata, u8 reg, unsigned long value)
{
	u8 i;
	int err;

	err = pata_gpio_set_register(pata, reg);
	if (err)
		return err;

	for (i = 0; i < pata->databus_gpios->ndescs; i++) {
		err = gpiod_direction_output(pata->databus_gpios->desc[i], (value >> i) & 0x01);
		if (err)
			return err;
	}

	gpiod_set_value(pata->strobe_write_gpio, 1);
	ndelay(165); // PIO-0 ATA Interface Reference Manual, Rev. C, P. 66 "DIOR–/DIOW– pulse width 16-bit"
	gpiod_set_value(pata->strobe_write_gpio, 0);
	ndelay(30);  // PIO-0 ATA Interface Reference Manual, Rev. C, P. 66 "DIOW– data hold"

	for (i = 0; i < pata->databus_gpios->ndescs; i++) {
		err = gpiod_direction_input(pata->databus_gpios->desc[i]);
		if (err)
			return err;
	}

	return 0;
}

static void pata_gpio_write16_safe(struct pata_gpio *pata, u8 reg, unsigned long value)
{
	int err;
	
	err = pata_gpio_write16(pata, reg, value);
	if (err) {
		dev_err(pata->dev, "failed to write gpios in %s, code %d\n", __func__, err);
		BUG();
	}
}

static u16 pata_gpio_read16_safe(struct pata_gpio *pata, u8 reg)
{
	u16 result;
	int err;

	err = pata_gpio_read16(pata, reg, &result);
	if (err) {
		dev_err(pata->dev, "failed to read gpios in %s, code %d\n", __func__, err);
		BUG();
	}

	return result;
}

/*
 * pata_gpio_check_status - Read device status register
 */
static u8 pata_gpio_check_status(struct ata_port *ap)
{
	return pata_gpio_read16_safe(ap->host->private_data, REG_STATUS) & 0xFF;
}

/*
 * pata_gpio_check_altstatus - Read alternate device status register
 */
static u8 pata_gpio_check_altstatus(struct ata_port *ap)
{
	return pata_gpio_read16_safe(ap->host->private_data, REG_ALTSTATUS) & 0xFF;
}

/*
 * pata_gpio_exec_command - issue ATA command to host controller
 */
static void pata_gpio_exec_command(struct ata_port *ap,
				const struct ata_taskfile *tf)
{
	pata_gpio_write16_safe(ap->host->private_data, REG_COMMAND, tf->command);
	ata_sff_pause(ap);
}

/*
 * pata_gpio_tf_load - send taskfile registers to host controller
 */
static void pata_gpio_tf_load(struct ata_port *ap,
				const struct ata_taskfile *tf)
{
	unsigned int is_addr = tf->flags & ATA_TFLAG_ISADDR;

	if (tf->ctl != ap->last_ctl) {
		pata_gpio_write16_safe(ap->host->private_data, REG_CTL, tf->ctl);
		ap->last_ctl = tf->ctl;
		ata_wait_idle(ap);
	}

	if (is_addr && (tf->flags & ATA_TFLAG_LBA48)) {
		pata_gpio_write16_safe(ap->host->private_data, REG_FEATURE, tf->hob_feature);
		pata_gpio_write16_safe(ap->host->private_data, REG_NSECT, tf->hob_nsect);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAL, tf->hob_lbal);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAM, tf->hob_lbam);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAH, tf->hob_lbah);
	}

	if (is_addr) {
		pata_gpio_write16_safe(ap->host->private_data, REG_FEATURE, tf->feature);
		pata_gpio_write16_safe(ap->host->private_data, REG_NSECT, tf->nsect);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAL, tf->lbal);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAM, tf->lbam);
		pata_gpio_write16_safe(ap->host->private_data, REG_LBAH, tf->lbah);
	}

	if (tf->flags & ATA_TFLAG_DEVICE)
		pata_gpio_write16_safe(ap->host->private_data, REG_DEVICE, tf->device);

	ata_wait_idle(ap);
}

/*
 * pata_gpio_tf_read - input device's ATA taskfile shadow registers
 */
static void pata_gpio_tf_read(struct ata_port *ap, struct ata_taskfile *tf)
{

	tf->feature = pata_gpio_read16_safe(ap->host->private_data, REG_FEATURE);
	tf->nsect = pata_gpio_read16_safe(ap->host->private_data, REG_NSECT);
	tf->lbal = pata_gpio_read16_safe(ap->host->private_data, REG_LBAL);
	tf->lbam = pata_gpio_read16_safe(ap->host->private_data, REG_LBAM);
	tf->lbah = pata_gpio_read16_safe(ap->host->private_data, REG_LBAH);
	tf->device = pata_gpio_read16_safe(ap->host->private_data, REG_DEVICE);

	if (tf->flags & ATA_TFLAG_LBA48) {
		pata_gpio_write16_safe(ap->host->private_data, REG_CTL, tf->ctl | ATA_HOB);

		tf->hob_feature = pata_gpio_read16_safe(ap->host->private_data, REG_FEATURE);
		tf->hob_nsect = pata_gpio_read16_safe(ap->host->private_data, REG_NSECT);
		tf->hob_lbal = pata_gpio_read16_safe(ap->host->private_data, REG_LBAL);
		tf->hob_lbam = pata_gpio_read16_safe(ap->host->private_data, REG_LBAM);
		tf->hob_lbah = pata_gpio_read16_safe(ap->host->private_data, REG_LBAH);

		pata_gpio_write16_safe(ap->host->private_data, REG_CTL, tf->ctl);
		ap->last_ctl = tf->ctl;
	}
}

/*
 * pata_gpio_data_xfer - Transfer data by PIO
 */
static unsigned int pata_gpio_data_xfer(struct ata_queued_cmd *qc,
				unsigned char *buf, unsigned int buflen, int rw)
{
	unsigned int i;
	unsigned int words = buflen >> 1;
	struct ata_port *ap = qc->dev->link->ap;
	u16 *buf16 = (u16 *) buf;

	/* Transfer multiple of 2 bytes */
	if (rw == READ)
		for (i = 0; i < words; i++)
			buf16[i] = pata_gpio_read16_safe(ap->host->private_data, REG_DATA);
	else
		for (i = 0; i < words; i++)
			pata_gpio_write16_safe(ap->host->private_data, REG_DATA, buf16[i]);

	/* Transfer trailing 1 byte, if any. */
	if (unlikely(buflen & 0x01)) {
		unsigned char *trailing_buf = buf + buflen - 1;

		dev_warn(ap->dev, "pata_gpio_data_xfer did uneven length xfer!\n");

		if (rw == READ) {
			*trailing_buf = pata_gpio_read16_safe(ap->host->private_data, REG_DATA) & 0xFF;
		} else {
			pata_gpio_write16_safe(ap->host->private_data, REG_DATA, *trailing_buf);
		}
	}

	return buflen;
}

/*
 * pata_gpio_set_devctl - Write device control register
 */
static void pata_gpio_set_devctl(struct ata_port *ap, u8 ctl)
{
	pata_gpio_write16_safe(ap->host->private_data, REG_CTL, ctl);
}

/*
 * pata_gpio_dev_select - Select device on ATA bus
 */
static void pata_gpio_dev_select(struct ata_port *ap, unsigned int device)
{
	u8 tmp = ATA_DEVICE_OBS;

	if (device != 0)
		tmp |= ATA_DEV1;

	pata_gpio_write16_safe(ap->host->private_data, REG_DEVICE, tmp);
	ata_sff_pause(ap);
}

/*
 * pata_gpio_devchk - PATA device presence detection
 */
static bool pata_gpio_devchk(struct ata_port *ap,
				unsigned int device)
{
	u8 nsect, lbal;

	pata_gpio_dev_select(ap, device);

	pata_gpio_write16_safe(ap->host->private_data, REG_NSECT, 0x55);
	pata_gpio_write16_safe(ap->host->private_data, REG_LBAL, 0xAA);

	pata_gpio_write16_safe(ap->host->private_data, REG_NSECT, 0xAA);
	pata_gpio_write16_safe(ap->host->private_data, REG_LBAL, 0x55);

	pata_gpio_write16_safe(ap->host->private_data, REG_NSECT, 0x55);
	pata_gpio_write16_safe(ap->host->private_data, REG_LBAL, 0xAA);

	nsect = pata_gpio_read16_safe(ap->host->private_data, REG_NSECT);
	lbal = pata_gpio_read16_safe(ap->host->private_data, REG_LBAL);

	return ((nsect == 0x55) && (lbal == 0xaa));
}

/*
 * pata_gpio_wait_after_reset - wait for devices to become ready after reset
 */
static int pata_gpio_wait_after_reset(struct ata_link *link,
		unsigned long deadline)
{
	int rc;

	ata_msleep(link->ap, ATA_WAIT_AFTER_RESET);

	/* always check readiness of the master device */
	rc = ata_sff_wait_ready(link, deadline);
	/* -ENODEV means the odd clown forgot the D7 pulldown resistor
	 * and TF status is 0xff, bail out on it too.
	 */
	if (rc)
		return rc;

	return 0;
}

/*
 * pata_gpio_bus_softreset - PATA device software reset
 */
static int pata_gpio_bus_softreset(struct ata_port *ap,
		unsigned long deadline)
{
	/* software reset.  causes dev0 to be selected */
	pata_gpio_write16_safe(ap->host->private_data, REG_CTL, ap->ctl);
	udelay(20);
	pata_gpio_write16_safe(ap->host->private_data, REG_CTL, ap->ctl | ATA_SRST);
	udelay(20);
	pata_gpio_write16_safe(ap->host->private_data, REG_CTL, ap->ctl);
	ap->last_ctl = ap->ctl;

	return pata_gpio_wait_after_reset(&ap->link, deadline);
}

/*
 * pata_gpio_softreset - reset host port via ATA SRST
 */
static int pata_gpio_softreset(struct ata_link *link, unsigned int *classes,
			 unsigned long deadline)
{
	struct ata_port *ap = link->ap;
	int rc;
	u8 err;

	/* issue bus reset */
	rc = pata_gpio_bus_softreset(ap, deadline);
	if (rc && rc != -ENODEV && rc != -EBUSY) {
		ata_link_err(link, "SRST failed (errno=%d)\n", rc);
		return rc;
	}

	/* determine by signature whether we have ATA or ATAPI devices */
	classes[0] = ata_sff_dev_classify(&ap->link.device[0], pata_gpio_devchk(ap, 0), &err);
	classes[1] = ata_sff_dev_classify(&ap->link.device[1], pata_gpio_devchk(ap, 1), &err);

	return 0;
}

static struct scsi_host_template pata_gpio_sht = {
	ATA_PIO_SHT("pata-gpio"),
};

static struct ata_port_operations pata_gpio_port_ops = {
 	.inherits				= &ata_sff_port_ops,
 	.sff_check_status		= pata_gpio_check_status,
 	.sff_check_altstatus	= pata_gpio_check_altstatus,
 	.sff_tf_load			= pata_gpio_tf_load,
 	.sff_tf_read			= pata_gpio_tf_read,
 	.sff_data_xfer			= pata_gpio_data_xfer,
 	.sff_exec_command		= pata_gpio_exec_command,
 	.sff_dev_select			= pata_gpio_dev_select,
 	.sff_set_devctl			= pata_gpio_set_devctl,
 	.softreset				= pata_gpio_softreset,
 };

static int claim_gpios(struct gpio_descs **target, unsigned count, const char *name, enum gpiod_flags flags, struct device *dev)
{
	struct gpio_descs *gpios;

	gpios = devm_gpiod_get_array(dev, name, flags);
	if (!gpios) {
		return -ENOMEM;
	}
	if (gpios->ndescs != count) {
		return -EINVAL;
	}

	*target = gpios;
	return 0;
}

static int pata_gpio_probe(struct platform_device *pdev)
{
	int err;
	struct device *dev = &pdev->dev;
	struct pata_gpio *pata;
	struct ata_host *host;
	struct ata_port *ap;

	pata = devm_kzalloc(dev, sizeof(struct pata_gpio), GFP_KERNEL);
	if (!pata)
		return -ENOMEM;

	pata->dev = dev;

	err = claim_gpios(&pata->databus_gpios, 16, "databus", GPIOD_IN, dev);
	if (err) {
		dev_err(dev, "Failed to request databus gpios: %d\n", err);
		return err;
	}

	err = claim_gpios(&pata->cs_gpios, 2, "cs", GPIOD_OUT_LOW, dev);
	if (err) {
		dev_err(dev, "Failed to request cs gpios: %d\n", err);
		return err;
	}

	err = claim_gpios(&pata->address_gpios, 3, "address", GPIOD_OUT_LOW, dev);
	if (err) {
		dev_err(dev, "Failed to request address gpios: %d\n", err);
		return err;
	}

	pata->reset_gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);

	if (IS_ERR(pata->reset_gpio))
		return PTR_ERR(pata->reset_gpio);

	pata->strobe_read_gpio = devm_gpiod_get(dev, "strobe-read", GPIOD_OUT_LOW);
	if (!pata->strobe_read_gpio)
		return -ENOMEM;

	if (IS_ERR(pata->strobe_read_gpio))
		return PTR_ERR(pata->strobe_read_gpio);

	pata->strobe_write_gpio = devm_gpiod_get(dev, "strobe-write", GPIOD_OUT_LOW);
	if (!pata->strobe_write_gpio)
		return -ENOMEM;
	
	if (IS_ERR(pata->strobe_write_gpio))
		return PTR_ERR(pata->strobe_write_gpio);

	// hard-reset
	if (pata->reset_gpio) {
		gpiod_set_value(pata->reset_gpio, 1);
		udelay(20);
		gpiod_set_value(pata->reset_gpio, 0);
		msleep(100);
	}

	host = ata_host_alloc(&pdev->dev, 1);
	if (!host) {
		dev_err(dev, "failed to allocate ide host\n");
		return -ENOMEM;
	}

	host->private_data = pata;

	ap = host->ports[0];
	ap->ops = &pata_gpio_port_ops;
	ap->pio_mask = ATA_PIO0;
	ap->flags |= ATA_FLAG_PIO_POLLING | ATA_FLAG_SLAVE_POSS;

	return ata_host_activate(host, 0, NULL, 0, &pata_gpio_sht);
}

static const struct of_device_id pata_gpio_dt_ids[] = {
	{ .compatible = "pata-gpio" },
	{}
};
MODULE_DEVICE_TABLE(of, pata_gpio_dt_ids);


static struct platform_driver pata_gpio_driver = {
	.driver = {
		.name	= "pata-gpio",
		.of_match_table = of_match_ptr(pata_gpio_dt_ids),
	},
	.probe		= pata_gpio_probe,
	.remove		= ata_platform_remove_one,
};
module_platform_driver(pata_gpio_driver);

MODULE_DESCRIPTION("PATA driver using generic bitbanged GPIO");
MODULE_AUTHOR("Tobias Schramm, Tobias Maedel");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
