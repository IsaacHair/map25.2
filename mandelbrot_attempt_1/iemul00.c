#include <stdio.h>
#include <stdlib.h>

/*
 * This will simply display some test multiplications to the LED displays.
 * Addr and out will display the factors and dir will display the product.
 * The upper bit is the sign, 3 after that are int part, 12 after are fraction.
 * Gen is used to to count up and delay the output so that the test
 * numbers are displayed for like 1 second before being changed to the next
 * pair.
 */

unsigned short addr;
FILE* fd;

void inst(char*op) {
	fprintf(fd, "%04x %s %04x\n", addr, op, addr+1);
	addr++;
}

void instval(char*op, unsigned short val) {
	fprintf(fd, "%04x %s %04x %04x\n", addr, op, val, addr+1);
	addr++;
}

void instnxt(char*op, unsigned short nxt) {
	fprintf(fd, "%04x %s %04x\n", addr, op, nxt);
	addr++;
}

void instvalnxt(char*op, unsigned short val, unsigned short nxt) {
	fprintf(fd, "%04x %s %04x %04x\n", addr, op, val, nxt);
	addr++;
}


