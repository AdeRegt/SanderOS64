
char *getenv(char *name){
    int modus = 410;
    int enos = 0;
    void* res = 0;
    __asm__ __volatile__( "int $0x81" : "=a"(res) : "a"(modus) , "d" (name) );
    return res;
}
