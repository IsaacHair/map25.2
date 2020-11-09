#include <stdlib.h>
#include <stdio.h>

/*
 * Should display a mandelbrot set rendering.
 * This version isn't completed.
 */

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


#define _rorgenaddr \
	addrval = (genval*2)|((genval&0x8000)/0x8000); \
	genval = addrval
#define _immaddr(A) \
	addrval = (A)
#define _immgen1(A) \
	genval |= (A)
#define _ramgen0 \
	genval &= ~ram[addrval]
#define _genram \
	ram[addrval] = genval
#define _immram(A) \
	ram[addrval] = (A)
#define _ramgen \
	genval = ram[addrval]
#define _ramgen1 \
	genval |= ram[addrval]
#define _immgen(A) \
	genval = (A)
#define _immgen0(A) \
	genval &= ~(A)
#define _rolram \
	ram[addrval] = (genval*2)|((genval&0x8000)/0x8000)
#define _successor(A) \
	ram[A] += 1
#define _dolcd_init \
	pos = 0
#define _dolcd_beginwrite \
	1
#define _dolcd_endwrite \
	/*set terminal highlight back to white*/ \
	printf("\n\x1B[49m")
#define _doputpixel \
	printf("%s  ", color); \
	pos++; \
	if (pos > 239) { \
		printf("\n"); \
		pos = 0; \
	}


FILE* fd;
unsigned short progaddr;

void dolcd_init() {
	_dolcd_init;
}

void dolcd_beginwrite() {
	_dolcd_beginwrite;
}

void dolcd_endwrite() {
	_dolcd_endwrite;
}

void doputpixel(char*color) {
	_doputpixel;
}

void dosuccessor(unsigned short num) {
	_successor(num);
}

void dorolgenaddr() {
	_rorgenaddr;
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
