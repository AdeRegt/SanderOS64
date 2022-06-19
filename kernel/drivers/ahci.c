#include "../include/ahci.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/ports.h"

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

#define HBA_PxIS_TFES (1 << 30)

HBAMemory* memory;	

uint8_t ahci_read_sector(HBAPort *hbaPort, uint64_t sector, uint32_t counter, void* buffer){
	uint32_t sectorL = (uint32_t) sector;
	uint32_t sectorH = (uint32_t) (sector >> 32);

	hbaPort->interruptStatus = (uint32_t)-1;

	HBACommandHeader* cmdHeader = (HBACommandHeader*)(uint64_t)hbaPort->commandListBase;
	cmdHeader->commandFISLength = sizeof(FIS_REG_H2D)/ sizeof(uint32_t);
	cmdHeader->write = 0;
	cmdHeader->prdtLength = 1;

	HBACommandTable* commandTable = (HBACommandTable*)(uint64_t)(cmdHeader->commandTableBaseAddress);
	memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength-1)*sizeof(HBAPRDTEntry));

	commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
	commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
	commandTable->prdtEntry[0].byteCount = (counter<<9)-1;
	commandTable->prdtEntry[0].interruptOnCompletion = 1;

	FIS_REG_H2D* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

	cmdFIS->fisType = FIS_TYPE_REG_H2D;
	cmdFIS->commandControl = 1; // command
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

	uint64_t spin = 0;

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

	void* newBase = requestPage();
	hbaPort->commandListBase = (uint32_t)(uint64_t)newBase;
	hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)newBase >> 32);
	memset((void*)(uint64_t)(hbaPort->commandListBase), 0, 1024);

	void* fisBase = requestPage();
	hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
	hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);
	memset(fisBase, 0, 256);

	HBACommandHeader* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

	for (int i = 0; i < 32; i++){
		cmdHeader[i].prdtLength = 8;

		void* cmdTableAddress = requestPage();
		uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
		cmdHeader[i].commandTableBaseAddress = (uint32_t)(uint64_t)address;
		cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)address >> 32);
		memset(cmdTableAddress, 0, 256);
// k_printf("we came hereE...\n");
	}
	ahci_port_startCMD(hbaPort);
}

void initialise_ahci_driver(unsigned long bar5, unsigned long ints){
	setInterrupt(ints,ahci_interrupt);
	memory = (HBAMemory*)bar5;
	uint32_t portsImplemented = memory->portsImplemented;
	for (int i = 0; i < 32; i++){
		if (portsImplemented & (1 << i)){
			HBAPort* port = (HBAPort*)&memory->ports[i];
			k_printf("AHCI: device found at port %d \n",i);
			if(memory->ports[i].signature==SATA_SIG_ATA||memory->ports[i].signature==SATA_SIG_ATAPI){
				ahci_port_configure(port);
				k_printf("Port configured\n");
			}
			if(memory->ports[i].signature==SATA_SIG_ATA){
				k_printf("Found the hdd\n");
				char *bufpo = (char*)requestPage();
				ahci_read_sector(port,1,4,bufpo);
			}else if(memory->ports[i].signature==SATA_SIG_ATAPI){
				k_printf("Found the cdrom\n");
			}
		}
	}
	k_printf("[AHCI] End of operation\n");for(;;);
}