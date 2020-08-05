#define SECTOR_SIZE 512
#define CF_BASE_REG 0
#define CF_DATA     CF_BASE_REG + 0 // IDE Data Port
#define CF_ERROR    CF_BASE_REG + 1 // Error code (read)
#define CF_FEATURE  CF_BASE_REG + 1 // Feature (write)
#define CF_NUMSECT  CF_BASE_REG + 2 // Numbers of sectors to transfer
#define CF_ADDR0    CF_BASE_REG + 3 // Sector address LBA 0 (0:7)
#define CF_ADDR1    CF_BASE_REG + 4 // Sector address LBA 1 (8:15)
#define CF_ADDR2    CF_BASE_REG + 5 // Sector address LBA 2 (16:23)
#define CF_ADDR3    CF_BASE_REG + 6 // Sector address LBA 3 (24:27)
#define CF_STATUS   CF_BASE_REG + 7 // Status (read)
#define CF_COMMAND  CF_BASE_REG + 7 // Command (write)
#define CF_ADDR3_ADDITIONAL 0xE0 // Drive 0 aka Master, LBA addressing, bit 5 and 7 at 1 (as per standard)

int cf_identify(struct pata_gpio *pata, u8* data)
{
	/*
		https://wiki.osdev.org/ATA_PIO_Mode:
		To use the IDENTIFY command, select a target drive by sending 0xA0 for the master drive, or 0xB0 for the slave, 
		to the "drive select" IO port. On the Primary bus, this would be port 0x1F6. Then set the Sectorcount, LBAlo, LBAmid, 
		and LBAhi IO ports to 0 (port 0x1F2 to 0x1F5). Then send the IDENTIFY command (0xEC) to the Command IO port (0x1F7). 
		Then read the Status port (0x1F7) again. If the value read is 0, the drive does not exist. For any other value: 
		poll the Status port (0x1F7) until bit 7 (BSY, value = 0x80) clears. Because of some ATAPI drives that do not follow spec, 
		at this point you need to check the LBAmid and LBAhi ports (0x1F4 and 0x1F5) to see if they are non-zero. 
		If so, the drive is not ATA, and you should stop polling. 
		Otherwise, continue polling one of the Status ports until bit 3 (DRQ, value = 8) sets, or until bit 0 (ERR, value = 1) sets.
		At that point, if ERR is clear, the data is ready to read from the Data port (0x1F0). Read 256 16-bit values, and store them.
	*/

	int i; 
	u16 result; 
	
	pata_gpio_write16(pata, CF_ADDR3, 0xA0);

	pata_gpio_write16(pata, CF_NUMSECT, 0x00);
	pata_gpio_write16(pata, CF_ADDR0, 0x00);
	pata_gpio_write16(pata, CF_ADDR1, 0x00);
	pata_gpio_write16(pata, CF_ADDR2, 0x00);

	pata_gpio_write16(pata, CF_COMMAND, 0xEC);

	pata_gpio_read16(pata, CF_STATUS, &result);
	if (result != 0x00)
	{
		usleep_range(1000000, 1000000);

		for (i = 0; i < SECTOR_SIZE; i+=2)
		{
			pata_gpio_read16(pata, CF_DATA, &result);
			*data++ = (result >> 8) & 0xFF;  
			*data++ = result & 0xFF;
		}

		return 1;
	}
	else
	{
		//printf("No device detected!\n");
		return 0; 
	}
}

