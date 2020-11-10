#include <stdlib.h>
#include <stdio.h>

void main(int argc, char **argv) {
	if (argc != 3) {
		printf("need inputs: <running product> <factor>\n");
		exit(0x01);
	}
	int i;
	unsigned long factor;
	unsigned long prod;
	prod = atoi(argv[1]);
	factor = atoi(argv[2]);
	for (i = 0; prod != 0x0000 && i < 0x100000; i++) {
		printf("prod:%04x\n", prod);
		prod = ((prod*factor)/0x1000)%0x10000;
	}
	printf("iterations:%d\n", i);
}
