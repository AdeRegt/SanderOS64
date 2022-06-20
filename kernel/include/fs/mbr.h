#pragma once
#include <stdint.h>

typedef struct{
	uint8_t drive_attr;
	uint8_t chsstr[3];
	uint8_t type;
	uint8_t chsend[3];
	uint32_t lbastart;
	uint32_t seccnt;
}mbr_entry;

mbr_entry* getMBRRecordsFromBuffer(void *buffer);