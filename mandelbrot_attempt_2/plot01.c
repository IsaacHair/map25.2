#include <stdlib.h>
#include <stdio.h>

/*
 * Mandelbrot set rendering with 32 bit precision.
 * Viewing window CANNOT be dynamically resized.
 */

unsigned short addr;
FILE* fd;

//window definitions
//these are constants, not pointers
#define VALUE_XSTART 0xf84f0000
#define VALUE_DX 0xffff0000
#define VALUE_XLIMN 0x08f10000
#define VALUE_YSTART 0xf55d0000
#define VALUE_DY 0x00010000
#define VALUE_YLIMN 0x09b30000

//pointers (all are for ram except *_LOC, which is for rom)
#define MUL_ARRAY 0xa900
#define MUL_F0 0xaa00
#define MUL_F1 0xaa02
#define MUL_PROD 0xaa04
#define MUL_RET 0xaa06
#define MUL_LOC 0xe000

#define ADD_ADDEND0 0x6900
#define ADD_ADDEND1 0x6902
#define ADD_SUM 0x6904
#define ADD_RET 0x6906
#define ADD_LOC 0xd800

#define MAIN_ZI 0x4200
#define MAIN_ZR 0x4202
#define MAIN_ZIS 0x4204
#define MAIN_ZRS 0x4206
#define MAIN_ZI0 0x4208
#define MAIN_ZR0 0x420a
#define MAIN_TEMP 0x420c
#define MAIN_COUNTER 0x420e
#define MAIN_Y 0x4210
#define MAIN_X 0x4212
#define MAIN_DY 0x4214
#define MAIN_DX 0x4216
#define MAIN_RESULT 0x4218
#define MAIN_XLIMN 0x421a
#define MAIN_YLIMN 0x421c

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
	fseek(fd, -5, SEEK_CUR);
	fprintf(fd, "%04x\n", nxt);
}

void makeaddrodd() {
	if (!(addr%2)) {
		setprevnxt((addr-1)+2);
		addr--;
		addr += 2;
	}
}

void buswrite(int val) {
	inst("imm out0 00ff");
	instval("imm out1", val%256);
	inst("imm out0 0200");
	inst("imm out1 0200");
}

void buswritegen() {
	//destroys upper part of gen
	inst("imm out0 00ff");
	inst("imm gen0 ff00");
	inst("gen out1 0000");
	inst("imm out0 0200");
	inst("imm out1 0200");
}

void comm1dat(int a, int b) {
	buswrite(a);
	inst("imm out1 0400");
	buswrite(b);
	inst("imm out0 0400");
}

void comm4dat(int a, int b, int c, int d, int e) {
	buswrite(a);
	inst("imm out1 0400");
	buswrite(b);
	buswrite(c);
	buswrite(d);
	buswrite(e);
	inst("imm out0 0400");
}

void genpred16() {
	//note: only the previous instruction in the file
	//should feed to this macro
	//note: this guy ends on an instruction with a shitload
	//of pointers to it
	unsigned short mask, addrshift;
	int i;
	
	makeaddrodd();
	for (i = 16; i >= 2; i = i>>1)
		for (mask = 0xffff>>(i/2), addrshift = 16/i;
		     mask; mask = mask>>i, addrshift++)
			instvalnxt("gen jzor", mask, addr+addrshift);
	instvalnxt("gen jzor", 0xffff, addr+16);
	for (mask = 0x3fff; mask; mask = mask>>1)
		instvalnxt("imm gen1", mask, addr+18);
	instvalnxt("imm gen0", 0x0001, addr+18);
	instvalnxt("imm gen1", 0xffff, addr+17);
	instvalnxt("imm gen1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 15; mask >= 0x0002; mask = mask>>1, i--)
		instvalnxt("imm gen0", mask, addr+i);
}

void addrpred5_1() {
	//must have a preceding instruction that is the only one feeding to it
	//must feed to instruction directly after
	//not the FASTEST protocol, but it is not the limiting factor at all
	unsigned short mask;
	unsigned short endaddr;
	makeaddrodd();
	endaddr = addr+19;
	for (mask = 0x0002; mask <= 0x0020; mask = mask<<1) {
		instvalnxt("addr jzor", mask, addr+1);
		instvalnxt("imm addr1", mask, addr+3);
		instvalnxt("imm addr0", mask, endaddr);
		addr++;
	}
	addr = endaddr;
}

void mul32code() {
	//FUNCTION, not macro, that multiplies two 32 bit numbers of the
	//1-4-27 sign-integer-fraction form
	
	//correct for sign
	
