typedef struct{
	unsigned long buffersize;
	unsigned long low_buf;
	unsigned long high_buf;
}PackageRecievedDescriptor;

void *draw_pixel(int x,int y,int z);
unsigned char scan_for_character();
unsigned char wait_for_character();
void *get_ip_from_name(uint8_t *name);
void *get_mac_from_ip(uint8_t *name);
void *start_tcp_session(uint8_t *ip,uint16_t port,void *functionp);
void *send_tcp_message(uint8_t *ip,uint16_t port,void *buffer,int size);
char *itos(unsigned int num, int base);