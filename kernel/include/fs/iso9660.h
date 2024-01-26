#include "../../include/device.h"

// https://wiki.osdev.org/ISO

#define ISO9660_VOLUME_DESCRIPTORS_BOOT_RECORD 0
#define ISO9660_VOLUME_DESCRIPTORS_PRIMARY_VOLUME_DESCRIPTOR 1
#define ISO9660_VOLUME_DESCRIPTORS_SUPPLEMENTARY_VOLUME_DESCRIPTOR 2
#define ISO9660_VOLUME_DESCRIPTORS_VOLUME_PARTITION_DESCRIPTOR 3
#define ISO9660_VOLUME_DESCRIPTORS_VOLUME_DESCRIPTOR_SET_TERMINATOR 255

#define ISO9660_PROTOCOL_VERSION 1

#define ISO9660_LOGICAL_BLOCK_SIZE 2048

typedef struct {
    uint8_t type;
    uint8_t identifier[5];
    uint8_t version;
}__attribute__((packed)) ISO9660FieldDescriptor;

typedef struct{
    ISO9660FieldDescriptor signature;
    uint8_t unused1;
    uint8_t system_identifier[32];
    uint8_t volume_identifier[32];
    uint8_t unused2[8];
    uint64_t volume_space_size;
    uint8_t unused3[32];
    uint32_t volume_set_size;
    uint32_t volume_sequence_number;
    uint32_t logical_block_size;
    uint64_t path_table_size;
    uint32_t location_of_type_L_path_table;
    uint32_t location_of_the_optional_type_L_path_table;
    uint32_t location_of_the_M_path_table;
    uint32_t location_of_the_optional_type_M_path_table;
    uint8_t directory_entry_for_the_root_directory[34];
    uint8_t volume_set_identifier[128];
    uint8_t publisher_identifier[128];
    uint8_t data_preparer_identifier[128];
    uint8_t application_identifier[128];
    uint8_t copyright_file_identifier[37];
    uint8_t abstract_file_identifier[37];
    uint8_t bibliographic_file_identifier[37];
    uint8_t volume_creation_date_and_time[17];
    uint8_t volume_modification_date_and_time[17];
    uint8_t volume_expiration_date_and_time[17];
    uint8_t volume_effective_date_and_time[17];
    uint8_t file_structure_version;
}__attribute__((packed)) ISO9660PrimaryVolumeDescriptor;

typedef struct{
    ISO9660PrimaryVolumeDescriptor* descriptor;
    void *directorytables;
    void *ideinfo;
}__attribute__((packed)) ISO9660Argument;

void initialise_iso9660(Blockdevice* dev);
unsigned long switch_endian32(unsigned long num);