#include <stdlib.h>
#include <stdio.h>

/*
 * Creates a version of the classic lunar lander game.
 * Note: As usual, function variables are pointed to by
 * immediate pointers and are not on a stack.
 */

unsigned short addr;
FILE* fd;

//note: _KEY* pinouts can be changed to anything you want
#define _KEY_R1 0x0008 //input (with 1k pull up)
#define _KEY_R2 0x0004 //input (with 1k pull up)
#define _KEY_R3 0x0002 //input (with 1k pull up)
#define _KEY_R4 0x0001 //input (with 1k pull up)
#define _KEY_C1 0x8000 //output (thru a 100ohm resistor)
#define _KEY_C2 0x4000 //output (thru a 100ohm resistor)
#define _KEY_C3 0x2000 //output (thru a 100ohm resistor)

//note: _LCD* pinouts can be changed to anything you want as well
#define _LCD_RST 0x1000 //output (thru a 100ohm resistor)
#define _LCD_CS 0x0800 //output (thru a 100ohm resistor)
#define _LCD_RS 0x0400 //output (thru a 100ohm resistor)
#define _LCD_WR 0x0200 //output (thru a 100ohm resistor)
#define _LCD_RD 0x0100 //output (thru a 100ohm resistor)

/*
 * Data bus: (cannot be changed easily; must be lower byte of output bus)
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

#define MFP_ARRAY 0x0010
#define MFP_F0 0x0004
#define MFP_F1 0x0005
#define MFP_PROD 0x0006
#define MFP_RET 0x0007
unsigned short mfp_loc;

//values stored as (16)int for now, but will need more accuracy in future
#define MAIN_X 0x0008 //lowest corner position
#define MAIN_Y 0x0009 //lowest corner position
#define MAIN_DX 0x000a //derivative of position (eg speed)
#define MAIN_DY 0x000b //derivative of position (eg speed)
#define MAIN_DDX 0x000c //2nd derivative of position (eg acceleration)
#define MAIN_DDY 0x000d //2nd derivative of position (eg acceleration)
#define MAIN_XEND 0x000e //upper corner position
#define MAIN_YEND 0x000f //upper corner position
#define MAIN_XOLD 0x0020
#define MAIN_YOLD 0x0021
#define MAIN_XENDOLD 0x0022
#define MAIN_YENDOLD 0x0023

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

void comm1datimm(int a, int b) {
	//uses immediate values
	instval("imm out0", _LCD_RS);
	buswrite(a);
	instval("imm out1", _LCD_RS);
	buswrite(b);
}

void comm4datimm(int a, int b, int c, int d, int e) {
	//uses immediate values
	instval("imm out0", _LCD_RS);
	buswrite(a);
	instval("imm out1", _LCD_RS);
	buswrite(b);
	buswrite(c);
	buswrite(d);
	buswrite(e);
}

void comm2_2datpointcut(int comm, unsigned short spoint, unsigned short epoint) {
	//uses data pointed to for command
	//writes upper byte then lower byte for spoint then epoint
	//cuts off upper 7 data bits for upper byte
	int i;
	instval("imm out0", _LCD_RS);
	buswrite(comm);
	instval("imm out1", _LCD_RS);
	inst("imm addr0 ffff");
	instval("imm addr1", spoint);
	inst("imm gen0 ffff");
	makeaddrodd();
	inst("ram jzor 0100");
	instnxt("dnc noop 0000", addr+2);
	inst("imm gen1 0001");
	buswritegen();
	inst("imm addr0 ffff");
	instval("imm addr1", spoint);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	buswritegen();
	inst("imm addr0 ffff");
	instval("imm addr1", epoint);
	inst("imm gen0 ffff");
	makeaddrodd();
	inst("ram jzor 0100");
	instnxt("dnc noop 0000", addr+2);
	inst("imm gen1 0001");
	buswritegen();
	inst("imm addr0 ffff");
	instval("imm addr1", epoint);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	buswritegen();
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
	//shifts answer down the puts carry on highest bit
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

void mfpcode() {
	//destroys *MFP_F0 and *MFP_F1
	//note that this multiplies two fixed points of the form:
	//(1)sign,(3)int,(12)fraction
	unsigned short firstloopaddr;
	unsigned short pointer;
	int i;
	unsigned short mask;
	char str[5], str1[5];

	//record location
	mfp_loc = addr;

	//initialize MFP_PROD sign
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_PROD);
	inst("imm ramall 0000");
	
	//make both factors positive and record the answer sign in MFP_PROD
	for (pointer = MFP_F0, i = 0; i < 2; i++, pointer = MFP_F1) {
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
		instval("imm addr1", MFP_PROD);
		makeaddrodd();
		inst("ram jzor 8000");
		instexpnxt("imm ramall 8000", "FINE");
		instexpnxt("imm ramall 0000", "FINE");
		sprintf(str, "%04x", addr);
		replacex88("FINE", str);
	}

	//explicitly define the procedure for recording partial products
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_F0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	//part that doesn't shift up
	//uses all 16 bits
	for (i = 0xb, mask = 0x0001; i != 0xe;
	     ((i==0) ? i = 0xf : i--), mask = mask<<1) {
		inst("imm addr0 ffff");
		instval("imm addr1", MFP_F1);
		makeaddrodd();
		instval("ram jzor", mask);
		instnxt("imm addr0 ffff", addr+2);
		instnxt("imm addr0 ffff", addr+2);
		instvalnxt("imm addr1", MFP_ARRAY|i, addr+2);
		instvalnxt("imm addr1", MFP_ARRAY|i, addr+2);
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
		instval("imm addr1", MFP_F1);
		makeaddrodd();
		instval("ram jzor", mask);
		instnxt("imm addr0 ffff", addr+2);
		instnxt("imm addr0 ffff", addr+2);
		instvalnxt("imm addr1", MFP_ARRAY|i, addr+2);
		instvalnxt("imm addr1", MFP_ARRAY|i, addr+2);
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
	instval("imm addr1", MFP_ARRAY|0xc);
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
	instval("imm addr1", MFP_ARRAY|0x0);
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
	instval("imm addr1", MFP_PROD);
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
	instval("imm addr1", MFP_RET);
	inst("ram asnx 0000");
}

void callmfp(unsigned int point_prod, unsigned int point_f0, unsigned int point_f1) {
	//transfer parameters and go to function
	inst("imm addr0 ffff");
	instval("imm addr1", point_f0);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_F0);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_f1);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_F1);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_RET);
	instvalexpnxt("imm ramall", addr+1, "MFPL");
	//transfer answer
	inst("imm addr0 ffff");
	instval("imm addr1", MFP_PROD);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_prod);
	inst("gen ramall 0000");
}

void replacemfpcall() {
	char str[5];
	sprintf(str, "%04x", mfp_loc);
	replacex88("MFPL", str);
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
	
	instval("imm dir0", _KEY_C1|_KEY_C2|_KEY_C3);
	instval("imm out0", _KEY_C1|_KEY_C2|_KEY_C3);
	inst("imm gen0 ffff");
	
	instval("imm dir0", _KEY_C1|_KEY_C2|_KEY_C3);
	instval("imm dir1", _KEY_C1);
	makeaddrodd();
	instval("in jzor", _KEY_R1);
	inst("imm gen1 8000");
	instval("in jzor", _KEY_R2);
	inst("imm gen1 4000");
	instval("in jzor", _KEY_R3);
	inst("imm gen1 2000");
	instval("in jzor", _KEY_R4);
	inst("imm gen1 1000");
	
	instval("imm dir0", _KEY_C1|_KEY_C2|_KEY_C3);
	instval("imm dir1", _KEY_C2);
	makeaddrodd();
	instval("in jzor", _KEY_R1);
	inst("imm gen1 0800");
	instval("in jzor", _KEY_R2);
	inst("imm gen1 0400");
	instval("in jzor", _KEY_R3);
	inst("imm gen1 0200");
	instval("in jzor", _KEY_R4);
	inst("imm gen1 0100");
	
	instval("imm dir0", _KEY_C1|_KEY_C2|_KEY_C3);
	instval("imm dir1", _KEY_C3);
	makeaddrodd();
	instval("in jzor", _KEY_R1);
	inst("imm gen1 0080");
	instval("in jzor", _KEY_R2);
	inst("imm gen1 0040");
	instval("in jzor", _KEY_R3);
	inst("imm gen1 0020");
	instval("in jzor", _KEY_R4);
	inst("imm gen1 0010");

	instval("imm dir0", _KEY_C1|_KEY_C2|_KEY_C3);
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
	comm1datimm(0x36, 0x00); //set MADCTL
	comm1datimm(0x3a, 0x66); //set COLMOD
	comm1datimm(0x53, 0x2c); //set WRCTRLD
	comm1datimm(0x55, 0x00); //set WRCABC
	comm1datimm(0x51, 0xff); //set brightness to max
	comm4datimm(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4datimm(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
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

void lcdsizeframeimm(int startcol, int endcol, int startpage, int endpage) {
	//size frame with immediate values
	comm4datimm(0x2a, (startcol/256)%256, startcol%256,
		 (endcol/256)%256, endcol%256); //set column min-max
	comm4datimm(0x2b, (startpage/256)%256, startpage%256,
		 (endpage/256)%256, endpage%256); //set page min-max
}

void lcdsizeframepoint(unsigned short scpoint, unsigned short ecpoint,
		       unsigned short sppoint, unsigned short eppoint) {
	//size frame with pointers
	comm2_2datpointcut(0x2a, scpoint, ecpoint);
	comm2_2datpointcut(0x2b, sppoint, eppoint);
}

void lcdboximmimm(int blue, int green, int red,
		  int startcol, int endcol, int startpage, int endpage) {
	//Start-end range is INCLUSIVE.
	//Column range is 0 to 239 and is written pixel by pixel
	//Page range is 0 to 319 and is written column by column
	//Color range is 0 to 63
	//creates a box using all immediate values
	//destroys addr, gen, out
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframeimm(startcol, endcol, startpage, endpage);
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

void lcdboximmpointimm(int blue, int green, int red,
		       unsigned short scpoint, unsigned short ecpoint,
		       unsigned short sppoint, unsigned short eppoint,
		       int size) {
	//Start-end range is INCLUSIVE.
	//Creates a box using immediate color, pointers to endpoints, and immediate size.
	//Size can be up to 76,800 (decimal), must be > 0, and is the COUNT
	//of the pixels in the box, not the index of pixels, so start counting at 1!
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframepoint(scpoint, ecpoint, sppoint, eppoint);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 3 extra pixels b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", (size+3)/2);
	makeaddr_addrpred16();
	loopaddr = addr;
	addrpred16();
	buswrite((blue<<2)%256);
	buswrite((green<<2)%256);
	buswrite((red<<2)%256);
	buswrite((blue<<2)%256);
	buswrite((green<<2)%256);
	buswrite((red<<2)%256);
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

void setimmimm(unsigned short ptr, unsigned short val) {
	//set an immediate location in ram to an immediate value
	//destroys addr
	inst("imm addr0 ffff");
	instval("imm addr1", ptr);
	instval("imm ramall", val);
}

void transimmimm(unsigned short sourceptr, unsigned short targetptr) {
	//transfer value at sourceptr to targetptr
	//destroys gen, addr
	inst("imm addr0 ffff");
	instval("imm addr1", sourceptr);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", targetptr);
	inst("gen ramall 0000");
}

void main(int argc, char** argv) {
	//note that column index is X and page index is Y
	int loopaddr, delayaddr;
	char str[5], str1[5];
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	fd = fopen(argv[2], "w+");
	inst("imm dir0 ffff");
	lcdinit();

	//init values
	setimmimm(MAIN_X, 0x0080);
	setimmimm(MAIN_Y, 0x0008);
	setimmimm(MAIN_DX, 0x0000);
	setimmimm(MAIN_DY, 0x0000);
	setimmimm(MAIN_DDX, 0x0000);
	setimmimm(MAIN_DDY, 0x0000);
	//init screen
	lcdboximmimm(0, 0, 0,  0, 239, 0, 319);
	//loop to move ship
	loopaddr = addr;
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 1800");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//actually move the ship
	transimmimm(MAIN_X, MAIN_XOLD);
	transimmimm(MAIN_Y, MAIN_YOLD);
	transimmimm(MAIN_XEND, MAIN_XENDOLD);
	transimmimm(MAIN_YEND, MAIN_YENDOLD);
	setimmimm(MAIN_DDX, 0xffff);
	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instexpnxt("dnc noop 0000", "MA_N");
	setimmimm(MAIN_DDX, 0x0001);
	sprintf(str, "%04x", addr);
	replacex88("MA_N", str);
	calladd(MAIN_DX, MAIN_DDX, MAIN_DX);
	calladd(MAIN_X, MAIN_DX, MAIN_X);
	calladd(MAIN_DY, MAIN_DDY, MAIN_DY);
	calladd(MAIN_Y, MAIN_DY, MAIN_Y);
	setimmimm(MAIN_XEND, 0x0009);
	setimmimm(MAIN_YEND, 0x0009);
	calladd(MAIN_XEND, MAIN_XEND, MAIN_X);
	calladd(MAIN_YEND, MAIN_YEND, MAIN_Y);
	lcdboximmpointimm(0, 0, 0,  MAIN_XOLD, MAIN_XENDOLD, MAIN_YOLD, MAIN_YENDOLD, 100);
	lcdboximmpointimm(63, 63, 63, MAIN_X, MAIN_XEND, MAIN_Y, MAIN_YEND, 100);
	inst("dnc noop 0000");
	sprintf(str, "%04x", addr);
	sprintf(str1, "%04x", loopaddr);
	replacex88(str, str1);

	//only write the code for the functions you use
	//mfpcode();
	addcode();
	//replacemfpcall();
	replaceaddcall();
	removex88(argv[1]);
}
