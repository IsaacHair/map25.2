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
 */

#define RED "\x1B[41m"
#define MAG "\x1B[45m"
#define YEL "\x1B[43m"
#define GRN "\x1B[42m"
#define CYA "\x1B[46m"
#define BLU "\x1B[44m"
#define BLK "\x1B[40m"

//ram and addresses of variables
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

int pos;
unsigned long instcounter0;
unsigned long instcounter1;

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
	//querty
	instcounter0 += 4;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void dosuccessor(unsigned short num) {
	ram[num] += 1;
	//querty
	instcounter0 += 15;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void doonecomp(unsigned short comp, unsigned short before) {
	ram[comp] = ~ram[before];
	//querty
	instcounter0 += 5;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void dotwocomp(unsigned short comp, unsigned short before) {
	doonecomp(comp, before);
	dosuccessor(comp);
}

void doasnimm(unsigned short var, unsigned short val) {
	ram[var] = val;
	//querty
	instcounter0 += 3;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void doasn(unsigned short var, unsigned short val) {
	ram[var] = ram[val];
	//querty
	instcounter0 += 7;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void doornand(unsigned short end, unsigned short in0, unsigned short in1, unsigned short in2) {
	unsigned short genval;
	genval = 0x0000;
	genval |= ram[in0];
	genval |= ram[in1];
	genval &= ~ram[in2];
	ram[end] = genval;
	//querty
	instcounter0 += 20;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void doand(unsigned short end, unsigned short in0, unsigned short in1) {
	doasnimm(end, 0xffff);
	ram[end] &= ~ram[in0];
	ram[end] &= ~ram[in1];
	//querty
	instcounter0 += 10;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void domul2(unsigned short prod, unsigned short factor) {
	//this is different from rol
	ram[prod] = 2*ram[factor];
	//querty
	instcounter0 += 10;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
}

void recordcodeadd() {
	doasn(ADD_ARG_SUM, ADD_ARG_ADDEND0);
	doasn(ADD_CARRY, ADD_ARG_ADDEND1);
addloop:
	doonecomp(ADD_INV0, ADD_ARG_SUM);
	doonecomp(ADD_INV1, ADD_CARRY);
	doand(ADD_AND, ADD_INV0, ADD_INV1);
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
	if (((ram[ADD32_BUFF]&0x8000)&&(ram[addendlow]&0x8000)) ||
	    ((!(ram[sumlow]&0x8000))&&((ram[ADD32_BUFF]&0x8000)||(ram[addendlow]&0x8000))))
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
	ram[DWN12_BUFF] = 0x0000;
	ram[DWN12_BUFF] |= (ram[inlow]/4096)&0x000f;
	ram[DWN12_BUFF] |= (ram[inhigh]*16)&0xfff0;
	ram[endlow] = ram[DWN12_BUFF];
	//querty
	instcounter0 += 50;
	if (instcounter0 > 1000000000) {
		instcounter0 = instcounter0 % 1000000000;
		instcounter1++;
	}
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
	if (ram[MUL_ACTUAL1]&ram[MUL_BITSHIFTER])
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
	instcounter0 = 0;
	instcounter1 = 0;

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
	if (((ram[MAIN_ZI]&0x6000)&&!(ram[MAIN_ZI]&0x8000)) ||
	    (((ram[MAIN_ZIN])&0x6000&&!((ram[MAIN_ZIN])&0x8000))) ||
	    ((ram[MAIN_ZR]&0x6000)&&!(ram[MAIN_ZR]&0x8000)) ||
	    (((ram[MAIN_ZRN])&0x6000&&!((ram[MAIN_ZRN])&0x8000))))
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
	printf("\n\ninst mod 1 billion:%ld\ninst/1billion:%ld\n", instcounter0, instcounter1);
	recordcodemultiply();
	recordcodeadd();
}
