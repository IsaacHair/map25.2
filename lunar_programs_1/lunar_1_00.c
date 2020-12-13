#include <stdlib.h>
#include <stdio.h>

/*
 * Creates a version of the classic lunar lander game.
 * Note: As usual, function variables are pointed to by
 * immediate pointers and are not on a stack.
 *
 * Arithmatic uses 32 bits where the pointer to the data
 * is even, the lower data word is at the pointer, and
 * the upper data word is at the odd version of the pointer
 * (eg pointer|0x0001).
 */

unsigned short addr;
FILE* fd;
long labelcount;

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

//ADD_ADDEND* are pointers to 32 bit values
#define ADD_ADDEND0 0x0000
#define ADD_ADDEND1 0x0002
#define ADD_SUM 0x0004
#define ADD_RET 0x0005
unsigned short add_loc;
char _add_loc_label[5];

//values stored as fixed point (1,9,22) (sign, int, fraction):
#define MAIN_X 0x0006 //lowest corner position
#define MAIN_Y 0x0008 //lowest corner position
#define MAIN_DX 0x000a //derivative of position (eg speed)
#define MAIN_DY 0x000c //derivative of position (eg speed)
#define MAIN_DDX 0x000e //2nd derivative of position (eg acceleration)
#define MAIN_DDY 0x0010 //2nd derivative of position (eg acceleration)
#define MAIN_XEND 0x0012 //upper corner position
#define MAIN_YEND 0x0014 //upper corner position
#define MAIN_XOLD 0x0016
#define MAIN_YOLD 0x0018
#define MAIN_XENDOLD 0x001a
#define MAIN_YENDOLD 0x001c
//amorphous values:
#define MAIN_DUMMY 0x001e //just used as placeholder

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

void makelabel(char* str) {
	//str needs to be 5 characters to ensure good behavior (tack on '\0')
	int i, div;
	for (i = 3, div = 1; i >= 0; i--, div *= 26)
		str[i] = (labelcount/div)%26+'A';
	str[4] = '\0';
	labelcount++;
}

void replacex88expexp(char* old, char* new) {
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

void replacex88immexp(unsigned short old, char* new) {
	char str[5];
	sprintf(str, "%04x", old);
	replacex88expexp(str, new);
}

void replacex88expimm(char* old, unsigned short new) {
	char str[5];
	sprintf(str, "%04x", new);
	replacex88expexp(old, str);
}

void replacex88immimm(unsigned short old, unsigned short new) {
	char str[5], str1[5];
	sprintf(str, "%04x", old);
	sprintf(str1, "%04x", new);
	replacex88expexp(str, str1);
}

void makeaddrodd() {
	//works at 0x0000, but the lack of instruction at 0x0000 means
	//the assembly code won't even run (as map25.2 resets to 0x0000)
	if (addr%2 == 0) {
		replacex88immimm(addr, addr+1);
		addr++;
	}
}

void makeaddreven() {
	char old[5];
	char new[5];
	if (addr%2 == 1) {
		replacex88immimm(addr, addr+1);
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

void buswriteout() {
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

void comm2_2datpointdwn6cut(int comm, unsigned short spoint, unsigned short epoint) {
	//writes command, then ram[spoint]>>14, then ram[spoint]>>6, then ram[epoint]>>14, then ram[epoint]>>6
	//cuts off the highest bit of ram for the >>14 parts
	instval("imm out0", _LCD_RS);
	buswrite(comm);
	instval("imm out1", _LCD_RS);
	inst("imm addr0 ffff");
	instval("imm addr1", spoint);
	inst("imm gen0 00ff");
	makeaddrodd();
	inst("ram jzor 4000");
	instnxt("imm gen0 0001", addr+2);
	inst("imm gen1 0001");
	buswritegen();
	inst("imm gen0 ffff");
	inst("imm gen1 00ff");
	makeaddrodd();
	inst("ram jzor 0040");
	inst("imm gen0 0001");
	inst("ram jzor 0080");
	inst("imm gen0 0002");
	inst("ram jzor 0100");
	inst("imm gen0 0004");
	inst("ram jzor 0200");
	inst("imm gen0 0008");
	inst("ram jzor 0400");
	inst("imm gen0 0010");
	inst("ram jzor 0800");
	inst("imm gen0 0020");
	inst("ram jzor 1000");
	inst("imm gen0 0040");
	inst("ram jzor 2000");
	inst("imm gen0 0080");
	buswritegen();
	inst("imm addr0 ffff");
	instval("imm addr1", epoint);
	inst("imm gen0 00ff");
	makeaddrodd();
	inst("ram jzor 4000");
	instnxt("imm gen0 0001", addr+2);
	inst("imm gen1 0001");
	buswritegen();
	inst("imm gen0 ffff");
	inst("imm gen1 00ff");
	makeaddrodd();
	inst("ram jzor 0040");
	inst("imm gen0 0001");
	inst("ram jzor 0080");
	inst("imm gen0 0002");
	inst("ram jzor 0100");
	inst("imm gen0 0004");
	inst("ram jzor 0200");
	inst("imm gen0 0008");
	inst("ram jzor 0400");
	inst("imm gen0 0010");
	inst("ram jzor 0800");
	inst("imm gen0 0020");
	inst("ram jzor 1000");
	inst("imm gen0 0040");
	inst("ram jzor 2000");
	inst("imm gen0 0080");
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
	char _new_noncarry[5], _new_carry[5], _noncarry[5], _carry[5], _done[5];

	makelabel(_new_noncarry);
	makelabel(_new_carry);
	makelabel(_noncarry);
	makelabel(_carry);
	makelabel(_done);

	//do the addition
	makeaddrodd();
	instexpnxt("ram jzor 0001", _new_noncarry);
	for (mask = 0x0001; mask; mask = mask<<1) {
		replacex88expexp(_new_noncarry, _noncarry);
		replacex88expexp(_new_carry, _carry);
		makeaddreven();
		replacex88expimm(_noncarry, addr);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalnxt("imm gen1", mask, addr+2);
			instvalnxt("imm gen0", mask, addr+2);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
		}
		else {
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
			instvalexpnxt("imm gen0", mask, _done);
		}
		makeaddreven();
		replacex88expimm(_carry, addr);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
		}
		else {
			instvalexpnxt("imm gen1", mask, _done);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
		}
	}
	replacex88expimm(_done, addr);
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
	char _new_noncarry[5], _new_carry[5], _noncarry[5], _carry[5], _done[5];

	makelabel(_new_noncarry);
	makelabel(_new_carry);
	makelabel(_noncarry);
	makelabel(_carry);
	makelabel(_done);

	//do the addition
	makeaddrodd();
	instnxt("ram jzor 0001", addr+1);
	instnxt("gen jzor 0001", addr+2);
	instnxt("gen jzor 0001", addr+3);
	instexpnxt("ram jzor 0002", _new_noncarry);
	instexpnxt("ram jzor 0002", _new_noncarry);
	instexpnxt("ram jzor 0002", _new_noncarry);
	instexpnxt("ram jzor 0002", _new_carry);
	for (mask = 0x0002; mask; mask = mask<<1) {
		replacex88expexp(_new_noncarry, _noncarry);
		replacex88expexp(_new_carry, _carry);
		makeaddreven();
		replacex88expimm(_noncarry, addr);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
		}
		else {
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
		}
		makeaddreven();
		replacex88expimm(_carry, addr);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalexpnxt("ram jzor", mask<<1, _new_noncarry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
			instvalexpnxt("ram jzor", mask<<1, _new_carry);
		}
		else {
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen0", mask>>1, addr+4);
			instvalnxt("imm gen1", mask>>1, addr+4);
			instvalexpnxt("imm gen0", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
			instvalexpnxt("imm gen1", mask, _done);
		}
	}
	replacex88expimm(_done, addr);
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

void addcode32() {
	//destroys gen and addr
/*XXX*/	char str[5];

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

void calladd32(unsigned short point_sum, unsigned short point_a0, unsigned short point_a1) {
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
	instvalexpnxt("imm ramall", addr+1, _add_loc_label);
	//transfer sum
	inst("imm addr0 ffff");
	instval("imm addr1", ADD_SUM);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", point_sum);
	inst("gen ramall 0000");
}	

void replaceadd32call() {
	replacex88expimm(_add_loc_label, add_loc);
}

void makeadd32label() {
	makelabel(_add_loc_label);
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

void lcdsizeframepoint_dwn6(unsigned short scpoint, unsigned short ecpoint,
		       unsigned short sppoint, unsigned short eppoint) {
	//size frame with pointers
	comm2_2datpointdwn6cut(0x2a, scpoint, ecpoint);
	comm2_2datpointdwn6cut(0x2b, sppoint, eppoint);
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
	//ok if write 3 extra pixel b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", ((endcol-startcol+1)*(endpage-startpage+1)+3)/4);
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

void lcdboxgreyimm(int shade,
		  int startcol, int endcol, int startpage, int endpage) {
	//Start-end range is INCLUSIVE.
	//Column range is 0 to 239 and is written pixel by pixel
	//Page range is 0 to 319 and is written column by column
	//Color range is 0 to 63
	//creates a box using all immediate values
	//Point to the upper word if using 32 bit arithmatic.
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframeimm(startcol, endcol, startpage, endpage);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 3 extra pixels b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", ((endcol-startcol+1)*(endpage-startpage+1)+3)/4);
	inst("imm out0 00ff");
	instval("imm out1", (shade<<2)%256);
	makeaddr_addrpred16();
	loopaddr = addr;
	addrpred16();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
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
	//Point to the upper word if using 32 bit arithmatic.
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframepoint(scpoint, ecpoint, sppoint, eppoint);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 3 extra pixels b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", (size+3)/4);
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

void lcdboxgreypointimm(int shade,
		       unsigned short scpoint, unsigned short ecpoint,
		       unsigned short sppoint, unsigned short eppoint,
		       int size) {
	//Start-end range is INCLUSIVE.
	//Creates a box using immediate color, pointers to endpoints, and immediate size.
	//Size can be up to 76,800 (decimal), must be > 0, and is the COUNT
	//of the pixels in the box, not the index of pixels, so start counting at 1!
	//Point to the upper word if using 32 bit arithmatic.
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframepoint(scpoint, ecpoint, sppoint, eppoint);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 3 extra pixels b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", (size+3)/4);
	inst("imm out0 00ff");
	instval("imm out1", (shade<<2)%256);
	makeaddr_addrpred16();
	loopaddr = addr;
	addrpred16();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	makeaddrodd();
	inst("addr jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", loopaddr);
	lcdpauseframe();
}

void lcdboxgreypointimm_dwn6(int shade,
		       unsigned short scpoint, unsigned short ecpoint,
		       unsigned short sppoint, unsigned short eppoint,
		       int size) {
	//Start-end range is INCLUSIVE.
	//Creates a box using immediate color, pointers to endpoints, and immediate size.
	//Size can be up to 76,800 (decimal), must be > 0, and is the COUNT
	//of the pixels in the box, not the index of pixels, so start counting at 1!
	//Point to the upper word if using 32 bit arithmatic.
	unsigned short loopaddr;
	lcdpauseframe();
	lcdsizeframepoint_dwn6(scpoint, ecpoint, sppoint, eppoint);
	lcdresetframe();
	//double up on writes for speed and to fit in 16 bit counter
	//ok if write 3 extra pixels b/c just box
	//use addr register as helper
	inst("imm addr0 ffff");
	instval("imm addr1", (size+3)/4);
	inst("imm out0 00ff");
	instval("imm out1", (shade<<2)%256);
	makeaddr_addrpred16();
	loopaddr = addr;
	addrpred16();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	buswriteout();
	makeaddrodd();
	inst("addr jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", loopaddr);
	lcdpauseframe();
}

void setimmimm32(unsigned short ptr, unsigned int val) {
	//set an immediate location in ram to an immediate value
	//destroys addr
	inst("imm addr0 ffff");
	instval("imm addr1", ptr);
	instval("imm ramall", val%65536);
	inst("imm addr1 0001");
	instval("imm ramall", (val/65536));
}

void transimmimm32(unsigned short targetptr, unsigned short sourceptr) {
	//transfer value at sourceptr to targetptr
	//destroys gen, addr
	inst("imm addr0 ffff");
	instval("imm addr1", sourceptr);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", targetptr);
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", sourceptr|0x0001);
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", targetptr|0x0001);
	inst("gen ramall 0000");
}

void main(int argc, char** argv) {
	//note that column index is X and page index is Y
/*XXX*/	int loopaddr, delayaddr, checkloopaddr;
	char _next[5];
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	labelcount = 0;
	fd = fopen(argv[2], "w+");
	makeadd32label();
	makelabel(_next);
	inst("imm dir0 ffff");
	lcdinit();

	//init values
	setimmimm32(MAIN_X, 0x14200000);
	setimmimm32(MAIN_Y, 0x16900000);
	setimmimm32(MAIN_DX, 0x00000000);
	setimmimm32(MAIN_DY, 0x00000000);
	setimmimm32(MAIN_DDX, 0x00000000);
	setimmimm32(MAIN_DDY, 0x00000000);
	//init screen
	lcdboxgreyimm(0,  0, 239, 0, 319);
	//loop to move ship
	loopaddr = addr;
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 0500");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//log old values
	transimmimm(MAIN_XOLD, MAIN_X);
	transimmimm(MAIN_YOLD, MAIN_Y);
	transimmimm(MAIN_XENDOLD, MAIN_XEND);
	transimmimm(MAIN_YENDOLD, MAIN_YEND);
	//set MAIN_DDX
	//start with gravity
	setimmimm(MAIN_DDX, 0xff88);
	//accelerate up
	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, 0x0222);
	calladd(MAIN_DDX, MAIN_DDX, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	//accelerate down
	keygen();
	makeaddrodd();
	inst("gen jzor 0100");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, 0xfeef);
	calladd(MAIN_DDX, MAIN_DDX, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	//set MAIN_DDY
	//start with zero
	setimmimm(MAIN_DDY, 0x0000);
	//accelerate right (negative)
	keygen();
	makeaddrodd();
	inst("gen jzor 2000");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, 0xfeef);
	calladd(MAIN_DDY, MAIN_DDY, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	//accelerate left (positive)
	keygen();
	makeaddrodd();
	inst("gen jzor 0020");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, 0x0111);
	calladd(MAIN_DDY, MAIN_DDY, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	//time scale adjust
	//adjust speed and time scale value ONLY WHEN PRESSING BUTTON
	//Note that time scale can be adjusted to go negative or to be zero
	//(but negative has weird scaling).
	//It can recover from being zero, too.
	keygen();
	makeaddrodd();
	inst("gen jzor 0010");
	instexpnxt("dnc noop 0000", _next);
	//ensure that MAIN_DT is nonzero
	setimmimm(MAIN_DUMMY, 0x0001);
	calladd(MAIN_DT, MAIN_DT, MAIN_DUMMY);
	//scale
	setimmimm(MAIN_DUMMY, 0x1080);
	callmfp(MAIN_DT, MAIN_DT, MAIN_DUMMY);
	callmfp(MAIN_DX, MAIN_DX, MAIN_DUMMY);
	callmfp(MAIN_DY, MAIN_DY, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	keygen();
	makeaddrodd();
	inst("gen jzor 1000");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, 0x0f80);
	callmfp(MAIN_DT, MAIN_DT, MAIN_DUMMY);
	callmfp(MAIN_DX, MAIN_DX, MAIN_DUMMY);
	callmfp(MAIN_DY, MAIN_DY, MAIN_DUMMY);
	replacex88expimm(_next, addr);
	//do the calculations
	//adjust acceleration based on time scale
	callmfp(MAIN_DDX, MAIN_DDX, MAIN_DT);
	callmfp(MAIN_DDY, MAIN_DDY, MAIN_DT);
	//integrate the acceleration
	calladd(MAIN_DX, MAIN_DDX, MAIN_DX);
	calladd(MAIN_DY, MAIN_DDY, MAIN_DY);
	calladd(MAIN_X, MAIN_DX, MAIN_X);
	calladd(MAIN_Y, MAIN_DY, MAIN_Y);
	//ensure in range
	//handle MAIN_X
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_X);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, ((240-(5-1))<<6));
	calladd(MAIN_X, MAIN_DUMMY, MAIN_X);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	setimmimm(MAIN_DUMMY, 0x10000-((240-(5-1))<<6));
	calladd(MAIN_DUMMY, MAIN_DUMMY, MAIN_X);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_DUMMY);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", _next);
	transimmimm(MAIN_X, MAIN_DUMMY);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	//handle MAIN_Y
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_Y);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	setimmimm(MAIN_DUMMY, ((320-(5-1))<<6));
	calladd(MAIN_Y, MAIN_DUMMY, MAIN_Y);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	setimmimm(MAIN_DUMMY, 0x10000-((320-(5-1))<<6));
	calladd(MAIN_DUMMY, MAIN_DUMMY, MAIN_Y);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_DUMMY);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", _next);
	transimmimm(MAIN_Y, MAIN_DUMMY);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	//render
	setimmimm(MAIN_XEND, (5-1)*(1<<6));
	setimmimm(MAIN_YEND, (5-1)*(1<<6));
	calladd(MAIN_XEND, MAIN_XEND, MAIN_X);
	calladd(MAIN_YEND, MAIN_YEND, MAIN_Y);
	lcdboxgreypointimm_dwn6(0,  MAIN_XOLD, MAIN_XENDOLD, MAIN_YOLD, MAIN_YENDOLD, 25);
	lcdboxgreypointimm_dwn6(63,  MAIN_X, MAIN_XEND, MAIN_Y, MAIN_YEND, 25);
	inst("dnc noop 0000");
	//loop infinitely
	replacex88immimm(addr, loopaddr);

	add32code();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);
}
