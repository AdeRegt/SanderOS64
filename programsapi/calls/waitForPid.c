void waitForPid(int pid)
{
    int modus = 416;
    __asm__ __volatile__( "int $0x81" :  : "a"(modus) , "b" (pid) );
}