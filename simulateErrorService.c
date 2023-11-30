#include "encDec.h" //Our Header
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <string.h>

char * simulateError(char * data) {
	int max = strlen(data) - 1;
	int err = 0;
	for(int i = 0; i < 5; i++) {
	  	err = (int)getpid() % max;
		data[err] = '1';
	}
	return data;
}

int main(int argc, char ** argv) {
	printf("%s", simulateError(argv[0]));
}
