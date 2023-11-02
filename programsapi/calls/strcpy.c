
char* strcpy(char* destination, const char* source){
    int size = strlen(source);
    memcpy(destination,source,size);
    return destination;
}
