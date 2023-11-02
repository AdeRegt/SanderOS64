
void *draw_pixel(int x,int y,int z){
    int mode = 402;
    void* res = 0;
	__asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(mode) , "b" (x) , "c" (y) , "d" (z) );
    return res;
}
