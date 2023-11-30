#include <string.h> // String helper functions
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <errno.h> //errors

char * es = "/home/dgeist/Project1/encodeService";
char * ds = "/home/dgeist/Project1/decodeService";
char * dfs = "/home/dgeist/Project1/deframeService";

char * StringToIntToSingleCharString(char * str) {
	char * newStr = calloc(1, sizeof(char));
	char temp[1];
	int test = atoi(str);
	temp[0] = (char)test;
	strcpy(newStr, temp);
	return newStr;
}

char * ConvertSize(int size) {
	char * strSize = calloc(2, sizeof(char));
	sprintf(strSize, "%d", size);
	return StringToIntToSingleCharString(strSize);
}

char * IntToStr(int size) {
	char * strSize = calloc(2, sizeof(char));
	sprintf(strSize, "%d", size);
	return strSize;
}

//Consumer is called (filename)
int main(int argc, char ** argv) {
	//printf("Entering Consumer!\n");
	
	//Calculate how much data is in the file
	int in_fd = open(argv[0], O_RDONLY);
	//Size of input.binf
	int data_size = 0;
	//input buffer
	char * buffer = calloc(8, sizeof(char));
	//read in one character from input.binf
	int size = read(in_fd, buffer, 8);
	//While there are still Characters in input.binf
	while(size > 0) {
		//Update size
		data_size += size;
		//Empty buffer
		free(buffer);
		buffer = calloc(8, sizeof(char));
		size = read(in_fd, buffer, 8); 
	}
	//close in_fd to reset the pointer in the file
	close(in_fd);
	//Release buffer
	free(buffer);
	buffer = (char *)NULL;
	
	//Delete Existing files from last run
	remove("output.outf");
	remove("output.chck");
	//Decoded and Capitalized Input
	int out_c = open("output.outf", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXO);
	//Encoded, Framed, Capitalized Input
	int out_b = open("output.chck", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXO);
	
	//Prepare our data buffer
	char * data = calloc(data_size, sizeof(char));
	
	//Call Deframe
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {argv[0], IntToStr(data_size), NULL};
		//ReRoute STDOUT to p
		dup2(p[1], STDOUT_FILENO);
		close(p[1]);
		//Call DeFrame Service
		execv(dfs, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		read(p[0], data, data_size);
		close(p[0]);
	}
	
	//printf("DATA FROM INPUT FILE: %s\n", data);
	
	//decode the data
	//Prepare our data buffer
	char * strData = calloc(data_size/8, sizeof(char));
	//Call Decode
	int p1[2];
	pipe(p1);
	pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {data, NULL};
		//ReRoute STDOUT to p
		dup2(p1[1], STDOUT_FILENO);
		close(p1[1]);
		//Call Decode Service
		execv(ds, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		read(p1[0], strData, data_size/8);
		close(p1[0]);
	}
	//printf("DATA FROM INPUT FILE DECODED: %s\n", strData);
	
	//Release data
	free(data);
	data = (char*) NULL;
	
	//Convert Characters to uppercase
	int strData_index = 0;
	int tempCharValue = 0;
	while(strData_index < strlen(strData)) {
		//Get the integer value of the character
		tempCharValue = (int)strData[strData_index];
		if(96 < tempCharValue && tempCharValue < 123) {
			strData[strData_index] = (char)(tempCharValue - 32);
		}
		strData_index++;
	}
	
	//printf("DATA FROM INPUT FILE DECODED AND CAPITALIZED: %s\n", strData);
	
	//Write Decoded Data to .outf
	write(out_c, strData, strlen(strData));
	close(out_c);
	
	//Release strData
	free(strData);
	strData = (char*) NULL;
	
	
	
	//Encode the data
	//Decoded and Capitalized Input
	int in = open("output.outf", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXO);
	
	//Build buffer
	buffer = (char*)calloc(64, sizeof(char));
	
	//Build Frame
	//frame_size = # chars * 8 bits per char + 16 bits for SYN + 8 bits for Size + '\0'
	int frame_size = (64*8+16+8)+1;
	char * frame = calloc(frame_size, sizeof(char));
	
	//Build SYN Character
	char * c_syn = StringToIntToSingleCharString("22");
	strcat(c_syn,c_syn);
	
	//Read in 64 characters
	size = read(in, buffer, 64);
	//Debugging
	//printf("CHARACTER AT 63: %d\n", (int)buffer[63]);
	
	int i = 0;
	while(size > 0) {
		//printf("Size %d\nBuffer Contents: %s\n", size, buffer);
		//Call Encode Service
		int p[2];
		pipe(p);
		int pid = fork();
		if(pid<0) {
			printf("Error in fork\n");
		}
		else if(pid == 0) {
			//Build Args
			char * const args[] = {c_syn, ConvertSize(size), buffer, "0", NULL};
			//ReRoute STDOUT to p
			dup2(p[1], STDOUT_FILENO);
			close(p[1]);
			//Call encode service
			execv(es, args);
			printf("Error in Exec\n");
		}
		else {
			wait(NULL);
			//Read frame from encode service
			free(frame);
			frame = calloc(frame_size, sizeof(char));
			read(p[0], frame, frame_size);
			close(p[0]);
		}
		
		//Write Frame to outputFP
		//printf("Wrote %d bytes\n", (int)write(out_b, frame, strlen(frame)));
		write(out_b, frame, strlen(frame));
		//printf("%s\n", strerror(errno));
		//printf("\nFrame %d: %s\n", i, frame);
		//Repeat untill end of input
		free(buffer);
		buffer = (char*)calloc(64, sizeof(char));
		size = read(in, buffer, 64);
		//printf("SIZE %d\n", size);
		i++;
	}
	close(in);
	close(out_b);
	
	//Print out name of binary file
	printf("output.chck");
	
	//EXIT
	//printf("Exiting Consumer!\n");
	return 0;
}
