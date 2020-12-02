#include <stdio.h>
#include <stdlib.h>

/*
 * Creates a rendering of the mandelbrot set.
 * Integers are simply stored as 16 bit integers.
 * "Floating points" are stored as follows:
 * The upper bit is the sign, 4 after that are int part, 11 after are fraction.
 * Note: the "point" doesnt actually "float"
 * Note: this program expects \n to constitute only ONE character;
 * this is relevant when re-writing nxt addresses.
 * Note: FUNCTIONS DO NOT SUPPORT RECURSION.
 */

unsigned short addr;
FILE* fd;

#define MUL_ARRAY 0xa900
#define MUL_F0 0xaa00
#define MUL_F1 0xaa01
#define MUL_PROD 0xaa02
#define MUL_RET 0xaa03
#define MUL_LOC 0xe000

#define ADD_ADDEND0 0x6900
#define ADD_ADDEND1 0x6901
#define ADD_SUM 0x6902
#define ADD_RET 0x6903
#define ADD_LOC 0xd800

#define MAIN_ZI 0x4200
#define MAIN_ZR 0x4201
#define MAIN_ZIS 0x4202
#define MAIN_ZRS 0x4203
#define MAIN_ZI0 0x4204
#define MAIN_ZR0 0x4205
#define MAIN_TEMP 0x4206
#define MAIN_COUNTER 0x4207
#define MAIN_Y 0x4208
#define MAIN_X 0x4209
#define MAIN_DY 0x420a
#define MAIN_DX 0x420b
#define MAIN_RESULT 0x420c
#define MAIN_XLIMN 0x420d
#define MAIN_YLIMN 0x420e

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
	int i;
	inst("imm out0 00ff");
	instval("imm out1", val%256);
	inst("imm out0 0200");
	inst("imm out1 0200");
}

void buswritegen() {
	//destroys upper part of gen
	int i;
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

void to(int mark) {
	//dont use if it is the very beginning of the program
	fseek(fd, -5, SEEK_CUR);
	fprintf(fd, "%04x\n", mark);
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
		instvalnxt("imm gen1", mask, addr+18);
	instvalnxt("imm gen0", 0x0001, addr+18);
	instvalnxt("imm gen1", 0xffff, addr+17);
	instvalnxt("imm gen1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 15; mask >= 0x0002; mask = mask>>1, i--)
		instvalnxt("imm gen0", mask, addr+i);
}

void addrpred4() {
	//must have a preceding instruction that is the only one feeding to it
	//must feed to instruction directly after
	makeaddrodd();
	instnxt("addr jzor 0003", addr+1);
	instnxt("addr jzor 0007", addr+2);
	instnxt("addr jzor 0001", addr+3);
	instnxt("addr jzor 000f", addr+4);
	instnxt("imm addr1 0003", addr+6);
	instnxt("imm addr1 0001", addr+6);
	instnxt("imm addr0 0001", addr+6);
	instnxt("imm addr1 000f", addr+5);
	instnxt("imm addr1 0007", addr+1);
	instnxt("imm addr0 0008", addr+3);
	instnxt("imm addr0 0004", addr+2);
	instnxt("imm addr0 0002", addr+1);
}

void mulcode() {
	//destroys MUL_F0 and MUL_F1
	//preserves progaddr
	unsigned short addrdone, pointer, firstloopaddr;
	unsigned short doneaddr, carryaddr, noncarryaddr;
	unsigned short newnoncarryaddr, newcarryaddr;
	unsigned short fulladdaddr;
	unsigned short originaladdr;
	int i;
	unsigned short mask;

	originaladdr = addr;
	addr = MUL_LOC;

	//initialize MUL_PROD to 0x0000 (yes this is necessary)
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	inst("imm ramall 0000");
	
	//make both factors positive and record the answer sign in MUL_PROD
	for (pointer = MUL_F0, i = 0; i < 2; i++, pointer = MUL_F1) {
		inst("imm addr0 ffff");
		instval("imm addr1", pointer);
		makeaddrodd();
		addrdone = addr+0x0080; //estimate
		inst("ram jzor 8000");
		instnxt("imm addr0 ffff", addrdone);
		inst("imm gen0 ffff");
		inst("ram gen1 0000");
		genpred16();
		inst("gen ramall 0000");
		inst("imm gen1 ffff");
		inst("ram gen0 0000");
		inst("gen ramall 0000");
		inst("imm addr0 ffff");
		instval("imm addr1", MUL_PROD);
		makeaddrodd();
		inst("ram jzor 8000");
		instnxt("imm ramall 8000", addrdone);
		instnxt("imm ramall 0000", addrdone);
		addr = addrdone;
	}

	//explicitly define the procedure for recording partial products
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_F0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	//part that doesn't shift up
	//uses all 16 bits
	for (i = 0xa, mask = 0x0001; i != 0xe;
	     ((i==0) ? i = 0xf : i--), mask = mask<<1) {
		inst("imm addr0 ffff");
		instval("imm addr1", MUL_F1);
		makeaddrodd();
		instval("ram jzor", mask);
		instnxt("imm addr0 ffff", addr+2);
		instnxt("imm addr0 ffff", addr+2);
		instvalnxt("imm addr1", MUL_ARRAY|i, addr+2);
		instvalnxt("imm addr1", MUL_ARRAY|i, addr+2);
		instnxt("imm ramall 0000", addr+2);
		instnxt("gen ramall 0000", addr+1);
	}
	//part that does shift up (shifts before buffering value)
	//also destroys gen
	//uses all 16 bits
	for (i = 0xe, mask = 0x1000; i >= 0xc;
	     i--, mask = mask<<1) {
		inst("imm gen0 8000");
		inst("imm addr0 ffff");
		instval("imm addr1", MUL_F1);
		makeaddrodd();
		instval("ram jzor", mask);
		instnxt("imm addr0 ffff", addr+2);
		instnxt("imm addr0 ffff", addr+2);
		instvalnxt("imm addr1", MUL_ARRAY|i, addr+2);
		instvalnxt("imm addr1", MUL_ARRAY|i, addr+2);
		instnxt("rol ramall 0000", addr+2);
		instnxt("rol ramall 0000", addr+2);
		instnxt("imm gen0 ffff", addr+2);
		instnxt("imm gen0 ffff", addr+2);
		instnxt("ram gen1 0000", addr+2);
		instnxt("ram gen1 0000", addr+2);
		instnxt("imm ramall 0000", addr+1);
	}
	
	//add the partial products with rotation
	//init
	inst("imm gen0 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xb);
	//next block address (for the very end of this one)
	doneaddr = addr+0x0480;
	//loop that actually does the addition
	//check addr what the addr WAS
	makeaddrodd();
	firstloopaddr = addr;
	inst("addr jzor 000f");
	instnxt("dnc noop 0000", doneaddr);
	inst("dnc noop 0000");
	//address predecessor
	addrpred4();
	inst("dnc noop 0000");
	//see if you can skip this address
	//still have to rotate, even if skipping
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	//do the rotation addition
	fulladdaddr = addr+0x0008;
	instnxt("dnc noop 0000", fulladdaddr);
	inst("ror ramall 0000");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	//can't have the lsb rotating to be the msb
	instnxt("imm gen0 8000", firstloopaddr);
	addr = fulladdaddr;
	inst("dnc noop 0000");
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0021;
	carryaddr = addr+0x0041;
	instnxt("ram jzor 0001", addr+1);
	instnxt("gen jzor 0001", addr+2);
	instnxt("gen jzor 0001", addr+3);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", carryaddr);
	for (mask = 0x0002; mask; mask = mask<<1) {
		//the following estimates need to be even
		addr = noncarryaddr;
		newnoncarryaddr = (addr/2*2)+0x0040;
		newcarryaddr = (addr/2*2)+0x0060;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		instvalnxt("imm gen0", mask>>1, addr+4);
		instvalnxt("imm gen1", mask>>1, addr+4);
		instvalnxt("imm gen1", mask>>1, addr+4);
		instvalnxt("imm gen0", mask>>1, addr+4);
		if (mask != 0x8000) {
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen0", 0x8000, firstloopaddr);
			instvalnxt("imm gen0", 0x8000, firstloopaddr);
			instvalnxt("imm gen0", 0x8000, firstloopaddr);
			instvalnxt("imm gen1", 0x8000, firstloopaddr);
		}
		addr = carryaddr;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		instvalnxt("imm gen1", mask>>1, addr+4);
		instvalnxt("imm gen0", mask>>1, addr+4);
		instvalnxt("imm gen0", mask>>1, addr+4);
		instvalnxt("imm gen1", mask>>1, addr+4);
		if (mask != 0x8000) {
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen0", 0x8000, firstloopaddr);
			instvalnxt("imm gen1", 0x8000, firstloopaddr);
			instvalnxt("imm gen1", 0x8000, firstloopaddr);
			instvalnxt("imm gen1", 0x8000, firstloopaddr);
		}
		carryaddr = newcarryaddr;
		noncarryaddr = newnoncarryaddr;
	}

	//add partial products without rotation
	addr = doneaddr;
	//init
	//use gen as is; no gen init
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0x0);
	//next block address (for the very end of this one)
	doneaddr = addr+0x0480;
	//loop that actually does the addition
	//check addr what the addr WAS
	makeaddrodd();
	firstloopaddr = addr;
	inst("addr jzor 0003");
	instnxt("dnc noop 0000", addr+3);
	instnxt("dnc noop 0000", addr+5);
	addr++;
	inst("addr jzor 0004");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", doneaddr);
	inst("dnc noop 0000");
	//address predecessor
	addrpred4();
	inst("dnc noop 0000");
	//see if you can skip this address
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", firstloopaddr);
	//do the non-rotation addition
	inst("dnc noop 0000");
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0021;
	carryaddr = addr+0x0041;
	instnxt("ram jzor 0001", noncarryaddr);
	for (mask = 0x0001; mask; mask = mask<<1) {
		//the following estimates need to be even
		addr = noncarryaddr;
		newnoncarryaddr = (addr/2*2)+0x0040;
		newcarryaddr = (addr/2*2)+0x0060;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen0", mask, firstloopaddr);
			instvalnxt("imm gen1", mask, firstloopaddr);
			instvalnxt("imm gen1", mask, firstloopaddr);
			instvalnxt("imm gen0", mask, firstloopaddr);
		}
		addr = carryaddr;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen1", mask, firstloopaddr);
			instvalnxt("imm gen0", mask, firstloopaddr);
			instvalnxt("imm gen0", mask, firstloopaddr);
			instvalnxt("imm gen1", mask, firstloopaddr);
		}
		carryaddr = newcarryaddr;
		noncarryaddr = newnoncarryaddr;
	}

	//correct for sign and overflow with %32
	addr = doneaddr;
	inst("gen dir1 0000");
	doneaddr = addr+0x0100;
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("gen ramall 0000", doneaddr);
	inst("dnc noop 0000");
	genpred16();
	inst("gen ramall 0000");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	instnxt("gen ramall 0000", doneaddr);
	addr = doneaddr;

	//go to return address
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_RET);
	inst("ram asnx 0000");

	addr = originaladdr;
}

void callmul(unsigned int point_prod, unsigned int point_f0, unsigned int point_f1) {
	//transfer parameters and go to function
	inst("imm addr0 ffff");
	instval("imm addr1", point_f0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_F0);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_f1);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_F1);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_RET);
	instvalnxt("imm ramall", addr+1, MUL_LOC);
	//transfer answer
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_prod);
	inst("gen ramall 0000");
}

void addcode() {
	//preserves progaddr
	//destroys gen and addr
	unsigned short mask;
	unsigned short carryaddr, noncarryaddr;
	unsigned short newcarryaddr, newnoncarryaddr;
	unsigned short doneaddr;
	unsigned short originaladdr;

	originaladdr = addr;
	addr = ADD_LOC;

	//charge gen with value at ADD_ADDEND0 and set addr to ADD_ADDEND1
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND1);
	//next code block approx location
	doneaddr = addr+0x0480;
	//do the addition
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0021;
	carryaddr = addr+0x0041;
	instnxt("ram jzor 0001", noncarryaddr);
	for (mask = 0x0001; mask; mask = mask<<1) {
		//the following estimates need to be even
		addr = noncarryaddr;
		newnoncarryaddr = (addr/2*2)+0x0040;
		newcarryaddr = (addr/2*2)+0x0060;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen0", mask, doneaddr);
			instvalnxt("imm gen1", mask, doneaddr);
			instvalnxt("imm gen1", mask, doneaddr);
			instvalnxt("imm gen0", mask, doneaddr);
		}
		addr = carryaddr;
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("ram jzor", mask<<1, newnoncarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
			instvalnxt("ram jzor", mask<<1, newcarryaddr);
		}
		else {
			instvalnxt("imm gen1", mask, doneaddr);
			instvalnxt("imm gen0", mask, doneaddr);
			instvalnxt("imm gen0", mask, doneaddr);
			instvalnxt("imm gen1", mask, doneaddr);
		}
		carryaddr = newcarryaddr;
		noncarryaddr = newnoncarryaddr;
	}

	//transfer sum
	addr = doneaddr;
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_SUM);
	inst("gen ramall 0000");
	//return
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_RET);
	inst("ram asnx 0000");

	addr = originaladdr;
}	

void calladd(unsigned short point_sum, unsigned short point_a0, unsigned short point_a1) {
	inst("imm addr0 ffff");
	instval("imm addr1", point_a0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND0);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_a1);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND1);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_RET);
	instvalnxt("imm ramall", addr+1, ADD_LOC);
	//transfer sum
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_SUM);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_sum);
	inst("gen ramall 0000");
}	

void render(unsigned short point_count, unsigned short point_zi0, unsigned short point_zr0) {
	unsigned short startaddr, printaddr;
	//init
	inst("imm addr0 ffff");
	instval("imm addr1", point_zi0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZI0);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_zr0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZR0);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZI);
	inst("imm ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZR);
	inst("imm ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	inst("imm ramall 0000");
	startaddr = addr;
	//calculate squares
	callmul(MAIN_ZIS, MAIN_ZI, MAIN_ZI);
	callmul(MAIN_ZRS, MAIN_ZR, MAIN_ZR);
	//calculate the iterated imaginary
	callmul(MAIN_ZI, MAIN_ZI, MAIN_ZR);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZI);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm gen0 8000");
	inst("rol ramall 0000");
	calladd(MAIN_ZI, MAIN_ZI, MAIN_ZI0);
	//calculate iterated real
	//get the twos complement of previous imaginary square
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZIS);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	genpred16();
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_TEMP);
	inst("gen ramall 0000");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	inst("gen ramall 0000");
	//calculate iterated real for real now (hahaha get it)
	calladd(MAIN_ZR, MAIN_ZRS, MAIN_TEMP);
	calladd(MAIN_ZR, MAIN_ZR, MAIN_ZR0);
	//increment counter
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	genpred16();
	inst("gen ramall 0000");
	inst("imm gen1 ffff");
	inst("ram gen0 ffff");
	inst("gen ramall 0000");
	//test if escape due to OLD square sum being >= 4
	//squares are guarunteed positive
	//allowing for overflow into sign bit
	//dividing squares by 2 to prevent overflow
	//and testing if >= 2
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZRS);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm gen0 0001");
	inst("ror ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZIS);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm gen0 0001");
	inst("ror ramall 0000");
	calladd(MAIN_TEMP, MAIN_ZRS, MAIN_ZIS);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_TEMP);
	makeaddrodd();
	printaddr = addr+0x0020;
	inst("ram jzor f000");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", printaddr);
	//test if counter is >= max (32)
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	makeaddrodd();
	inst("ram jzor ffe0");
	instnxt("dnc noop 0000", startaddr);
	instnxt("dnc noop 0000", printaddr);
	addr = printaddr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_count);
	inst("gen ramall 0000");
}

void main(int argc, char**argv) {
	if (argc != 2) {
		printf("need target\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	addr = 0;
	unsigned short delayaddr, loopaddr;

	//lcd init
	inst("imm dir1 ffff");
	inst("imm out1 ffff");
	inst("imm out0 1000");
	//delay for reset
	inst("imm gen1 ffff");
	delayaddr = addr;
	inst("dnc noop 0000");
	genpred16();
	inst("dnc noop 0000");
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	inst("imm out1 1000");
	//more delay for reset
	inst("imm gen1 ffff");
	delayaddr = addr;
	inst("dnc noop 0000");
	genpred16();
	inst("dnc noop 0000");
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//various commands
	inst("imm out0 0c00"); //cs and rs low
	buswrite(0x38); //out of idle
	buswrite(0x11); //out of sleep
	buswrite(0x13); //normal display mode
	buswrite(0x20); //inversion is off
	buswrite(0x29); //display is on
	comm1dat(0x0c, 0xe6); //set COLMOD
	comm4dat(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4dat(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
	buswrite(0x2c); //begin frame write
	inst("imm out1 0400"); //RS high
	
	//init values
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_X);
	inst("imm ramall 0800");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_Y);
	inst("imm ramall f4c0");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_DX);
	inst("imm ramall ffe8");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_DY);
	inst("imm ramall 0018");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_XLIMN);
	inst("imm ramall 1600");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_YLIMN);
	inst("imm ramall f4c0");
	//print the rendering for the pixel you are at
	loopaddr = addr;
	//get rendering and load it into gen
	render(MAIN_RESULT, MAIN_Y, MAIN_X);
	//shift the rendering by *8
	//1 becomes almost black, 31 is light blue, 32 rolls over to black
	//use addr as register helper
	//don't need to account for last bit wrap around
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_RESULT);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	buswritegen(); //blue
	//shift the rendering by *32
	//if >= 4, make medium green
	//if = 32, make it black
	//use addr as register helper
	//don't need to account for last bit wrap around
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	makeaddrodd();
	inst("gen jzor 0780");
	instnxt("dnc noop 0000", addr+3);
	inst("imm gen1 007f");
	inst("imm gen0 0080");
	inst("gen jzor 0400");
	instnxt("dnc noop 0000", addr+2);
	inst("imm gen0 00ff");
	inst("dnc noop 0000");
	buswritegen(); //green
	inst("imm gen0 ffff");
	buswritegen(); //red
	//increment column
	calladd(MAIN_Y, MAIN_Y, MAIN_DY);
	//test column value
	calladd(MAIN_TEMP, MAIN_Y, MAIN_YLIMN);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_TEMP);
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", loopaddr);
	//increment row and reset column
	calladd(MAIN_X, MAIN_X, MAIN_DX);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_Y);
	inst("imm ramall f4c0");
	//test row value
	calladd(MAIN_TEMP, MAIN_X, MAIN_XLIMN);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_TEMP);
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", loopaddr);
	
	//end lcd write
	inst("imm out0 0400"); //RS low
	buswrite(0x00); //nop to end the write
	instnxt("dnc noop 0000", addr); //halt

	//code for functions
	mulcode();
	addcode();

	fclose(fd);
}
