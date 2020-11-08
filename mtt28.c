#include <stdlib.h>
#include <stdio.h>

/*
 * This serves as a way to simulate what will be going on in the map25.2
 * with its mandelbrot program.
 * Going to have a 240 by 320 character output.
 * Using spaces with colored background.
 * So, make sure the terminal window is enlarged when executing.
 * You can fit this in the terminal by making the font smaller.
 * The 240 by 320 "display" is used because this needs to almost exactly
 * replicate what the map25.2 program will be.
 * This program should also replicate the functionality of the map25.2
 * Yes, that means using gotos.
 * It also means defining math functions on a bitwise level.
 * Also going to adjust for the difference in width and height of characters.
 * In other words, characters are repeated twice left to right.
 * On the terminal display, set the font to 1px to see it properly.
 * Note: I am assuming that unsigned short is 16 bits.
 * The protol for writing funcitons is that addr register
 * and general register are NOT PRESERVED between functions or
 * code blocks.
 * Note: this version still hasnt actually separated the map25.2 functions
 * out properly; I am going to do this once everything is scripted in terms
 * of the instrucitons the map25.2 can handle.
 * This is actually gonna simulate everything down to the 64k ram.
 * Primitive instruction counter for map25.2 execution.
 * For jump operations, going to favor single, unnested if statements because
 * these are the least likely to create random shithole glitches when
 * translated to map25.2 assembly.
 * If the jump operation needs to be nested or complex, I will do this through
 * the use of virtual logic gates applied to the various factors that are
 * tested. The efficacy of this code can easily be checked with the c program
 * in the terminal.
 */

#define RED "\x1B[41m"
#define MAG "\x1B[45m"
#define YEL "\x1B[43m"
#define GRN "\x1B[42m"
#define CYA "\x1B[46m"
#define BLU "\x1B[44m"
#define BLK "\x1B[40m"

//ram and addresses of variables
//THESE ARE GLOBAL, SO RECURSIVE FUNCTIONS WILL FAIL
unsigned short ram[65536];
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

unsigned short genval;
unsigned short addrval;

int pos;

void dolcd_init() {
	pos = 0;
}

void dolcd_beginwrite() {
	1;
}

void dolcd_endwrite() {
	//set terminal highlight back to white
	printf("\n\x1B[49m");
}

void doputpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void dosuccessor(unsigned short num) {
	ram[num] += 1;
/*0005 gen jzor 00ff 0006
0006 gen jzor 0fff 0008
0007 gen jzor 000f 000a
0008 gen jzor 3fff 000c
0009 gen jzor 03ff 000e
000a gen jzor 003f 0010
000b gen jzor 0003 0012
000c gen jzor 7fff 0014
000d gen jzor 1fff 0016
000e gen jzor 07ff 0018
000f gen jzor 01ff 001a
0010 gen jzor 007f 001c
0011 gen jzor 001f 001e
0012 gen jzor 0007 0020
0013 gen jzor 0001 0022
0014 gen jzor ffff 0024
0015 imm gen1 3fff 0026
0016 imm gen1 1fff 0027
0017 imm gen1 0fff 0028
0018 imm gen1 07ff 0029
0019 imm gen1 03ff 002a
001a imm gen1 01ff 002b
001b imm gen1 00ff 002c
001c imm gen1 007f 002d
001d imm gen1 003f 002e
001e imm gen1 001f 002f
001f imm gen1 000f 0030
0020 imm gen1 0007 0031
0021 imm gen1 0003 0032
0022 imm gen1 0001 0033
0023 imm gen0 0001 0035
0024 imm gen1 ffff 0035
0025 imm gen1 7fff 0034
0026 imm gen0 4000 0035
0027 imm gen0 2000 0035
0028 imm gen0 1000 0035
0029 imm gen0 0800 0035
002a imm gen0 0400 0035
002b imm gen0 0200 0035
002c imm gen0 0100 0035
002d imm gen0 0080 0035
002e imm gen0 0040 0035
002f imm gen0 0020 0035
0030 imm gen0 0010 0035
0031 imm gen0 0008 0035
0032 imm gen0 0004 0035
0033 imm gen0 0002 0035
0034 imm gen0 8000 0035
0035 imm out1 ffff 0036
0036 gen out0 0000 0005*/
}

void dorolgenaddr() {
	addrval = (genval*2)|((genval&0x8000)/0x8000);
	genval = addrval;
}

void doonecomp(unsigned short comp, unsigned short before) {
	genval = ~ram[before];
	ram[comp] = genval;
}

void dotwocomp(unsigned short comp, unsigned short before) {
	doonecomp(comp, before);
	dosuccessor(comp);
}

void doasnimm(unsigned short var, unsigned short val) {
	ram[var] = val;
}

void doasn(unsigned short var, unsigned short val) {
	genval = ram[val];
	ram[var] = genval;
}

void doornand(unsigned short end, unsigned short in0, unsigned short in1, unsigned short in2) {
	genval = 0x0000;
	genval |= ram[in0];
	genval |= ram[in1];
	genval &= ~ram[in2];
	ram[end] = genval;
}

void donor(unsigned short end, unsigned short in0, unsigned short in1) {
	genval = 0xffff;
	genval &= ~ram[in0];
	genval &= ~ram[in1];
	ram[end] = genval;
}

void dologiccompressedandimm(unsigned short out, unsigned short in, unsigned short imm) {
	doonecomp(DLCAI_BUFF, in);
	genval = 0xffff;
	genval &= ~ram[DLCAI_BUFF];
	genval &= imm;
	if (genval&0xffff)
		ram[out] = 0x0001;
	else
		ram[out] = 0x0000;
}

void dologiconecomp(unsigned short out, unsigned short in) {
	genval = ram[in];
	if (genval&0xffff)
		ram[out] = 0x0000;
	else
		ram[out] = 0x0001;
}

void dologicnor(unsigned short result, unsigned short in0, unsigned short in1) {
	genval = 0x0001;
	genval &= ~ram[in1];
	genval &= ~ram[in0];
	ram[result] = genval;
}

void dologicor(unsigned short result, unsigned short in0, unsigned short in1) {
	dologicnor(result, in0, in1);
	if (ram[result]&0xffff)
		ram[result] = 0x0000;
	else
		ram[result] = 0x0001;
}

void domul2(unsigned short prod, unsigned short factor) {
	//this is different from rol
	genval = ram[factor];
	ram[prod] = genval*2;
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
	if(ram[ADD_CARRY]&0xffff)
		goto addloop;
	//goto ram[ADD_RETADDR];
}

void calladd(unsigned short sum, unsigned short addend0, unsigned short addend1) {
	doasn(ADD_ARG_ADDEND0, addend0);
	doasn(ADD_ARG_ADDEND1, addend1);
	doasnimm(ADD_ARG_RETADDR, /*some addr bs*/0);
	recordcodeadd(); //goto doadd
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
	genval = inlow;
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	genval &= 0x000f;
	ram[DWN12_BUFF] = genval;
	genval = inhigh;
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	dorolgenaddr();
	genval &= 0xfff0;
	genval |= ram[DWN12_BUFF];
	ram[endlow] = genval;
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
	if ((ram[MUL_ARG_FACTOR1])&0x8000) {
		dotwocomp(MUL_ACTUAL1, MUL_ARG_FACTOR1);
		if (ram[MUL_ENDSIGN]&0x8000)
			doasnimm(MUL_ENDSIGN, 0x0000);
		else
			doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else
		doasn(MUL_ACTUAL1, MUL_ARG_FACTOR1);
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
	ram[MUL_ARG_PROD] = ram[MUL_MULBUFF0];
	//goto ram[MUL_ARD_RETADDR];
}

void callmultiply(unsigned short prod, unsigned short factor0, unsigned short factor1) {
	doasn(MUL_ARG_FACTOR0, factor0);
	doasn(MUL_ARG_FACTOR1, factor1);
	doasnimm(MUL_ARG_RETADDR, /*addr+something*/0);
	recordcodemultiply(); //goto domultiply;
	doasn(prod, MUL_ARG_PROD);
}

void main(int argc, char**argv) {
	dolcd_init();
	dolcd_beginwrite();

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
