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

void setprevnxt(unsigned short nxt) {
	do
		fseek(fd, -2, SEEK_CUR);
	while (fgetc(fd) != ' ');
	fprintf(fd, "%04x\n", nxt);
}

void makeaddrodd() {
	if (!(addr%2)) {
		setprevnxt((addr-1)+2);
		addr--;
		addr += 2;
	}
}

void genpred16() {
	//note: only the previous instruction in the file
	//should feed to this macro
	//note: this guy ends on an instruction with a shitload
	//of pointers to it
	//note: needs previous instruction
	unsigned short mask, addrshift;
	int i;
	
	makeaddrodd();
	for (i = 16; i >= 2; i = i>>1)
		for (mask = 0xffff>>(i/2), addrshift = 16/i;
		     mask; mask = mask>>i, addrshift++)
			instvalnxt("gen jzor", mask, addr+addrshift);
	instvalnxt("gen jzor", 0xffff, addr+16);
	for (mask = 0x3fff; mask; mask = mask>>1)
		instvalnxt("imm gen1", mask, addr+19);
	instvalnxt("imm gen0", 0x0001, addr+19);
	instvalnxt("imm gen1", 0xffff, addr+18);
	instvalnxt("imm gen1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 16; mask >= 0x0001; mask = mask>>1, i--)
		instvalnxt("imm gen0", mask, addr+i);
}

void main(int argc, char**argv) {
	if (argc != 2) {
		printf("need target\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	addr = 0;
	
	inst("dnc noop 0000");
	genpred16();
	instnxt("dnc noop 0000", 0x0000);

	fclose(fd);
}
