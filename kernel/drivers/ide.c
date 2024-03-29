#include "../include/ide.h"
#include "../include/graphics.h"
#include "../include/SCSI.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/idt.h"
#include "../include/pci.h"
#include "../include/timer.h"
#include "../include/device.h"
#include "../include/fs/mbr.h"
#include "../include/fs/iso9660.h"

unsigned short ide_port_primary = 0;
unsigned short ide_port_secondary = 0;

IDEDevice ata1 = {.command = 0x1f0, .control = 0x3f6, .irq = 14, .slave = 0};
IDEDevice ata2 = {.command = 0x1f0, .control = 0x3f6, .irq = 14, .slave = 1};
IDEDevice ata3 = {.command = 0x170, .control = 0x3f6, .irq = 15, .slave = 0};
IDEDevice ata4 = {.command = 0x170, .control = 0x376, .irq = 15, .slave = 1};

volatile int ideXirq = 0;
__attribute__((interrupt)) void irq_ide(interrupt_frame* frame)
{
	k_printf("ide: ACK int\n");
	inportb(0x177); // acknowledge second
	inportb(0x1F7);	// acknowledge first
	outportb(0x20, 0x20);
	outportb(0xA0, 0x20);
	((uint8_t*)&ideXirq)[0] = 1;
}

void resetIDEFire()
{
	ideXirq = 0;
}

void waitForIDEFire()
{
	while (ideXirq == 0)
    {
        sleep(1);
    }
}

char getIDEError(IDEDevice cdromdevice)
{
	unsigned char msg = inportb(cdromdevice.command + 7);
	if ((msg >> 0) & 1)
	{
		if (msg & 0x80)
		{
			k_printf("ide: Bad sector\n");
		}
		else if (msg & 0x40)
		{
			k_printf("ide: Uncorrectable data\n");
		}
		else if (msg & 0x20)
		{
			k_printf("ide: No media\n");
		}
		else if (msg & 0x10)
		{
			k_printf("ide: ID mark not found\n");
		}
		else if (msg & 0x08)
		{
			k_printf("ide: No media\n");
		}
		else if (msg & 0x04)
		{
			k_printf("ide: Command aborted\n");
		}
		else if (msg & 0x02)
		{
			k_printf("ide: Track 0 not found\n");
		}
		else if (msg & 0x01)
		{
			k_printf("ide: No address mark\n");
		}
		return 1;
	}
	return 0;
}

void ide_wait_for_ready(IDEDevice cdromdevice)
{
	unsigned char dev = 0x00;
	while ((dev = inportb(cdromdevice.command + 7)) & ATA_SR_BSY)
	{
		if (dev & ATA_SR_DF)
		{
			k_printf("IDE: ERROR1\n");
			for (;;)
				;
		}
		if (dev & ATA_SR_ERR)
		{
			k_printf("IDE: ERROR2\n");
			for (;;)
				;
		}
		sleep(2);
	}
}

char read_cmd[12] = {SCSI_READ_12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t ide_atapi_read(Blockdevice* dev, upointer_t lba, uint32_t counter, void* buffer){

    IDEDevice cdromdevice = (IDEDevice)((IDEDevice*)dev->attachment)[0];

	resetIDEFire();
    
	getIDEError(cdromdevice);
	ide_wait_for_ready(cdromdevice);

	outportb(cdromdevice.command + 6, cdromdevice.slave == 1 ? 0xB0 : 0xA0);
	outportb(cdromdevice.command + 1, 0x00);
	outportb(cdromdevice.command + 4, ATAPI_SECTOR_SIZE & 0xff);
	outportb(cdromdevice.command + 5, ATAPI_SECTOR_SIZE >> 8);
	outportb(cdromdevice.command + 7, 0xA0);

	getIDEError(cdromdevice);

	ide_wait_for_ready(cdromdevice);

	for(int i = 0 ; i < 12 ; i++){
		read_cmd[i] = 0;
	}

	read_cmd[9] = counter;
	read_cmd[2] = (lba >> 0x18) & 0xFF; /* most sig. byte of LBA */
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;
	read_cmd[0] = SCSI_READ_12;

	resetIDEFire();
	getIDEError(cdromdevice);
	ide_wait_for_ready(cdromdevice);
	unsigned short *mdx = (unsigned short *)&read_cmd;
	ide_wait_for_ready(cdromdevice);
	for (int f = 0; f < 6; f++)
	{
		outportw(cdromdevice.command + 0, mdx[f]);
	}
	getIDEError(cdromdevice);
	waitForIDEFire();
	ide_wait_for_ready(cdromdevice);
	unsigned short size = (((int)inportb(cdromdevice.command + 5)) << 8) | (int)(inportb(cdromdevice.command + 4));
	ide_wait_for_ready(cdromdevice);
	int mp = 0;
	for (unsigned short i = 0; i < (size / 2); i++)
	{
		if (getIDEError(cdromdevice) == 1)
		{
			return 0;
		}
		ide_wait_for_ready(cdromdevice);
		((uint16_t*)buffer)[mp++] = inportw(cdromdevice.command + 0);
	}
	ide_wait_for_ready(cdromdevice);
	waitForIDEFire();
	return 1;
}

uint8_t ide_atapi_eject(IDEDevice cdromdevice){
   	getIDEError(cdromdevice);

	ide_wait_for_ready(cdromdevice);
	outportb(cdromdevice.command + 6, cdromdevice.slave == 1 ? 0xB0 : 0xA0);
	outportb(cdromdevice.command + 1, 0x00);
	outportb(cdromdevice.command + 7, 0xA0);

	getIDEError(cdromdevice);
	ide_wait_for_ready(cdromdevice);

	read_cmd[0] = 0x1B;
	read_cmd[1] = 0x00;
	read_cmd[2] = 0x00;
	read_cmd[3] = 0x00;
	read_cmd[4] = 0x02;
	read_cmd[5] = 0x00;
	read_cmd[6] = 0x00;
	read_cmd[7] = 0x00;
	read_cmd[8] = 0x00;
	read_cmd[9] = 0x00;
	read_cmd[10] = 0x00;
	read_cmd[11] = 0x00;

	resetIDEFire();
	getIDEError(cdromdevice);
	ide_wait_for_ready(cdromdevice);
	unsigned short *mdx = (unsigned short *)&read_cmd;
	for (int f = 0; f < 6; f++)
	{
		outportw(cdromdevice.command + 0, mdx[f]);
	}
	getIDEError(cdromdevice);
	waitForIDEFire();
	ide_wait_for_ready(cdromdevice);
	getIDEError(cdromdevice);
	ide_wait_for_ready(cdromdevice);
}

uint8_t ide_atapi_write(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){
    return 0; // writes are not supported, we are read only!
}

uint8_t ide_ata_read(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){

    IDEDevice cdromdevice = (IDEDevice)((IDEDevice*)dev->attachment)[0];

	resetIDEFire();
	outportb(cdromdevice.command + 6, 0xE0 | (cdromdevice.slave << 4) | ((sector >> 24) & 0x0F));
	outportb(cdromdevice.command + 2, (unsigned char)counter);
	outportb(cdromdevice.command + 3, (unsigned char)sector);
	outportb(cdromdevice.command + 4, (unsigned char)(sector >> 8));
	outportb(cdromdevice.command + 5, (unsigned char)(sector >> 16));
	outportb(cdromdevice.command + 7, 0x20);
	waitForIDEFire();
	int U = 0;
	int i = 0;
	for (i = 0; i < (512 / 2); i++)
	{
		unsigned short X = inportw(cdromdevice.command);
		((unsigned short*)buffer)[U++] = X;
	}

    return 1;
}

uint8_t ide_ata_write(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){
    return 0; // writes are not supported, we are read only!
}

void init_ide_device(IDEDevice device)
{
	setInterrupt(device.irq, irq_ide);

	resetIDEFire();

	outportb(device.command + 6, device.slave == 1 ? 0xB0 : 0xA0);
	outportb(device.command + 2, 0);
	outportb(device.command + 3, 0);
	outportb(device.command + 4, 0);
	outportb(device.command + 5, 0);
	outportb(device.command + 7, 0xEC);

	sleep(2);
	if (inportb(device.command + 7) == 0)
	{
		return;
	}

	while (1)
	{
		if ((inportb(device.command + 7) & 0x80) > 0)
		{
			break;
		}
		else if (!(inportb(device.command + 4) == 0 && inportb(device.command + 5) == 0))
		{
			break;
		}
		else if(ideXirq)
		{
			break;
		}
	}

	sleep(2);

	if (inportb(device.command + 4) == 0x3C || inportb(device.command + 5) == 0xC3)
	{
		k_printf("ide: Device is SATA\n");
		return;
	}

	if (inportb(device.command + 4) == 0 && inportb(device.command + 5) == 0)
	{
		k_printf("ide: device is ATA\n");
		unsigned char *identbuffer = (unsigned char *) requestPage();
		for (int i = 0; i < 256; i++)
		{
			unsigned short datapart = inportw(device.command);
			unsigned char datapartA = (datapart>>8) & 0xFF;
			unsigned char datapartB = datapart & 0xFF;
			identbuffer[(i*2)+0] = datapartA;
			identbuffer[(i*2)+1] = datapartB;
		}
		IDE_IDENTIFY *ident = (IDE_IDENTIFY*) identbuffer;
		ident->unused2[0] = 0;
		ident->unused3[0] = 0;
		k_printf("ide: ATA version=%s name=%s \n",ident->version,ident->name);

		Blockdevice *regdev = (Blockdevice*)registerBlockDevice(512, ide_ata_read, ide_ata_write, 0, (void*)&device);
		initialise_fs(regdev,identbuffer);
		
		// ATA device detected!
	}

	if( inportb(device.command+4)==0x14 && inportb(device.command+5)==0xEB )
	{
		k_printf("ide: ATAPI\n");
		for (int i = 0; i < 256; i++)
		{
			inportw(device.command);
		}

		resetIDEFire();

		outportb(device.command + 6, device.slave == 1 ? 0xB0 : 0xA0);
		outportb(device.command + 2, 0);
		outportb(device.command + 3, 0);
		outportb(device.command + 4, 0);
		outportb(device.command + 5, 0);
		outportb(device.command + 7, 0xA1);

		sleep(2);
		getIDEError(device);
		ide_wait_for_ready(device);

		if (inportb(device.command + 7) == 0)
		{
			return;
		}
		if (getIDEError(device) == 0)
		{
			k_printf("ide: device is ATAPI\n");
			unsigned char *identbuffer = (unsigned char *) requestPage();
			for (int i = 0; i < 256; i++)
			{
				unsigned short datapart = inportw(device.command);
				unsigned char datapartA = (datapart>>8) & 0xFF;
				unsigned char datapartB = datapart & 0xFF;
				identbuffer[(i*2)+0] = datapartA;
				identbuffer[(i*2)+1] = datapartB;
			}
			IDE_IDENTIFY *ident = (IDE_IDENTIFY*) identbuffer;
			ident->unused2[0] = 0;
			ident->unused3[0] = 0;
			k_printf("ide: ATAPI version=%s name=%s \n",ident->version,ident->name);
			// ide_atapi_eject(device);

			Blockdevice *regdev = (Blockdevice*)registerBlockDevice(ISO9660_LOGICAL_BLOCK_SIZE, ide_atapi_read, ide_atapi_write, 0, (void*)&device);
			initialise_iso9660(regdev);
		}
		
	}

}

void ide_driver_start(int bus,int slot,int function){
    ide_port_primary = getBARaddress(bus,slot,function,0x10);
    ide_port_secondary = getBARaddress(bus,slot,function,0x14);
    if(!ide_port_primary){
        k_printf("ide: warning, primary port not defined, defaulting at standard port.\n");
        ide_port_primary = 0x1f0;
    }
    if(!ide_port_secondary){
        k_printf("ide: warning, secondary port not defined, defaulting at standard port.\n");
        ide_port_secondary = 0x170;
    }
    init_ide_device(ata1);
    init_ide_device(ata2);
    init_ide_device(ata3);
    init_ide_device(ata4);
}
