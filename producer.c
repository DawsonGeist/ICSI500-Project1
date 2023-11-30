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
char * c = "/home/dgeist/Project1/consumer";

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


int main(int argc, char ** argv) {
	// Read the input file
	int in = open("input.inpf", O_RDONLY);
	//Delete the previous output file
	remove("input.binf");
	int out_b = open("input.binf", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXO);
		
	//Build buffer
	char * buffer = (char*)calloc(64, sizeof(char));
	
	//Build Frame
	//frame_size = # chars * 8 bits per char + 16 bits for SYN + 8 bits for Size + '\0'
	int frame_size = (64*8+16+8)+1;
	char * frame = calloc(frame_size, sizeof(char));
	
	//Build SYN Character
	char * c_syn = StringToIntToSingleCharString("22");
	strcat(c_syn,c_syn);
	
	//Read in 64 characters
	int size = read(in, buffer, 64);
	
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
			char * const args[] = {c_syn, ConvertSize(size), buffer, argv[1], NULL};
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
		write(out_b, frame, strlen(frame));
		//printf("Wrote %d bytes\n", (int)write(out_b, frame, strlen(frame)));
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
	
	//Release buffer and Frame
	free(buffer);
	buffer = (char *) NULL;
	free(frame);
	frame = (char *) NULL;
	
	
	
	//Write to consumer
	char * cbinfile = calloc(20, sizeof(char)); 
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {"input.binf", NULL};
		//ReRoute STDOUT to p
		dup2(p[1], STDOUT_FILENO);
		close(p[1]);
		//Call Binary String Service
		execv(c, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		read(p[0], cbinfile, 20);
		close(p[0]);
	}
	
	//printf("FILENAME: %s\n", cbinfile);
	
	//Calculate how much data is in the file
	int in_fd = open(cbinfile, O_RDONLY);
	//Size of input.binf
	int data_size = 0;
	//input buffer
	buffer = calloc(8, sizeof(char));
	//read in one character from input.binf
	size = read(in_fd, buffer, 8);
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
	
	//Prepare our data buffer
	char * data = calloc(data_size, sizeof(char));
	
	//Deframe the File
	//Call Deframe
	int p1[2];
	pipe(p1);
	pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {cbinfile, IntToStr(data_size), NULL};
		//ReRoute STDOUT to p
		dup2(p1[1], STDOUT_FILENO);
		close(p1[1]);
		//Call DeFrame Service
		execv(dfs, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		read(p1[0], data, data_size);
		close(p1[0]);
	}
	//printf("DATA FROM INPUT FILE: %s\n", data);
	
	
	//decode the data
	//Prepare our data buffer
	char * strData = calloc(data_size/8, sizeof(char));
	//Call Decode
	int p2[2];
	pipe(p2);
	pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {data, NULL};
		//ReRoute STDOUT to p
		dup2(p2[1], STDOUT_FILENO);
		close(p2[1]);
		//Call Decode Service
		execv(ds, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		read(p2[0], strData, data_size/8);
		close(p2[0]);
	}
	printf("%s\n", strData);
	
	//Release data
	free(data);
	data = (char*) NULL;
	
	
	
	//Write Decoded Data to .outf
	remove("out.done");
	int out = open("out.done", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXO);
	write(out, strData, strlen(strData));
	close(out);
	
	//Release strData
	free(strData);
	strData = (char*) NULL;
	
	//printf("Exiting!\n");
	return 0;
}


