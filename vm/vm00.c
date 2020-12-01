#include <stdlib.h>
#include <stdio.h>

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("need <program file>\n");
		exit(0x01);
	}
	
