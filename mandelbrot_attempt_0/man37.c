#include <stdlib.h>
#include <stdio.h>

/*
 * Should display a mandelbrot set rendering.
 * A note about the map25.2 to vma412 pinout:
 * (map25.2 pins are on the right, 0x0 is lsb and rightmost bit)
 * gnd = gnd
 * 3v3 = vpp
 * lcd_rst = 0xc
 * lcd_cs = 0xb
 * lcd_rs = 0xa
 * lcd_wr = 0x9
 * lcd_rd = 0x8
 * lcd_d[7:0] = [0x7:0x0]
 * 5v, sd_ss, sd_di, sd_do, sd_sck = n/c
 * Logic is value, not inverse.
 */

FILE* fd;
unsigned short addr;

//locations of MUL function and ADD funciton
#define FXMUL 0xc000
#define FXADD 0x8000

//ram and addresses of variables
//THESE ARE GLOBAL, SO RECURSIVE FUNCTIONS WILL FAIL
#define MAIN_ZR 0x8000
#define MAIN_ZI 0x8001
#define MAIN_CR 0x8002
#define MAIN_CI 0x8003
#define MAIN_ZRS 0x8004
#define MAIN_ZIS 0x8005
#define MAIN_ZRN 0x8006
#define MAIN_ZIN 0x8007
#define MAIN_I 0x8008
#define MAIN_JMPBUFF 0x8009

#define MUL_MULBUFF0 0x800a
#define MUL_MULBUFF1 0x800b
#define MUL_FACTOR0ROTATE 0x800c
#define MUL_BITSHIFTER 0x800d
#define MUL_ENDSIGN 0x800e
#define MUL_ACTUAL0 0x800f
#define MUL_ACTUAL1 0x8010

#define ADD32_BUFF 0x8011
#define DWN12_BUFF 0x8012
#define DO32MUL2_BUFF 0x8013

#define MUL_ARG_PROD 0x8014
#define MUL_ARG_FACTOR0 0x8015
#define MUL_ARG_FACTOR1 0x8016
#define MUL_ARG_RETADDR 0x8017

#define ADD_ARG_SUM 0x8018
#define ADD_ARG_ADDEND0 0x8019
#define ADD_ARG_ADDEND1 0x801a
#define ADD_ARG_RETADDR 0x801b

#define ADD_INV0 0x801c
#define ADD_INV1 0x801d
#define ADD_CARRY 0x801e
#define ADD_AND 0x801f

#define MUL_JMPBUFF0 0x8020
#define MUL_JMPBUFF1 0x8021
#define MUL_JMPBUFF2 0x8022

#define ADD32_JMPBUFF0 0x8023
#define ADD32_JMPBUFF1 0x8024
#define ADD32_JMPBUFF2 0x8025
#define ADD32_JMPBUFF3 0x8026
#define ADD32_JMPBUFF4 0x8027

#define DLCAI_BUFF 0x8028

#define MAIN_JMPBUFF0 0x8029
#define MAIN_JMPBUFF1 0x802a
#define MAIN_JMPBUFF2 0x802b
#define MAIN_JMPBUFF3 0x802c
#define MAIN_JMPBUFF4 0x802d
#define MAIN_JMPBUFF5 0x802e
#define MAIN_JMPBUFF6 0x802f
#define MAIN_JMPBUFF7 0x8030

#define MUL_JMPBUFF3 0x8031
#define MUL_JMPBUFF4 0x8032
#define MUL_JMPBUFF5 0x8033
#define MUL_JMPBUFF6 0x8034
#define MUL_JMPBUFF7 0x8035
#define MUL_JMPBUFF8 0x8036

#define GARBAGE0 0x8037
#define GARBAGE1 0x8038
#define GARBAGE2 0x8039
#define GARBAGE3 0x803a


//Some general instruction/lcd functions.
void inst(char*str) {
	fprintf(fd, "%04x %s %04x\n", addr, str, addr+1);
	addr++;
}

void buswrite(int val) {
	int i;
	inst("imm out0 00ff");
	fprintf(fd, "%04x imm out1 %04x %04x\n", addr, val%256, addr+1);
	addr++;
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

void toimm(unsigned short mark) {
	fprintf(fd, "%04x nc noop 0000 %04x\n", addr, mark);
	addr++;
}

void instval(char*str, unsigned short val) {
	fprintf(fd, "%04x %s %04x %04x\n", addr, str, val, addr+1);
	addr++;
}

void instnxt(char*str, unsigned short next) {
	fprintf(fd, "%04x %s %04x\n", addr, str, next);
	addr++;
}

void rst() {
	int marklow, markhigh;
	marklow = addr;
	doasnimm(GARBAGE0, 0xffe0);
	doasnimm(GARBAGE1, 0x0001);
	calladd(GARBAGE0, GARBAGE0, GARBAGE1);
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor ffff");
	toimm(addr+2);
	toimm(marklow);
	markhigh = addr;
	doasnimm(GARBAGE0, 0xf000);
	doasnimm(GARBAGE1, 0x0001);
	calladd(GARBAGE0, GARBAGE0, GARBAGE1);
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor ffff");
	toimm(addr+2);
	toimm(markhigh);
}


//Macros for defining the operations within the rest of the program.
#define _rolgenaddr \
	inst("imm addr0 ffff"); \
	inst("ror addr1 0000"); \
	inst("imm gen0 ffff"); \
	inst("addr gen1 0000")
#define _immaddr(A) \
	inst("imm addr0 ffff"); \
	instval("imm addr1", A)
#define _immgen1(A) \
	instval("imm gen1", A)
#define _ramgen0 \
	inst("ram gen0 0000")
#define _genram \
	inst("gen ramall 0000")
#define _immram(A) \
	instval("imm ramall", A)
#define _ramgen \
	inst("imm gen0 ffff"); \
	inst("ram gen1 0000")
#define _ramgen1 \
	inst("ram gen1 0000")
#define _immgen(A) \
	inst("imm gen0 ffff"); \
	instval("imm gen1", A)
#define _immgen0(A) \
	instval("imm gen0", A)
#define _rolram \
	inst("rol ramall 0000")
#define _successor(A) \
	inst("imm addr0 ffff"); \
	instval("imm addr1", A); \
	inst("imm gen1 ffff"); \
	inst("ram gen0 0000"); \
	if(!(addr%2)) \
		inst("dnc noop 0000"); \
	instnxt("gen jzor 00ff", addr+1); \
	instnxt("gen jzor 0fff", addr+2); \
	instnxt("gen jzor 000f", addr+3); \
	instnxt("gen jzor 3fff", addr+4); \
	instnxt("gen jzor 03ff", addr+5); \
	instnxt("gen jzor 003f", addr+6); \
	instnxt("gen jzor 0003", addr+7); \
	instnxt("gen jzor 7fff", addr+8); \
	instnxt("gen jzor 1fff", addr+9); \
	instnxt("gen jzor 07ff", addr+10); \
	instnxt("gen jzor 01ff", addr+11); \
	instnxt("gen jzor 007f", addr+12); \
	instnxt("gen jzor 001f", addr+13); \
	instnxt("gen jzor 0007", addr+14); \
	instnxt("gen jzor 0001", addr+15); \
	instnxt("gen jzor ffff", addr+16); \
	instnxt("imm gen1 3fff", addr+17); \
	instnxt("imm gen1 1fff", addr+17); \
	instnxt("imm gen1 0fff", addr+17); \
	instnxt("imm gen1 07ff", addr+17); \
	instnxt("imm gen1 03ff", addr+17); \
	instnxt("imm gen1 01ff", addr+17); \
	instnxt("imm gen1 00ff", addr+17); \
	instnxt("imm gen1 007f", addr+17); \
	instnxt("imm gen1 003f", addr+17); \
	instnxt("imm gen1 001f", addr+17); \
	instnxt("imm gen1 000f", addr+17); \
	instnxt("imm gen1 0007", addr+17); \
	instnxt("imm gen1 0003", addr+17); \
	instnxt("imm gen1 0001", addr+17); \
	instnxt("imm gen0 0001", addr+18); \
	instnxt("imm gen1 ffff", addr+17); \
	instnxt("imm gen1 7fff", addr+15); \
	instnxt("imm gen0 4000", addr+15); \
	instnxt("imm gen0 2000", addr+14); \
	instnxt("imm gen0 1000", addr+13); \
	instnxt("imm gen0 0800", addr+12); \
	instnxt("imm gen0 0400", addr+11); \
	instnxt("imm gen0 0200", addr+10); \
	instnxt("imm gen0 0100", addr+9); \
	instnxt("imm gen0 0080", addr+8); \
	instnxt("imm gen0 0040", addr+7); \
	instnxt("imm gen0 0020", addr+6); \
	instnxt("imm gen0 0010", addr+5); \
	instnxt("imm gen0 0008", addr+4); \
	instnxt("imm gen0 0004", addr+3); \
	instnxt("imm gen0 0002", addr+2); \
	instnxt("imm gen0 8000", addr+1); \
	inst("gen ramall 0000"); \
	inst("imm gen1 ffff"); \
	inst("ram gen0 0000"); \
	inst("gen ramall 0000")
#define _dolcd_endwrite \
	inst("imm out0 0400"); \
	buswrite(0x00)
#define _doputpixel(R, G, B) \
	inst("imm gen0 ffff"); \
	instval("imm gen1", R); \
	buswritegen(); \
	inst("imm gen0 ffff"); \
	instval("imm gen1", G); \
	buswritegen(); \
	inst("imm gen0 ffff"); \
	instval("imm gen1", B); \
	buswritegen()


//The beginning of the actual mandelbrot functions.
void dolcd_endwrite() {
	_dolcd_endwrite;
}

void doputpixel(int red, int green, int blue) {
	_doputpixel(red, green, blue);
}

void dosuccessor(unsigned short num) {
	_successor(num);
}

void dorolgenaddr() {
	_rolgenaddr;
}

void doonecomp(unsigned short comp, unsigned short before) {
	_immaddr(before);
	_immgen1(0xffff);
	_ramgen0;
	_immaddr(comp);
	_genram;
}

void dotwocomp(unsigned short comp, unsigned short before) {
	doonecomp(comp, before);
	dosuccessor(comp);
}

void doasnimm(unsigned short var, unsigned short val) {
	_immaddr(var);
	_immram(val);
}

void doasn(unsigned short var, unsigned short val) {
	_immaddr(val);
	_ramgen;
	_immaddr(var);
	_genram;
}

void doornand(unsigned short end, unsigned short in0, unsigned short in1, unsigned short in2) {
	_immgen(0x0000);
	_immaddr(in0);
	_ramgen1;
	_immaddr(in1);
	_ramgen1;
	_immaddr(in2);
	_ramgen0;
	_immaddr(end);
	_genram;
}

void donor(unsigned short end, unsigned short in0, unsigned short in1) {
	_immgen(0xffff);
	_immaddr(in0);
	_ramgen0;
	_immaddr(in1);
	_ramgen0;
	_immaddr(end);
	_genram;
}

void dologiccompressedandimm(unsigned short out, unsigned short in, unsigned short imm) {
	doonecomp(DLCAI_BUFF, in);
	_immgen(0xffff);
	_immaddr(DLCAI_BUFF);
	_ramgen0;
	_immgen0(~imm);
	if (genval&0xffff) {
		_immaddr(out);
		_immram(0x0001);
	}
	else {
		_immaddr(out);
		_immram(0x0000);
	}
}

void dologiconecomp(unsigned short out, unsigned short in) {
	_immaddr(in);
	_ramgen;
	if (genval&0xffff) {
		_immaddr(out);
		_immram(0x0000);
	}
	else {
		_immaddr(out);
		_immram(0x0001);
	}
}

void dologicnor(unsigned short result, unsigned short in0, unsigned short in1) {
	_immgen(0x0001);
	_immaddr(in0);
	_ramgen0;
	_immaddr(in1);
	_ramgen0;
	_immaddr(result);
	_genram;
}

void dologicor(unsigned short result, unsigned short in0, unsigned short in1) {
	dologicnor(result, in0, in1);
	if (ram[result]&0xffff) {
		_immaddr(result);
		_immram(0x0000);
	}
	else {
		_immaddr(result);
		_immram(0x0001);
	}
}

void domul2(unsigned short prod, unsigned short factor) {
	//this is different from rol
	_immaddr(factor);
	_ramgen;
	_immgen0(0x8000);
	_immaddr(prod);
	_rolram;
}

void recordcodeadd() {
	doasn(ADD_ARG_SUM, ADD_ARG_ADDEND0);
	doasn(ADD_CARRY, ADD_ARG_ADDEND1);
addloop:
	doonecomp(ADD_INV0, ADD_ARG_SUM);
	doonecomp(ADD_INV1, ADD_CARRY);
	donor(ADD_AND, ADD_INV0, ADD_INV1);
	doornand(ADD_ARG_SUM, ADD_CARRY, ADD_ARG_SUM, ADD_AND);
	domul2(ADD_CARRY, ADD_AND);
	if (ram[ADD_CARRY]&0xffff)
		goto addloop;
	//goto ram[ADD_ARG_RETADDR];
}

void calladd(unsigned short sum, unsigned short addend0, unsigned short addend1) {
	doasn(ADD_ARG_ADDEND0, addend0);
	doasn(ADD_ARG_ADDEND1, addend1);
	doasnimm(ADD_ARG_RETADDR, addr+5);
	toimm(FXADD);
	doasn(sum, ADD_ARG_SUM);
}

void calladd32(unsigned short sumhigh, unsigned short sumlow, unsigned short addendhigh, unsigned short addendlow) {
	doasn(ADD32_BUFF, sumlow);
	calladd(sumlow, addendlow, sumlow);
	calladd(sumhigh, addendhigh, sumhigh);
	dologiccompressedandimm(ADD32_JMPBUFF0, ADD32_BUFF, 0x8000);
	dologiccompressedandimm(ADD32_JMPBUFF1, addendlow, 0x8000);
	dologiconecomp(ADD32_JMPBUFF0, ADD32_JMPBUFF0);
	dologiconecomp(ADD32_JMPBUFF1, ADD32_JMPBUFF1);
	dologicnor(ADD32_JMPBUFF0, ADD32_JMPBUFF0, ADD32_JMPBUFF1);
	dologiccompressedandimm(ADD32_JMPBUFF2, sumlow, 0x8000);
	dologiccompressedandimm(ADD32_JMPBUFF3, ADD32_BUFF, 0x8000);
	dologiccompressedandimm(ADD32_JMPBUFF4, addendlow, 0x8000);
	dologicor(ADD32_JMPBUFF4, ADD32_JMPBUFF3, ADD32_JMPBUFF4);
	dologiconecomp(ADD32_JMPBUFF4, ADD32_JMPBUFF4);
	dologicnor(ADD32_JMPBUFF4, ADD32_JMPBUFF4, ADD32_JMPBUFF2);
	dologicor(ADD32_JMPBUFF0, ADD32_JMPBUFF4, ADD32_JMPBUFF0);
	if (/*((ram[ADD32_BUFF]&0x8000)&&(ram[addendlow]&0x8000)) ||
	    ((!(ram[sumlow]&0x8000))&&((ram[ADD32_BUFF]&0x8000)||(ram[addendlow]&0x8000)))*/
	    ram[ADD32_JMPBUFF0]&0xffff)
		dosuccessor(sumhigh);
}

void do32mul2(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//positive inputs only, and output is sign adjusted
	doasn(DO32MUL2_BUFF, inlow);
	domul2(endlow, inlow);
	domul2(endhigh, inhigh);
	if (ram[DO32MUL2_BUFF]&0x8000)
		ram[endhigh] |= 0x0001;
}

void do32dwn12(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//endhigh is not actually changed
	_immaddr(inlow);
	_ramgen;
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	_immgen0(0xfff0);
	_immaddr(DWN12_BUFF);
	_genram;
	_immaddr(inhigh);
	_ramgen;
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	_immgen0(0x000f);
	_immaddr(DWN12_BUFF);
	_ramgen1;
	_immaddr(endlow);
	_genram;
}

void recordcodemultiply() {
	doasnimm(MUL_MULBUFF0, 0x0000);
	doasnimm(MUL_MULBUFF1, 0x0000);
	doasnimm(MUL_FACTOR0ROTATE, 0x0000);
	doasnimm(MUL_BITSHIFTER, 0x0001);
	if ((ram[MUL_ARG_FACTOR0])&0x8000) {
		dotwocomp(MUL_ACTUAL0, MUL_ARG_FACTOR0);
		doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else {
		doasn(MUL_ACTUAL0, MUL_ARG_FACTOR0);
		doasnimm(MUL_ENDSIGN, 0x0000);
	}
	dologiccompressedandimm(MUL_JMPBUFF3, MUL_ARG_FACTOR1, 0x8000);
	dologiccompressedandimm(MUL_JMPBUFF4, MUL_ENDSIGN, 0x8000);
	dologiconecomp(MUL_JMPBUFF5, MUL_JMPBUFF3);
	dologiconecomp(MUL_JMPBUFF6, MUL_JMPBUFF4);
	dologicnor(MUL_JMPBUFF7, MUL_JMPBUFF5, MUL_JMPBUFF6);
	dologicnor(MUL_JMPBUFF8, MUL_JMPBUFF5, MUL_JMPBUFF4);
	if (ram[MUL_JMPBUFF7]&0xffff) {
		dotwocomp(MUL_ACTUAL1, MUL_ARG_FACTOR1);
		doasnimm(MUL_ENDSIGN, 0x0000);
	}
	if (ram[MUL_JMPBUFF8]&0xffff) {
		dotwocomp(MUL_ACTUAL1, MUL_ARG_FACTOR1);
		doasnimm(MUL_ENDSIGN, 0x8000);
	}
	if (ram[MUL_JMPBUFF5]&0xffff)
		doasn(MUL_ACTUAL1, MUL_ARG_FACTOR1);
	/*if ((ram[MUL_ARG_FACTOR1])&0x8000) {
		dotwocomp(MUL_ACTUAL1, MUL_ARG_FACTOR1);
		if (ram[MUL_ENDSIGN]&0x8000)
			doasnimm(MUL_ENDSIGN, 0x0000);
		else
			doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else
		doasn(MUL_ACTUAL1, MUL_ARG_FACTOR1);*/
mulloop:
	doonecomp(MUL_JMPBUFF0, MUL_ACTUAL1);
	doonecomp(MUL_JMPBUFF1, MUL_BITSHIFTER);
	donor(MUL_JMPBUFF2, MUL_JMPBUFF0, MUL_JMPBUFF1);
	if (ram[MUL_JMPBUFF2]&0xffff)
		calladd32(MUL_MULBUFF1, MUL_MULBUFF0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	do32mul2(MUL_FACTOR0ROTATE, MUL_ACTUAL0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	domul2(MUL_BITSHIFTER, MUL_BITSHIFTER);
	if (!(ram[MUL_BITSHIFTER]&0x8000))
		goto mulloop;
	do32dwn12(MUL_MULBUFF1, MUL_MULBUFF0, MUL_MULBUFF1, MUL_MULBUFF0);
	if (ram[MUL_ENDSIGN]&0x8000)
		dotwocomp(MUL_MULBUFF0, MUL_MULBUFF0);
	doasn(MUL_ARG_PROD, MUL_MULBUFF0);
	//goto ram[MUL_ARG_RETADDR];
}

void callmultiply(unsigned short prod, unsigned short factor0, unsigned short factor1) {
	doasn(MUL_ARG_FACTOR0, factor0);
	doasn(MUL_ARG_FACTOR1, factor1);
	doasnimm(MUL_ARG_RETADDR, /*addr+something*/0);
	recordcodemultiply(); //goto domultiply;
	doasn(prod, MUL_ARG_PROD);
}

void main(int argc, char**argv) {
	if (argc != 2) {
		printf("need target file\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	//***BEGIN EXPLICIT MAP25.2 INSTRUCTIONS
	addr = 0;
	inst("imm dir1 ffff");
	inst("imm out1 ffff");
	rst(); //reset
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
	//***END EXPLICIT INSTRUCTIONS (these functions funnel
	//down to the macros at the file beginning)

	doasnimm(MAIN_CR, 0x1000);
row:
	doasnimm(MAIN_CI, 0xe980);
column:
	doasnimm(MAIN_I, 0x0000);
	doasnimm(MAIN_ZR, 0x0000);
	doasnimm(MAIN_ZI, 0x0000);
iterate:
	callmultiply(MAIN_ZRS, MAIN_ZR, MAIN_ZR);
	callmultiply(MAIN_ZIS, MAIN_ZI, MAIN_ZI);
	domul2(MAIN_ZI, MAIN_ZI);
	callmultiply(MAIN_ZI, MAIN_ZI, MAIN_ZR);
	calladd(MAIN_ZI, MAIN_ZI, MAIN_CI);
	dotwocomp(MAIN_ZIS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZRS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZR, MAIN_CR);
	dotwocomp(MAIN_ZRN, MAIN_ZR);
	dotwocomp(MAIN_ZIN, MAIN_ZI);
	doasnimm(MAIN_JMPBUFF, 0x0001);
	calladd(MAIN_I, MAIN_I, MAIN_JMPBUFF); //putting this here instead of after checking zr
	dologiccompressedandimm(MAIN_JMPBUFF0, MAIN_ZI, 0x6000);
	dologiccompressedandimm(MAIN_JMPBUFF1, MAIN_ZI, 0x8000);
	dologiconecomp(MAIN_JMPBUFF0, MAIN_JMPBUFF0);
	dologicnor(MAIN_JMPBUFF0, MAIN_JMPBUFF0, MAIN_JMPBUFF1);
	dologiccompressedandimm(MAIN_JMPBUFF2, MAIN_ZIN, 0x6000);
	dologiccompressedandimm(MAIN_JMPBUFF3, MAIN_ZIN, 0x8000);
	dologiconecomp(MAIN_JMPBUFF2, MAIN_JMPBUFF2);
	dologicnor(MAIN_JMPBUFF2, MAIN_JMPBUFF2, MAIN_JMPBUFF3);
	dologiccompressedandimm(MAIN_JMPBUFF4, MAIN_ZR, 0x6000);
	dologiccompressedandimm(MAIN_JMPBUFF5, MAIN_ZR, 0x8000);
	dologiconecomp(MAIN_JMPBUFF4, MAIN_JMPBUFF4);
	dologicnor(MAIN_JMPBUFF4, MAIN_JMPBUFF4, MAIN_JMPBUFF5);
	dologiccompressedandimm(MAIN_JMPBUFF6, MAIN_ZRN, 0x6000);
	dologiccompressedandimm(MAIN_JMPBUFF7, MAIN_ZRN, 0x8000);
	dologiconecomp(MAIN_JMPBUFF6, MAIN_JMPBUFF6);
	dologicnor(MAIN_JMPBUFF6, MAIN_JMPBUFF6, MAIN_JMPBUFF7);
	dologicor(MAIN_JMPBUFF0, MAIN_JMPBUFF0, MAIN_JMPBUFF2);
	dologicor(MAIN_JMPBUFF0, MAIN_JMPBUFF0, MAIN_JMPBUFF4);
	dologicor(MAIN_JMPBUFF0, MAIN_JMPBUFF0, MAIN_JMPBUFF6);
	if (/*((ram[MAIN_ZI]&0x6000)&&!(ram[MAIN_ZI]&0x8000)) ||
	    (((ram[MAIN_ZIN])&0x6000&&!((ram[MAIN_ZIN])&0x8000))) ||
	    ((ram[MAIN_ZR]&0x6000)&&!(ram[MAIN_ZR]&0x8000)) ||
	    (((ram[MAIN_ZRN])&0x6000&&!((ram[MAIN_ZRN])&0x8000)))*/
	    ram[MAIN_JMPBUFF0]&0xffff)
		goto iterate_end;
	if (!(ram[MAIN_I]&0xffe0))
		goto iterate;
iterate_end:
	if (!(ram[MAIN_I]&0xfffc))
		doputpixel(YEL);
	else if (!(ram[MAIN_I]&0xfff8))
		doputpixel(GRN);
	else if (!(ram[MAIN_I]&0xfff0))
		doputpixel(CYA);
	else if (!(ram[MAIN_I]&0xffe0))
		doputpixel(BLU);
	else
		doputpixel(BLK);
	doasnimm(MAIN_JMPBUFF, 0x0030);
	calladd(MAIN_CI, MAIN_CI, MAIN_JMPBUFF);
	doasnimm(MAIN_JMPBUFF, 0xe980);
	calladd(MAIN_JMPBUFF, MAIN_CI, MAIN_JMPBUFF);
	if (ram[MAIN_JMPBUFF]&0x8000)
		goto column;
	doasnimm(MAIN_JMPBUFF, 0xffd0);
	calladd(MAIN_CR, MAIN_CR, MAIN_JMPBUFF);
	doasnimm(MAIN_JMPBUFF, 0x2c00);
	calladd(MAIN_JMPBUFF, MAIN_CR, MAIN_JMPBUFF);
	if (!(ram[MAIN_JMPBUFF]&0x8000))
		goto row;

	dolcd_endwrite();
end:
	1; //would be goto end in the map25.2 to create infinite loop
	recordcodemultiply();
	recordcodeadd();
}
