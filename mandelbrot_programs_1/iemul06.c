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
 */

unsigned short addr;
FILE* fd;

#define MUL_ARRAY 0x5500
#define MUL_F0 0xaa00
#define MUL_F1 0xaa01
#define MUL_PROD 0xaa02
#define MUL_RET 0xaa03

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
		instvalnxt("imm gen1", mask, addr+19);
	instvalnxt("imm gen0", 0x0001, addr+19);
	instvalnxt("imm gen1", 0xffff, addr+18);
	instvalnxt("imm gen1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 16; mask >= 0x0001; mask = mask>>1, i--)
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
	unsigned short addrdone, pointer, firstloopaddr;
	unsigned short doneaddr, carryaddr, noncarryaddr;
	unsigned short newnoncarryaddr, newcarryaddr;
	int i;
	unsigned short mask;

	//initialize MUL_PROD to 0x0000 (yes this is necessary)
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	inst("imm ram 0000");
	
	//make both factors positive and record the answer sign in MUL_PROD
	inst("imm addr0 ffff");
	for (pointer = MUL_F0; pointer != MUL_F1; pointer = MUL_F1) {
		instval("imm addr1", pointer);
		makeaddrodd();
		addrdone = addr+0x0040; //estimate
		inst("ram jzor 8000");
		instnxt("imm addr0 ffff", addrdone);
		inst("imm gen0 ffff");
		inst("ram gen1 0000");
		genpred16();
		inst("gen ram 0000");
		inst("imm gen1 ffff");
		inst("ram gen0 0000");
		inst("ram gen0 ffff");
		instval("ram gen1", MUL_PROD);
		makeaddrodd();
		inst("ram jzor 8000");
		instnxt("imm ram 8000", addr+2);
		instnxt("imm ram 0000", addr+1);
		inst("gen ram 0000");
		instnxt("imm addr0 ffff", addrdone);
		addr = addrdone;
	}

	//explicitly define the procedure for recording partial products
	/*inst("imm addr0 ffff");*/ //already done
	instval("imm addr1", MUL_F0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	//part that doesn't shift up
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
		instnxt("imm ram 0000", addr+2);
		instnxt("gen ram 0000", addr+1);
	}
	//part that does shift up (shifts before buffering value)
	//also destroys gen
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
		instnxt("rol ram 0000", addr+2);
		instnxt("rol ram 0000", addr+2);
		instnxt("imm gen0 ffff", addr+2);
		instnxt("imm gen0 ffff", addr+2);
		instnxt("ram gen1 0000", addr+2);
		instnxt("ram gen1 0000", addr+2);
		instnxt("imm ram 0000", addr+1);
	}

	//add the partial products with rotation
	//init
	inst("imm gen0 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xc);
	//loop that actually does the addition
	firstloopaddr = addr;
	//next block address (for the very end of this one)
	doneaddr = addr+0x0400;
	//check addr what the addr WAS
	makeaddrodd();
	inst("addr jzor 000f");
	instnxt("dnc noop 0000", doneaddr);
	inst("dnc noop 0000");
	//address predecessor
	addrpred4();
	//do the rotation addition
	inst("dnc noop 0000");
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0011;
	carryaddr = addr+0x0021;
	instnxt("ram jzor 0001", addr+1);
	instnxt("gen jzor 0001", addr+2);
	instnxt("gen jzor 0001", addr+3);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", noncarryaddr);
	instnxt("ram jzor 0002", carryaddr);
	for (mask = 0x0002; mask; mask = mask<<1) {
		//the following estimates need to be even
		newnoncarryaddr = (addr&0xfffe)+0x0020;
		newcarryaddr = (addr&0xfffe)+0x0030;
		addr = noncarryaddr;
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
	//add MUL_ARRAY|0xe without rotation
	//use gen as is; no gen init
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0xe);
	//next code block approx location
	doneaddr = addr+0x0400;
	//do the addition
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0011;
	carryaddr = addr+0x0021;
	instnxt("ram jzor 0001", noncarryaddr);
	for (mask = 0x0001; mask; mask = mask<<1) {
		//the following estimates need to be even
		newnoncarryaddr = (addr&0xfffe)+0x0020;
		newcarryaddr = (addr&0xfffe)+0x0030;
		addr = noncarryaddr;
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
	doneaddr = addr+0x0400;
	//do the addition
	makeaddrodd();
	//the following estimates need to be even
	noncarryaddr = addr+0x0011;
	carryaddr = addr+0x0021;
	instnxt("ram jzor 0001", noncarryaddr);
	for (mask = 0x0001; mask; mask = mask<<1) {
		//the following estimates need to be even
		newnoncarryaddr = (addr&0xfffe)+0x0020;
		newcarryaddr = (addr&0xfffe)+0x0030;
		addr = noncarryaddr;
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
	doneaddr = addr+0x0100;
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("gen ram 0000", doneaddr);
	genpred16();
	inst("gen ram 0000");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	instnxt("gen ram 0000", doneaddr);
	addr = doneaddr;

	//go to return address
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_RET);
	inst("ram asnx 0000");
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

	mulcode();

	fclose(fd);
}
