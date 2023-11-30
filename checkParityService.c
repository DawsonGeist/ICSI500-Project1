#include "encDec.h" //Our Header
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <string.h>

char * checkParity(char * str) {
	char * result = calloc(10, sizeof(char));
	int i = strlen(str) -1;
	int count = 0;
	while(i > -1) {
		count = str[i] == '1' ? count+1 : count;
		i--;
	}
	
	if((count % 2) == 0)  {
		result = "1";
	}
	else {
		result = "ERROR";
	}
	
	return result;
}

int main(int argc, char ** argv) {
	printf("%s", checkParity(argv[0]));
	return 0;
}
