#pragma once
#include <stdint.h>

typedef struct{
	uint8_t drive_attr;
	uint8_t chsstr[3];
	uint8_t type;
	uint8_t chsend[3];
	uint32_t lbastart;
	uint32_t seccnt;
}__attribute__((packed)) mbr_entry;

typedef struct{
	uint8_t Partition_Type_GUID[16];
	uint8_t Unique_Partition_GUID[16];
	uint64_t StartingLBA;
	uint64_t EndingLBA;
	uint64_t Attributes;
	uint8_t Partition_Name[72];
}__attribute__((packed)) efi_entry;

mbr_entry* getMBRRecordsFromBuffer(void *buffer);
void initialise_fs(Blockdevice *dev,void* buffer);