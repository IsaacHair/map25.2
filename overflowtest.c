#include <stdlib.h>
#include <stdio.h>

void main() {
	unsigned short mask;
	mask = 0x8000;
	printf("mask:%08x\n", mask);
	mask = mask<<1;
	printf("masknew:%08x\n", mask);
}
