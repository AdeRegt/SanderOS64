void __stack_chk_fail(){
    hang(__FUNCTION__);
    for(;;);
}