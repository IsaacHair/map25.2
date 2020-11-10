#include <stdlib.h>
#include <stdio.h>

FILE* fd;
unsigned short addr;

void main(int argc, char **argv) {
	if (argc != 2) {
		printf("need target file\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	addr = 0;


