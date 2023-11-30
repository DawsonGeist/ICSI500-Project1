#include "encDec.h" //Our Header
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <string.h>

char * cps = "/home/dgeist/Project1/checkParityService";

int binToChar(char * binInput) {
	//remove parity bit
	char * bi_truncated = calloc(7, sizeof(char));
	for(int i = 0; i < 7; i++) {
		bi_truncated[i] = binInput[i+1];
	}
	int sum = 0;
	int index = 0;
	int power = 64;
	while(index < strlen(bi_truncated)) {
		if(bi_truncated[index] == '1'){
			sum += power;
		}
		power/= 2;
		index++;
	}
	return sum;
}

char * binToStr(char * binInput) {
	char * strOutput = calloc((strlen(binInput)/8) + 1, sizeof(char));
	char * err = calloc(10, sizeof(char));
	char tempOutput[(strlen(binInput)/8) + 1];
	//printf("Input: %s\n", binInput);
	char temp[8];
	int tempIndex = 0;
	int inputIndex = 0;
	int outputIndex = 0;
	
	//CAREFUL, WE WANT TO LOOP ONE LAST TIME BECAUSE WHEN WE PASS THE LAST BIT (hence <=)
	//inputIndex == strlen(binInput)
	//temp WILL BE FULL BUT WON'T BE CONVERTED (we end up missing the last character) 		
	//BECAUSE WE DON'T HIT THE ELSE STATEMENT WITHIN THE FOR LOOP (because we break out of loop)
	while (inputIndex <= strlen(binInput)) { 
		if(tempIndex < 8) {
			temp[tempIndex] = binInput[inputIndex];
			tempIndex++;
			inputIndex++;
		}
		else {
			//TODO Implement Check Parity Bit Service
			int p[2];
			pipe(p);
			int pid = fork();
			if(pid<0) {
				printf("Error in fork\n");
			}
			else if(pid == 0) {
				//Build Args
				char * const args[] = {temp, NULL};
				//ReRoute STDOUT to p
				dup2(p[1], STDOUT_FILENO);
				close(p[1]);
				//Call Check Parity Service
				execv(cps, args);
				//printf("Error in Exec\n");
			}
			else {
				wait(NULL);
				//Read result from child process
				read(p[0], err, 10);
				close(p[0]);
			}
			
			if(strcmp(err, "ERROR") == 0) {
				//strcpy(strOutput, tempOutput);
				//strcat(strOutput, "-ERROR");
				printf("(%c)", (char)binToChar(temp));
			}
			
			//Convert temp into character
			tempOutput[outputIndex] = (char)binToChar(temp);
			outputIndex++;
			tempIndex = 0;
		}	
	}
	tempOutput[outputIndex] = '\0';
	strcpy(strOutput, tempOutput);
	//printf("Output: %s\n", strOutput);
	return strOutput;
}

char * decode(char * data) {
	char * strData = binToStr(data);
	return strData;
}

int main(int argc, char ** argv) {
	printf("%s", decode(argv[0]));
	return 0;
}
