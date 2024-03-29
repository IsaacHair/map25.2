#include <stdlib.h>
#include <stdio.h>

/*
 * Creates a version of the classic lunar lander game.
 * Note: There is NO STACK in this version, so functions
 * do not support recursion.
 */

unsigned short addr;
FILE* fd;

#define _KEY_R1 0x0008 //input (with 1k pull up)
#define _KEY_R2 0x0004 //input (with 1k pull up)
#define _KEY_R3 0x0002 //input (with 1k pull up)
#define _KEY_R4 0x0001 //input (with 1k pull up)
#define _KEY_C1 0x8000 //output (thru a 100ohm resistor)
#define _KEY_C2 0x4000 //output (thru a 100ohm resistor)
#define _KEY_C3 0x2000 //output (thru a 100ohm resistor)

#define _LCD_RST 0x1000 //output (thru a 100ohm resistor)
#define _LCD_CS 0x0800 //output (thru a 100ohm resistor)
#define _LCD_RS 0x0400 //output (thru a 100ohm resistor)
#define _LCD_WR 0x0200 //output (thru a 100ohm resistor)
#define _LCD_RD 0x0100 //output (thru a 100ohm resistor)

/*
 * Data bus:
 * _LCD_Dn corresponds to output 0xn (thru a 100ohm resistor)
 * where "n" is the number data bit.
 * Note that output 0xn refers to the position of the bit
 * on the output bus using little endian enumeration.
 * This is not defined because it is easier to just remember
 * that the lower half of the output bus is LCD data.
 * The use of 100ohm resistors is just to protect the map25.2
 */

#define ADD_ADDEND0 0x0000
#define ADD_ADDEND1 0x0001
#define ADD_SUM 0x0002
#define ADD_RET 0x0003
unsigned short add_loc;

#define MUL_ARRAY 0x0010
#define MUL_F0 0x0004
#define MUL_F1 0x0005
#define MUL_PROD 0x0006
#define MUL_RET 0x0007
unsigned short mul_loc;

void inst(char*op) {
	fprintf(fd, "%04x %s \x88%04x\n", addr, op, addr+1);
	addr++;
}

void instval(char*op, unsigned short val) {
	fprintf(fd, "%04x %s %04x \x88%04x\n", addr, op, val, addr+1);
	addr++;
}

void instnxt(char*op, unsigned short nxt) {
	fprintf(fd, "%04x %s \x88%04x\n", addr, op, nxt);
	addr++;
}

void instvalnxt(char*op, unsigned short val, unsigned short nxt) {
	fprintf(fd, "%04x %s %04x \x88%04x\n", addr, op, val, nxt);
	addr++;
}

void instexpnxt(char*op, char*nxt) {
	fprintf(fd, "%04x %s \x88%s\n", addr, op, nxt);
	addr++;
}

void instvalexpnxt(char*op, unsigned short val, char*nxt) {
	fprintf(fd, "%04x %s %04x \x88%s\n", addr, op, val, nxt);
	addr++;
}

void replacex88(char* old, char* new) {
	//leaves \x88 identifier
	int c;
	char str[4];
	int copy;
	fseek(fd, 0, SEEK_SET);
	for (c = fgetc(fd), copy = -1; c != EOF; c = fgetc(fd)) {
		if (c == 0x88) {
			copy = 0;
			continue;
		}
		if (copy > -1) {
			str[copy] = c;
			copy++;
		}
		if (copy == 4) {
			copy = -1;
			if (str[0] == old[0] && str[1] == old[1]
			    && str[2] == old[2] && str[3] == old[3]) {
				fseek(fd, -4, SEEK_CUR);
				fwrite(new, sizeof(char), 4, fd);
			}
		}
	}
}

void makeaddrodd() {
	//works at 0x0000, but the lack of instruction at 0x0000 means
	//the assembly code won't even run (as map25.2 resets to 0x0000)
	char old[5];
	char new[5];
	if (addr%2 == 0) {
		sprintf(old, "%04x", addr);
		sprintf(new, "%04x", addr+1);
		replacex88(old, new);
		addr++;
	}
}

void makeaddreven() {
	char old[5];
	char new[5];
	if (addr%2 == 1) {
		sprintf(old, "%04x", addr);
		sprintf(new, "%04x", addr+1);
		replacex88(old, new);
		addr++;
	}
}

void removex88(char* filepath) {
	FILE* newfd = fopen(filepath, "w");
	int c;
	fseek(fd, 0, SEEK_SET);
	for (c = fgetc(fd); c != EOF; c = fgetc(fd))
		if (c != 0x88)
			fprintf(newfd, "%c", c);
}

void buswrite(int val) {
	inst("imm out0 00ff");
	instval("imm out1", val%256);
	instval("imm out0", _LCD_WR);
	instval("imm out1", _LCD_WR);
}

void buswritegen() {
	//destroys upper part of gen
	inst("imm out0 00ff");
	inst("imm gen0 ff00");
	inst("gen out1 0000");
	instval("imm out0", _LCD_WR);
	instval("imm out1", _LCD_WR);
}

void buswirteout() {
	instval("imm out0", _LCD_WR);
	instval("imm out1", _LCD_WR);
}

void comm1dat(int a, int b) {
	instval("imm out0", _LCD_RS);
	buswrite(a);
	instval("imm out1", _LCD_RS);
	buswrite(b);
}

void comm4dat(int a, int b, int c, int d, int e) {
	instval("imm out0", _LCD_RS);
	buswrite(a);
	instval("imm out1", _LCD_RS);
	buswrite(b);
	buswrite(c);
	buswrite(d);
	buswrite(e);
}

void addrpred16() {
	unsigned short mask, addrshift;
	int i;
	
	makeaddrodd();
	for (i = 16; i >= 2; i = i>>1)
		for (mask = 0xffff>>(i/2), addrshift = 16/i;
		     mask; mask = mask>>i, addrshift++)
			instvalnxt("addr jzor", mask, addr+addrshift);
	instvalnxt("addr jzor", 0xffff, addr+16);
	for (mask = 0x3fff; mask; mask = mask>>1)
		instvalnxt("imm addr1", mask, addr+18);
	instvalnxt("imm addr0", 0x0001, addr+18);
	instvalnxt("imm addr1", 0xffff, addr+17);
	instvalnxt("imm addr1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 15; mask >= 0x0002; mask = mask>>1, i--)
		instvalnxt("imm addr0", mask, addr+i);
}

void makeaddr_addrpred16() {
	makeaddrodd();
}

void genpred16() {
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

void makeaddr_genpred16() {
	makeaddrodd();
}

void addgenram() {
	//adds values in ram and gen and puts answer in gen
	//requires average of 40.5 clocks
	//high efficiency rom packing (not perfect though)
	unsigned short mask;
	char str[5];

	//do the addition
	makeaddrodd();
	instexpnxt("ram jzor 0001", "N_NC");
	for (mask = 0x0001; mask; mask = mask<<1) {
		replacex88("N_NC", "__NC");
		replacex88("N__C", "___C");
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("__NC", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalnxt("imm gen1", mask, addr+2);
			instvalnxt("imm gen0", mask, addr+2);
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
		}
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("___C", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
		}
	}
	sprintf(str, "%04x", addr);
	replacex88("DONE", str);
}

void makeaddr_addgenram() {
	makeaddrodd();
}

void addgenramdwn() {
	//adds values in ram and gen and puts answer in gen
	//shifts answer down
	//requires exactly 48 clocks
	//high efficiency rom packing (not perfect though)
	unsigned short mask;
	char str[5];

	//do the addition
	makeaddrodd();
	instnxt("ram jzor 0001", addr+1);
	instnxt("gen jzor 0001", addr+2);
	instnxt("gen jzor 0001", addr+3);
	instexpnxt("ram jzor 0002", "N_NC");
	instexpnxt("ram jzor 0002", "N_NC");
	instexpnxt("ram jzor 0002", "N_NC");
	instexpnxt("ram jzor 0002", "N__C");
	for (mask = 0x0002; mask; mask = mask<<1) {
		replacex88("N_NC", "__NC");
		replacex88("N__C", "___C");
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("__NC", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
		}
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("___C", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
		}
	}
	sprintf(str, "%04x", addr);
	replacex88("DONE", str);
}

void makeaddr_addgenramdwn() {
	makeaddrodd();
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

void makeaddr_addrpred4() {
	makeaddrodd();
}

void mulcode() {
	//destroys *MUL_F0 and *MUL_F1
	//note that this multiplies two fixed points of the form:
	//(1)sign,(3)int,(12)fraction
	unsigned short firstloopaddr;
	unsigned short pointer;
	int i;
	unsigned short mask;
	char str[5], str1[5];

	//record location
	mul_loc = addr;

	//initialize MUL_PROD sign
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	inst("imm ramall 0000");
	
	//make both factors positive and record the answer sign in MUL_PROD
	for (pointer = MUL_F0, i = 0; i < 2; i++, pointer = MUL_F1) {
		inst("imm addr0 ffff");
		instval("imm addr1", pointer);
		makeaddrodd();
		inst("ram jzor 8000");
		instexpnxt("imm addr0 ffff", "FINE");
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
		instexpnxt("imm ramall 8000", "FINE");
		instexpnxt("imm ramall 0000", "FINE");
		sprintf(str, "%04x", addr);
		replacex88("FINE", str);
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
	//loop that actually does the addition
	//check addr what the addr WAS
	makeaddrodd();
	firstloopaddr = addr;
	inst("addr jzor 000f");
	instexpnxt("dnc noop 0000", "BLOC");
	inst("dnc noop 0000");
	//address predecessor
	addrpred4();
	//see if you can skip this address
	//still have to rotate, even if skipping
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	//do the rotation addition
	instexpnxt("dnc noop 0000", "FULL");
	//skip
	inst("ror ramall 0000");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	//can't have the lsb rotating to be the msb
	instnxt("imm gen0 8000", firstloopaddr);
	//actual addition
	makeaddr_addgenramdwn();
	sprintf(str, "%04x", addr);
	replacex88("FULL", str);
	addgenramdwn();
	sprintf(str1, "%04x", addr);
	sprintf(str, "%04x", firstloopaddr);
	replacex88(str1, str);
	replacex88("BLOC", str1);

	//add the partial products without rotation
	//init (use gen as is)
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_ARRAY|0x0);
	//loop that actually does the addition
	//check addr what the addr WAS (eg did it end in 0b01)
	makeaddrodd();
	firstloopaddr = addr;
	inst("addr jzor 0002");
	instnxt("addr jzor 0001", addr+2);
	instnxt("dnc noop 0000", addr+3);
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", "BLOC");
	//address predecessor
	inst("dnc noop 0000");
	addrpred4();
	//see if you can skip this address
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 0000", firstloopaddr);
	//do the rotation addition
	instexpnxt("dnc noop 0000", "FULL");
	//actual addition
	makeaddr_addgenram();
	sprintf(str, "%04x", addr);
	replacex88("FULL", str);
	addgenram();
	sprintf(str1, "%04x", addr);
	sprintf(str, "%04x", firstloopaddr);
	replacex88(str1, str);
	replacex88("BLOC", str1);

	//correct for sign
	inst("dnc noop 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("gen ramall 0000", "FINE");
	inst("dnc noop 0000");
	genpred16();
	inst("gen ramall 0000");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	instexpnxt("gen ramall 0000", "FINE");
	sprintf(str, "%04x", addr);
	replacex88("FINE", str);

	//go to return address
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_RET);
	inst("ram asnx 0000");
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
	instvalexpnxt("imm ramall", addr+1, "MULL");
	//transfer answer
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_PROD);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_prod);
	inst("gen ramall 0000");
}

void replacemulcall() {
	char str[5];
	sprintf(str, "%04x", mul_loc);
	replacex88("MULL", str);
}

void addcode() {
	//destroys gen and addr
	char str[5];

	//record location
	add_loc = addr;

	//charge gen with value at ADD_ADDEND0 and set addr to ADD_ADDEND1
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_ADDEND1);
	addgenram();

	//transfer sum
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_SUM);
	inst("gen ramall 0000");

	//return
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_RET);
	inst("ram asnx 0000");
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
	instvalexpnxt("imm ramall", addr+1, "ADDL");
	//transfer sum
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_SUM);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_sum);
	inst("gen ramall 0000");
}	

void replaceaddcall() {
	char str[5];
	sprintf(str, "%04x", add_loc);
	replacex88("ADDL", str);
}

void keygen() {
	//A high bit means that it IS pressed and a low bit
	//means that it is not.
	//Bits from left to right corresponds to keys:
	//1,4,7,*,2,5,8,0,3,6,9,#,<not used>,<not used>,<not used>,<not used>.
	
	inst("imm dir0 e000");
	inst("imm out0 e000");
	inst("imm gen0 ffff");
	
	inst("imm dir0 e000");
	inst("imm dir1 8000");
	makeaddrodd();
	inst("in jzor 0008");
	inst("imm gen1 8000");
	inst("in jzor 0004");
	inst("imm gen1 4000");
	inst("in jzor 0002");
	inst("imm gen1 2000");
	inst("in jzor 0001");
	inst("imm gen1 1000");
	
	inst("imm dir0 e000");
	inst("imm dir1 4000");
	makeaddrodd();
	inst("in jzor 0008");
	inst("imm gen1 0800");
	inst("in jzor 0004");
	inst("imm gen1 0400");
	inst("in jzor 0002");
	inst("imm gen1 0200");
	inst("in jzor 0001");
	inst("imm gen1 0100");
	
	inst("imm dir0 e000");
	inst("imm dir1 2000");
	makeaddrodd();
	inst("in jzor 0008");
	inst("imm gen1 0080");
	inst("in jzor 0004");
	inst("imm gen1 0040");
	inst("in jzor 0002");
	inst("imm gen1 0020");
	inst("in jzor 0001");
	inst("imm gen1 0010");

	inst("imm dir0 e000");
}

void lcdinit() {
	//destroys out, gen, and dir
	//should set everything
	unsigned short delayaddr;
	
	inst("imm dir1 1fff");
	inst("imm out1 1fff");
	instval("imm out0", _LCD_RST); //rst
	//delay for reset
	inst("imm gen1 ffff");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	instval("imm out1", _LCD_RST); //!rst
	//more delay for reset
	inst("imm gen1 ffff");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//various commands
	instval("imm out0", _LCD_CS|_LCD_RS); //cs and rs low
	buswrite(0x01); //software reset
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 1000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	buswrite(0x11); //out of sleep
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 1000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	buswrite(0x38); //out of idle
	buswrite(0x29); //display is on
	buswrite(0x13); //regular display mode
	buswrite(0x20); //inversion is off
	buswrite(0x34); //tearing effect line off
	comm1dat(0x36, 0x00); //set MADCTL
	comm1dat(0x3a, 0x66); //set COLMOD
	comm1dat(0x53, 0x2c); //set WRCTRLD
	comm1dat(0x55, 0x00); //set WRCABC
	comm1dat(0x51, 0xff); //set brightness to max
	comm4dat(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4dat(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
}

void lcdresetframe() {
	instval("imm out0", _LCD_RS); //RS low
	buswrite(0x2c); //begin frame write
	instval("imm out1", _LCD_RS); //RS high
}

void lcdcontinueframe() {
	instval("imm out0", _LCD_RS); //RS low
	buswrite(0x3c); //continue frame write
	instval("imm out1", _LCD_RS); //RS high
}

void lcdpauseframe() {
	instval("imm out0", _LCD_RS); //RS low
	buswrite(0x00); //nop to end the write
}

void lcdsizeframe(int startcol, int endcol, int startpage, int endpage) {
	comm4dat(0x2a, (startcol/256)%256, startcol%256,
		 (endcol/256)%256, endcol%256); //set column min-max
	comm4dat(0x2b, (startpage/256)%256, startpage%256,
		 (endpage/256)%256, endpage%256); //set page min-max
}

void lcdboximm(int blue, int green, int red,
	    int startcol, int endcol, int startpage, int endpage) {
	//destroys addr, gen, out
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframe(startcol, endcol, startpage, endpage);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 1 extra pixel b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", ((endcol-startcol+1)*(endpage-startpage+1)+1)/2);
	makeaddr_addrpred16();
	loopaddr = addr;
	addrpred16();
	buswrite((blue<<2)%256);
	buswrite((green<<2)%256);
	buswrite((red<<2)%256);
	buswrite((blue<<2)%256);
	buswrite((green<<2)%256);
	buswrite((red<<2)%256);
	makeaddrodd();
	inst("addr jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", loopaddr);
	lcdpauseframe();
}

void main(int argc, char** argv) {
	unsigned short delayaddr, loopaddr;
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	fd = fopen(argv[2], "w+");

	inst("imm dir0 ffff");
	lcdinit();
	lcdboximm(63, 0, 0,  0, 239, 0, 319);
	lcdboximm(11, 11, 11,  33, 44, 33, 44);
	lcdboximm(0, 50, 0,  1, 100, 50, 70);
	lcdboximm(0, 0, 0,  144, 200, 200, 310);
	callmul(0xf000, 0xf001, 0xf002);
	calladd(0xf002, 0xf000, 0xf001);
	//delay
	loopaddr = addr;
	inst("imm addr0 ffff");
	inst("imm addr1 8000");
	makeaddr_addrpred16();
	delayaddr = addr;
	addrpred16();
	makeaddrodd();
	inst("addr jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	keygen();
	instnxt("dnc noop 0000", loopaddr);

	mulcode();
	addcode();
	replacemulcall();
	replaceaddcall();
	removex88(argv[1]);
}
