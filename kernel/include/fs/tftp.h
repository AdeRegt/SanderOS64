#include "../ethernet.h"

#define TFTP_DEFAULT_PORT 69

char* load_tftp_file(char* path);
void tftp_detect_and_initialise();

typedef struct {
    struct UDPHeader udpheader;
    uint16_t operation;
    uint8_t data[50];
}__attribute__((packed)) TFTPRequestFile;

typedef struct {
    struct UDPHeader udpheader;
    uint16_t operation;
    uint16_t block;
}__attribute__((packed)) TFTPResponseFile;

typedef struct {
    struct UDPHeader udpheader;
    uint16_t operation;
    uint8_t tag[6];
    uint8_t value[10];
}__attribute__((packed)) TFTPOptionFile;