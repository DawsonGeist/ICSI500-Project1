#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encDec.h"

char * AddParityBit(char * binInput) {
	char * modifiedInput = calloc(strlen(binInput), sizeof(char));
	char * temp = calloc(8, sizeof(char));
	int index = 1;
	int sum = 0;
	while(index < strlen(binInput)) {
		if(binInput[index] == '1') {
			sum++;
		}
		temp[index] = binInput[index];
		index++;
	}
	temp[0] = sum%2 == 1 ? '1' : '0';
	strcpy(modifiedInput, temp);
	
	return modifiedInput;
}

char * AddParityBit2(char * binInput) {
	int i = 1; 
	int max = strlen(binInput);
	while(i < max){
		if(binInput[i] == '1') {
			binInput[0] = '1';
		}
		else {
			binInput[0] = '0';
		}
		i++;
	}
	
	return binInput;
}

int main(int argc, char** argv) {
	printf("%s", AddParityBit(argv[0]));
}
