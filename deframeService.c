#include <string.h> // String helper functions
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <errno.h> //errors
#include "encDec.h"

char * deframe(char * in, int data_size) {
	//input file
	int in_fd = open(in, O_RDONLY);
	//input buffer
	char * buffer = calloc(8, sizeof(char));
	
	//Create Data buffer
	char * data = calloc(data_size, sizeof(char));
	//Data index
	int data_index = 0;
	//Reopen input file
	in_fd = open(in, O_RDONLY);
	//Empty buffer
	free(buffer);
	buffer = calloc(8, sizeof(char));
	//Create Flag
	int size_flag = 0;
	//read in one character from input.binf
	int size = read(in_fd, buffer, 8);
	//While there are still Characters in input.binf
	while(size > 0) {
		//TODO Check for error in parity bit... Maybe. Might happen in decodeService
		
		//check for SYN character(bug in parity bit occurs sometimes)
		if(strcmp(buffer, "10010110") == 0 || strcmp(buffer, "00010110") == 0) {
			//we are currently at a parity bit. Set size_flag = 1
			size_flag = 1;
		}
		else if(size_flag == 1) {
			//The current Character is the size character encoded in binary
			//set size_flag = 0, then continue
			size_flag = 0;
		}
		else {
			//Write the data to our data buffer
			int i = 0;
			while(i < strlen(buffer)) {
				data[data_index++] = buffer[i++];
			}
		}
		//read in the next character(8 bit representation) from input.binf
		size = read(in_fd, buffer, 8);
	}
	return data;
}

//Takes a filepath, returns the data from the input
int main(int argc, char ** argv) {
	printf("%s", deframe(argv[0], atoi(argv[1])));
}
