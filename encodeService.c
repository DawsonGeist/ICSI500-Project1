#include "encDec.h" //Our Header
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <string.h>

char * bfs = "/home/dgeist/Project1/buildFrameService";
char * ps = "/home/dgeist/Project1/parityBitService";
char * ses = "/home/dgeist/Project1/simulateErrorService";

char * charToBin(char charInput) {
	char * binOutput = calloc(8, sizeof(char)); //String Literal - Non-Modifiable
	char temp[8]; //String, but as an array of chars... modifiable
	int charValue = charInput;
	int outputIndex = 1; //0th index is reserved for parity bit
	int divisor = 64;
	while (outputIndex < 8) {
		//printf("charValue: %d\n", charValue);
		//printf("Divisor: %d\n", divisor);
		//printf("Output: %s\n", binOutput);
		if((charValue/divisor) > 0) {
			temp[outputIndex] = '1';
			charValue = charValue % divisor;
		}
		else {
			temp[outputIndex] = '0';
		}
		divisor /= 2;
		outputIndex++;
	}
	strcpy(binOutput, temp);
	//Set Parity Bit
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid<0) {
		//printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {binOutput, NULL};
		//ReRoute STDOUT to p
		dup2(p[1], STDOUT_FILENO);
		close(p[1]);
		//Call Parity Bit Service
		execv(ps, args);
		//printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		//Read result from child process
		read(p[0], temp, 8);
		strcpy(binOutput, temp);
		close(p[0]);
	}
	//printf("binOutput: %s\n", binOutput);
	return binOutput;
}

char * strToBin(char * strInput) {
	char * binOutput = calloc((strlen(strInput) * 8) + 1, sizeof(char));
	//printf("Input: %s\n", strInput);
	//printf("Binary Size: %d\n", (int)((strlen(strInput) * 8) + 1));
	int inputIndex = 0;
	int outputIndex = 0;
	while(inputIndex < strlen(strInput)) {
		char * currCharAsBin = charToBin(strInput[inputIndex]);
		int currCharAsBinIndex = 0;
		while(currCharAsBinIndex < strlen(currCharAsBin)) {
			binOutput[outputIndex++] = currCharAsBin[currCharAsBinIndex++];
		}
		inputIndex++;
	}
	binOutput[(strlen(strInput) * 8)] = '\0';
	//printf("Output: %s\n", binOutput);
	return binOutput;
}

char * encode(char * data) {
	char * binData = strToBin(data);
	return binData;
}

int main(int argc, char** argv) {
	//Encode Data
	char * syn = encode(argv[0]);
	char * size = encode(argv[1]);
	char * data = encode(argv[2]);
	
	//Simulate error
	if(strcmp(argv[3], "1") == 0) {
		//Call Simulate Error Service
		int p0[2];
		pipe(p0);
		int pid0 = fork();
		if(pid0<0) {
			printf("Error in fork\n");
		}
		else if(pid0 == 0) {
			//Build Args
			char * const args[] = {data, NULL};
			//ReRoute STDOUT to p
			dup2(p0[1], STDOUT_FILENO);
			close(p0[1]);
			//Call encode service
			execv(ses, args);
			printf("Error in Exec\n");
		}
		else {
			wait(NULL);
			char * errorData = calloc(strlen(data), sizeof(char));
			read(p0[0], errorData, strlen(data));
			close(p0[0]);
			strcpy(data, errorData);
			free(errorData);
			errorData = (char *)NULL;
		}
	}
	
	// Build Frame
	int frame_size = (strlen(syn) + strlen(size) + strlen(data));
	char * frame = calloc(frame_size, sizeof(char));
	int p[2];
	pipe(p);
	int pid = fork();
	if(pid<0) {
		printf("Error in fork\n");
	}
	else if(pid == 0) {
		//Build Args
		char * const args[] = {syn, size, data, NULL};
		//printf("data in args: %s\n", args[2]);
		//ReRoute STDOUT to p
		//dup2(p[1], STDOUT_FILENO);
		close(p[1]);
		//Call Build Frame Service
		execv(bfs, args);
		printf("Error in Exec\n");
	}
	else {
		wait(NULL);
		//Read result from child process
		//read(p[0], frame, frame_size);
		close(p[0]);
	}
	
	//Print Frame 
	printf("%s", frame);
	//printf("%s", syn);
	//printf("%s", size);
	//printf("%s", data);
	return 0;
}




