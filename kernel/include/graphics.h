
typedef unsigned long long size_t;
typedef struct {
	void* BaseAddress;
	size_t BufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
} GraphicsInfo;

void set_graphics_info(GraphicsInfo *gi);
void clear_screen(unsigned int colour);
void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour);
void printString(char* message);
void k_printf(char* format,...);
void initialise_graphics_driver();
void putc(char deze);
