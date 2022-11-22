typedef struct {
    unsigned char address[4];
    unsigned short port;
    void *function;
}__attribute__((packed)) INetRequest;

void *draw_pixel(int x,int y,int z);
unsigned char scan_for_character();
unsigned char wait_for_character();

void *setup_tcp_connection(INetRequest *tcpinfo);