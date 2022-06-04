void kernel_main(){
    ((unsigned long long*)0xb8000)[0] ='A';
    for(;;);
}