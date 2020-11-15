#include <stdio.h>
#include <stdlib.h>

/*
 * This will simply display some test multiplications to the LED displays.
 * Addr and out will display the factors and dir will display the product.
 * The upper bit is the sign, 3 after that are int part, 12 after are fraction.
 * Gen is used to to count up and delay the output so that the test
 * numbers are displayed for like 1 second before being changed to the next
 * pair.
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

	//set correct address (this function has an address range of approx 6000 because the jumps give horribly large estimates)
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
	for (i = 0xb, mask = 0x0001; i != 0xe;
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
	for (i = 0xe, mask = 0x2000; i >= 0xd;
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
	instval("imm addr1", MUL_ARRAY|0xc);
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

	addr = doneaddr;
	//add MUL_ARRAY|0xf without rotation
	//use gen as is; no gen init
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xf);
	//next code block approx location
	doneaddr = addr+0x0480;
	//see if you can skip this address
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", doneaddr);
	//do the addition
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

	addr = doneaddr;
	//add MUL_ARRAY|0xe without rotation
	//use gen as is; no gen init
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xe);
	//next code block approx location
	doneaddr = addr+0x0480;
	//see if you can skip this address
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", doneaddr);
	//do the addition
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

	addr = doneaddr;
	//add MUL_ARRAY|0xd without rotation
	//use gen as is; no gen init
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xd);
	//next code block approx location
	doneaddr = addr+0x0480;
	//see if you can skip this address
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", doneaddr);
	//do the addition
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

	//correct for sign (weird shit see notes)
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

void main(int argc, char**argv) {
	if (argc != 2) {
		printf("need target\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	addr = 0;
	unsigned short startaddr, delayaddr, printaddr;
	
	//see what colors you would give to some random points
	//init
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZI0);
	inst("imm ramall 13c0");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ZR0);
	inst("imm ramall 0690");
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
	//test if escape due to OLD square sum being >=4
	//squares are guarunteed positive
	//allowing for overflow into sign bit
	calladd(MAIN_TEMP, MAIN_ZRS, MAIN_ZIS);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_TEMP);
	makeaddrodd();
	printaddr = addr+0x0020;
	inst("ram jzor c000");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", printaddr);
	//test if counter is >= max (32)
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	makeaddrodd();
	inst("ram jzor ffe0");
	instnxt("dnc noop 0000", startaddr);
	instnxt("dnc noop 0000", printaddr);
	//print
	addr = printaddr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_COUNTER);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	//delay
	inst("imm gen1 ffff");
	delayaddr = addr;
	inst("dnc noop 0000");
	genpred16();
	inst("dnc noop 0000");
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", 0x0000);
	instnxt("dnc noop 0000", delayaddr);

	mulcode();
	addcode();

	fclose(fd);
}
