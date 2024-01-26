
int strcmp ( const char * a, const char * b ){
    register const unsigned char *s1 = (const unsigned char *) a;
    register const unsigned char *s2 = (const unsigned char *) b;
    unsigned char reg_char; 
    unsigned char c1; 
    unsigned char c2;

    do{
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        if (c1 == '\0'){
            return c1 - c2;
        }
    }while (c1 == c2);

    return c1 - c2;
}
