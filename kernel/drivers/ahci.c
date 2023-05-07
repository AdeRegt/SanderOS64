#include "../include/ahci.h"
#include "../include/device.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/fs/mbr.h"

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	0x96690101	// Port multiplier
 
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4
 
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35

#define HBA_PxIS_TFES (1 << 30)

HBAMemory* memory;	

uint8_t ahci_read_sector(HBAPort *hbaPort, upointer_t sector, uint32_t counter, void* buffer){
	uint32_t sectorL = (uint32_t) sector;
	#ifdef __x86_64
	uint32_t sectorH = (uint32_t) (sector >> 32);
	#endif
	#ifndef __x86_64
	uint32_t sectorH = 0;
	#endif

	hbaPort->interruptStatus = (uint32_t)-1;

	HBACommandHeader* cmdHeader = (HBACommandHeader*)(upointer_t)hbaPort->commandListBase;
	cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t);
	cmdHeader->write = 0;
	cmdHeader->prdtLength = 1;

	HBACommandTable* commandTable = (HBACommandTable*)(upointer_t)(cmdHeader->commandTableBaseAddress);
	memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

	commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(upointer_t)buffer;
	#ifdef __x86_64
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((upointer_t)buffer >> 32);
	#endif
	#ifndef __x86_64
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)0;
	#endif
	if(hbaPort->signature==SATA_SIG_ATA){
		commandTable->prdtEntry[0].byteCount = (counter<<9)-1;
		commandTable->prdtEntry[0].interruptOnCompletion = 1;
	}

	FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

	cmdFIS->fisType = FIS_TYPE_REG_H2D;
	cmdFIS->commandControl = 1; // command
	if(hbaPort->signature==SATA_SIG_ATA){
		cmdFIS->command = ATA_CMD_READ_DMA_EX;

		cmdFIS->lba0 = (uint8_t)sectorL;
		cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
		cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
		cmdFIS->lba3 = (uint8_t)sectorH;
		cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
		cmdFIS->lba4 = (uint8_t)(sectorH >> 16);

		cmdFIS->deviceRegister = 1<<6; //LBA mode

		cmdFIS->countLow = counter & 0xFF;
		cmdFIS->countHigh = (counter >> 8) & 0xFF;
	}else if(hbaPort->signature==SATA_SIG_ATAPI){
		cmdFIS->command = 0xA0;
		cmdFIS->featureLow = 1;
		cmdFIS->featureHigh = 0;
		commandTable->prdtEntry[0].interruptOnCompletion = 1;
		cmdHeader->atapi = 1;
		commandTable->atapiCommand[ 0] = 0xA8;
		commandTable->atapiCommand[ 1] = 0;
		commandTable->atapiCommand[ 2] = (sectorL >> 0x18) & 0xFF;
    	commandTable->atapiCommand[ 3] = (sectorL >> 0x10) & 0xFF;
    	commandTable->atapiCommand[ 4] = (sectorL >> 0x08) & 0xFF;
    	commandTable->atapiCommand[ 5] = (sectorL >> 0x00) & 0xFF;
    	commandTable->atapiCommand[ 6] = 0x00;
    	commandTable->atapiCommand[ 7] = 0x00;
    	commandTable->atapiCommand[ 8] = 0x00;
    	commandTable->atapiCommand[ 9] = counter & 0xFF;
    	commandTable->atapiCommand[10] = 0x00;
    	commandTable->atapiCommand[11] = 0x00;
		cmdFIS->countLow = 1;
		cmdFIS->countHigh = 0;
	}

	upointer_t spin = 0;

	while ((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
		spin ++;
	}
	if (spin == 1000000) {
		return 0;
	}

	hbaPort->commandIssue = 1;

	while (1){

		if((hbaPort->commandIssue == 0)){
			break;
		}
		if(hbaPort->interruptStatus & HBA_PxIS_TFES){
			return 0;
		}
	}

	return 1;
}

uint8_t ahci_write_sector(HBAPort *hbaPort, upointer_t sector, uint32_t counter, void* buffer){
	uint32_t sectorL = (uint32_t) sector;
	#ifdef __x86_64
	uint32_t sectorH = (uint32_t) (sector >> 32);
	#endif
	#ifndef __x86_64
	uint32_t sectorH = 0;
	#endif

	hbaPort->interruptStatus = (uint32_t)-1;
	HBACommandHeader* cmdHeader = (HBACommandHeader*)(upointer_t)hbaPort->commandListBase;
	cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t);
	cmdHeader->write = 1;
	cmdHeader->prdtLength = 1;

	HBACommandTable* commandTable = (HBACommandTable*)(upointer_t)(cmdHeader->commandTableBaseAddress);
	memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

	commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(upointer_t)buffer;
	#ifdef __x86_64
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((upointer_t)buffer >> 32);
	#endif
	#ifndef __x86_64
	commandTable->prdtEntry[0].dataBaseAddressUpper = 0;
	#endif
	commandTable->prdtEntry[0].byteCount = (counter<<9)-1;
	commandTable->prdtEntry[0].interruptOnCompletion = 1;

	FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

	cmdFIS->fisType = FIS_TYPE_REG_H2D;
	cmdFIS->commandControl = 1; // command
	cmdFIS->command = ATA_CMD_WRITE_DMA_EX;

	cmdFIS->lba0 = (uint8_t)sectorL;
	cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
	cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
	cmdFIS->lba3 = (uint8_t)sectorH;
	cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
	cmdFIS->lba4 = (uint8_t)(sectorH >> 16);

	cmdFIS->deviceRegister = 1<<6; //LBA mode

	cmdFIS->countLow = counter & 0xFF;
	cmdFIS->countHigh = (counter >> 8) & 0xFF;

	upointer_t spin = 0;

	while ((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
		spin ++;
	}
	if (spin == 1000000) {
		return 0;
	}

	hbaPort->commandIssue = 1;

	while (1){

		if((hbaPort->commandIssue == 0)){
			break;
		}
		if(hbaPort->interruptStatus & HBA_PxIS_TFES){
			return 0;
		}
	}

	return 1;
}

__attribute__((interrupt)) void ahci_interrupt(interrupt_frame* frame){
    k_printf("Interrupt: AHCI-interrupt \n");
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}



void ahci_port_stopCMD(HBAPort *hbaPort){
	hbaPort->cmdSts &= ~HBA_PxCMD_ST;
	hbaPort->cmdSts &= ~HBA_PxCMD_FRE;

	while(1){
		if (hbaPort->cmdSts & HBA_PxCMD_FR) continue;
		if (hbaPort->cmdSts & HBA_PxCMD_CR) continue;

		break;
	}

}

void ahci_port_startCMD(HBAPort *hbaPort){
	while (hbaPort->cmdSts & HBA_PxCMD_CR);

	hbaPort->cmdSts |= HBA_PxCMD_FRE;
	hbaPort->cmdSts |= HBA_PxCMD_ST;
}


void ahci_port_configure(HBAPort *hbaPort){
	ahci_port_stopCMD(hbaPort);

	void* newBase = requestPage();//memory + (1<<10);
	// k_printf("mem clb: %x \n",newBase);
	hbaPort->commandListBase = (uint32_t)(upointer_t)newBase;
	#ifdef __x86_64
	hbaPort->commandListBaseUpper = (uint32_t)((upointer_t)newBase >> 32);
	#endif
	#ifndef __x86_64
	hbaPort->commandListBaseUpper = 0;
	#endif
	memset((void*)(upointer_t)(hbaPort->commandListBase), 0, 1024);

	void* fisBase = requestPage();//memory + (32<<10) + (1<<8);
	// k_printf("mem fis: %x \n",fisBase);
	hbaPort->fisBaseAddress = (uint32_t)(upointer_t)fisBase;
	#ifdef __x86_64
	hbaPort->fisBaseAddressUpper = (uint32_t)((upointer_t)fisBase >> 32);
	#endif
	#ifndef __x86_64
	hbaPort->fisBaseAddressUpper = 0;
	#endif
	memset(fisBase, 0, 256);

	#ifdef __x86_64
	HBACommandHeader* cmdHeader = (HBACommandHeader*)((upointer_t)hbaPort->commandListBase + ((upointer_t)hbaPort->commandListBaseUpper << 32));
	#endif
	#ifndef __x86_64
	HBACommandHeader* cmdHeader = (HBACommandHeader*)((upointer_t)hbaPort->commandListBase);
	#endif
	for (int i = 0; i < 32; i++){
		cmdHeader[i].prdtLength = 8;

		void* cmdTableAddress = requestPage();
		upointer_t address = (upointer_t)cmdTableAddress + (i << 8);
		cmdHeader[i].commandTableBaseAddress = (uint32_t)(upointer_t)address;
		#ifdef __x86_64
		cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((upointer_t)address >> 32);
		#endif
		#ifndef __x86_64
		cmdHeader[i].commandTableBaseAddressUpper = 0;
		#endif
		memset(cmdTableAddress, 0, 256);
	}
	ahci_port_startCMD(hbaPort);
}

char ahci_ata_read(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){
	return ahci_read_sector((HBAPort*)dev->attachment,sector,counter,buffer);
}

char ahci_ata_write(Blockdevice* dev, upointer_t sector, uint32_t counter, void* buffer){
	return ahci_write_sector((HBAPort*)dev->attachment,sector,counter,buffer);
}

void initialise_ahci_driver(unsigned long bar5, unsigned long ints){
	k_printf("AHCI: ints at %x \n",ints);
	setInterrupt(ints,ahci_interrupt);
	memory = (HBAMemory*)bar5;
	uint32_t portsImplemented = memory->portsImplemented;
	for (int i = 0; i < 32; i++){
		if (portsImplemented & (1 << i)){
			HBAPort* port = (HBAPort*)&memory->ports[i];
			k_printf("AHCI: device found at port %d \n",i);
			uint32_t sataStatus = port->sataStatus;
			uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;
        	uint8_t deviceDetection = sataStatus & 0b111;

			if(deviceDetection!=0x3){
				continue;
			}
			if(interfacePowerManagement!=0x1){
				continue;
			}

			if(port->signature==SATA_SIG_ATA||port->signature==SATA_SIG_ATAPI){
				ahci_port_configure(port);
				k_printf("AHCI: Port configured\n");
				char* buffer = (char*) requestPage();
				char xu1 = ahci_read_sector(port,0,1,(void*)buffer);
				if(xu1){
					Blockdevice* dev = registerBlockDevice(512, ahci_ata_read, ahci_ata_write, 3, port);
					initialise_fs(dev,buffer);
				}else{
					k_printf("AHCI: Failed to read testsector!\n");
				}
			}
		}
	}
	k_printf("AHCI: End of operation\n");
}