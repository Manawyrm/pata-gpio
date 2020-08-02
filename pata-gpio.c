#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>

struct pata_gpio {
	struct gpio_descs *led_gpios;
	struct gpio_descs *databus_gpios;
	struct gpio_desc *reset_gpio;
	struct gpio_descs *cs_gpios;
	struct gpio_descs *address_gpios;
	struct gpio_desc *strobe_write_gpio;
	struct gpio_desc *strobe_read_gpio;
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
	int nb, i, err;
	struct device *dev = &pdev->dev;
	struct pata_gpio *pata;

	pata = devm_kzalloc(dev, sizeof(struct pata_gpio), GFP_KERNEL);
	if (!pata) {
		return -ENOMEM;
	}

	err = claim_gpios(&pata->led_gpios, 4, "led", 0, dev);
	if (err) {
		dev_err(dev, "Failed to request led gpios: %d\n", err);
		return err;
	}

	err = claim_gpios(&pata->databus_gpios, 16, "databus", 0, dev);
	if (err) {
		dev_err(dev, "Failed to request databus gpios: %d\n", err);
		return err;
	}

	err = claim_gpios(&pata->cs_gpios, 2, "cs", 0, dev);
	if (err) {
		dev_err(dev, "Failed to request cs gpios: %d\n", err);
		return err;
	}

	err = claim_gpios(&pata->address_gpios, 3, "address", 0, dev);
	if (err) {
		dev_err(dev, "Failed to request address gpios: %d\n", err);
		return err;
	}

	pata->reset_gpio = devm_gpiod_get(dev, "reset", 0);
	if (!pata->reset_gpio) {
		return -ENOMEM;
	}
	if (IS_ERR(pata->reset_gpio)) {
		return PTR_ERR(pata->reset_gpio);
	}

	pata->strobe_read_gpio = devm_gpiod_get(dev, "strobe-read", 0);
	if (!pata->strobe_read_gpio) {
		return -ENOMEM;
	}
	if (IS_ERR(pata->strobe_read_gpio)) {
		return PTR_ERR(pata->strobe_read_gpio);
	}

	pata->strobe_write_gpio = devm_gpiod_get(dev, "strobe-write", 0);
	if (!pata->strobe_write_gpio) {
		return -ENOMEM;
	}
	if (IS_ERR(pata->strobe_write_gpio)) {
		return PTR_ERR(pata->strobe_write_gpio);
	}

	dev_info(dev, "miau :3\n");

/*
			gpiod_set_value_cansleep(led, 1);
						usleep_range(1000000, 10000000); 

 */ 

	return 0;
}

static const struct of_device_id pata_gpio_dt_ids[] = {
	{ .compatible = "pata-gpio" },
	{}
};
MODULE_DEVICE_TABLE(of, pata_gpio_dt_ids);


static struct platform_driver spi_gpio_driver = {
	.driver = {
		.name	= "pata-gpio",
		.of_match_table = of_match_ptr(pata_gpio_dt_ids),
	},
	.probe		= pata_gpio_probe,
};
module_platform_driver(spi_gpio_driver);

MODULE_DESCRIPTION("PATA driver using generic bitbanged GPIO");
MODULE_AUTHOR("Tobias Schramm, Tobias Maedel");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");