#include <stdlib.h>
#include <stdio.h>

unsigned short addr;
FILE* fd;

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("need target assembly file\n");
		exit(0x01);
	}
}	
