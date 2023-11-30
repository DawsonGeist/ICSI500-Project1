// Encode and Helper Functions
char * encode(char * data);

char * charToBin(char charInput);

char * strToBin(char * strInput);

// Decode and Helper Functions
char * decode(char * data);

int binToChar(char * binInput);

char * binToStr(char * binInput);

// Parity Bit
char * AddParityBit(char * binInput);

char * checkParity(char * str);

//Frames
char * buildFrame(char* SYN, char* SIZE, char* BUFFER);

char * deframe(char * in, int data_size);
