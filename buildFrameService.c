#include "encDec.h" //Our Header
#include <stdlib.h> //Calloc/Malloc Memory allocation
#include <stdio.h> //Input Output operations - printf - remove after debugging
#include <unistd.h> //Open, Close Files
#include <fcntl.h> //File Flags
#include <sys/types.h> //fork
#include <sys/wait.h> //wait
#include <string.h>


char * buildFrame(char* SYN, char* SIZE, char* BUFFER) {
	int frame_size = (strlen(SYN) + strlen(SIZE) + strlen(BUFFER)) + 1;
	char * frame = calloc(frame_size, sizeof(char));
	//printf("TOTAL SIZE OF ARGS: %d\n", (int)(strlen(SYN) + strlen(SIZE) + strlen(BUFFER)));
	int i = 0;
	int f_i = 0;
	//printf("SYN: %s\n", SYN);
	while(i < strlen(SYN)) {
		frame[f_i++] = SYN[i++];
	}
	//printf("FRAME: %s\n", frame);
	i=0;
	//printf("SIZE: %s\n", SIZE);
	while(i < strlen(SIZE)) {
		frame[f_i++] = SIZE[i++];
	}
	//printf("FRAME: %s\n", frame);
	i=0;
	//printf("BUFFER: %s\n", BUFFER);
	while(i < strlen(BUFFER)) {
		frame[f_i++] = BUFFER[i++];
	}
	//printf("FRAME: %s\n", frame);
	//printf("COMBINED DATA SIZE: %d\n", (int)(strlen(SYN) + strlen(SIZE) + strlen(BUFFER)));
	//printf("FRAME SIZE: %d\n", (int)strlen(frame));
	//printf("FRAME %s\n", frame);
	frame[f_i] = '\0';
	return frame;
}

int main(int argc, char** argv) {
	//printf("Entering bfs\n");
	
	int i = 0;
	while(i < argc) {
		//printf("Arg %d: %s\n", i, argv[i]);
		i++;
	}
	if(argc >= 3) {
		//printf("Building Frame!\n");
		printf("%s", buildFrame(argv[0], argv[1], argv[2]));
	}
	//printf("Exiting bfs\n");
	return 0;
}
