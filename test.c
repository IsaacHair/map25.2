#include <stdlib.h>
#include <stdio.h>

void main() {
	unsigned short num = 0x8000;
	printf("%04x\n", (unsigned short)(num<<1));
}
