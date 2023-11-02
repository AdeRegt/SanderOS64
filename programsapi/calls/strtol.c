#include <string.h> 

long int strtol(const char *str, char **endptr, int base){
    long int result = 0;
	int pointer = strlen(str)-1;
	int min = -1;
	if(base==16){
		for(size_t i = 0 ; i < strlen(str) ; i++){
			if(str[i]=='x'){
				min = i;
			}
		}
	}
	int power = 1;
	for(int i = pointer ; i > min ; i--){
		char deze = str[i];
		if(base==10){
			if(deze>='0'&&deze<='9'){
				char t = deze-'0';
				result += (t*power);
			}
			power *= 10;
		}else if(base==16){
			int t = 0;
			if(deze>='0'&&deze<='9'){
				t = deze-'0';
			}else if(deze>='A'&&deze<='Z'){
				t = 10+(deze-'A');
			}else if(deze>='a'&&deze<='z'){
				t = 10+(deze-'a');
			}
			result += t*(16^(power-1));
			power++;
		}
	}
	*endptr = (char*) str;
	return result;
}
