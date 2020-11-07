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
 */

//bash terminal color escape codes
#define RED "\x1B[41m"
#define MAG "\x1B[45m"
#define YEL "\x1B[43m"
#define GRN "\x1B[42m"
#define CYA "\x1B[46m"
#define BLU "\x1B[44m"
#define BLK "\x1B[40m"

//ram and addresses of variables
//NOTE: BECAUSE FUNCTION VARIABLE POINTERS
//ARE IMMEDIATE VALUES AND NOT ON A FUNCTION STACK,
//RECURSIVE FUNCTIONS WILL NOT WORK.
//For future programs, I will define a stack,
//but it is unncecessary for this program.
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

#define DWN12_BUFF 0x8011
#define DOMUL2_BUFF 0x8012
#define DO32MUL2_BUFF 0x8013
#define ADD32_BUFFLOW 0x8014
#define ADD32_BUFFHIGH 0x8015

int pos;

void lcd_init() {
	pos = 0;
}

void lcd_beginwrite() {
	1;
}

void lcd_endwrite() {
	//set terminal highlight back to white
	printf("\n\x1B[49m");
}

void calltwocomp(unsigned short comp, unsigned short before) {
	ram[comp] = (~ram[before])+1;
}

void doasnimm(unsigned short var, unsigned short val) {
	ram[var] = val;
}

void doasn(unsigned short var, unsigned short val) {
	ram[var] = ram[val];
}

void calladd(unsigned short sum, unsigned short addend0, unsigned short addend1) {
	//twos complement addition automatically manages the sign for you
	ram[sum] = ram[addend0]+ram[addend1];
}

void domul2(unsigned short prod, unsigned short factor) {
	//note: this DOES NOT FIX SIGN, so it is just the bit shift
	//sign only becomes a problem if the number is within 1 digit of max
	ram[prod] = ram[factor]*2;
	ram[prod] &= 0xfffe; //zeroth bit to zero
}

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void dosuccessor(unsigned short num) {
	ram[num] += 1;
}

void calladd32(unsigned short sumhigh, unsigned short sumlow, unsigned short addendhigh, unsigned short addendlow) {
	/*doasn(ADD32_BUFFLOW, sumlow);
	calladd(sumlow, addendlow, sumlow);
	calladd(sumhigh, addendhigh, sumhigh);
	if (((ram[ADD32_BUFFLOW]&0x8000)&&(ram[addendlow]&0x8000)) ||
	    ((!(ram[sumlow]&0x8000))&&((ram[ADD32_BUFFLOW]&0x8000)||(ram[addendlow]&0x8000))))
		dosuccessor(sumhigh);*/
	unsigned int sum = 0;
	sum += (ram[sumhigh])*65536+(ram[sumlow]);
	sum += (ram[addendhigh])*65536+(ram[addendlow]);
	ram[sumhigh] = sum/65536;
	ram[sumlow] = sum%65536;
}

void do32mul2(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//positive inputs only, and output is sign adjusted
	domul2(endlow, inlow);
	domul2(endhigh, inhigh);
	if (ram[inlow]&0x8000)
		ram[endhigh] |= 0x0001;
}

void do32dwn12(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//endhigh is not actually changed
	ram[DWN12_BUFF] = 0x0000;
	ram[DWN12_BUFF] |= (ram[inlow]>>12)&0x000f;
	ram[DWN12_BUFF] |= (ram[inhigh]<<4)&0xfff0;
	ram[endlow] = ram[DWN12_BUFF];
}

void callmultiply(unsigned short prod, unsigned short factor0, unsigned short factor1) {
	doasnimm(MUL_MULBUFF0, 0x0000);
	doasnimm(MUL_MULBUFF1, 0x0000);
	doasnimm(MUL_FACTOR0ROTATE, 0x0000);
	doasnimm(MUL_BITSHIFTER, 0x0001);
	if ((ram[factor0])&0x8000) {
		calltwocomp(MUL_ACTUAL0, factor0);
		doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else {
		doasn(MUL_ACTUAL0, factor0);
		doasnimm(MUL_ENDSIGN, 0x0000);
	}
	if ((ram[factor1])&0x8000) {
		calltwocomp(MUL_ACTUAL1, factor1);
		if (ram[MUL_ENDSIGN]&0x8000)
			doasnimm(MUL_ENDSIGN, 0x0000);
		else
			doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else
		doasn(MUL_ACTUAL1, factor1);
mulloop:
	//note: this part requires that the factors are all positive
	if (ram[MUL_ACTUAL1]&ram[MUL_BITSHIFTER])
		calladd32(MUL_MULBUFF1, MUL_MULBUFF0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	do32mul2(MUL_FACTOR0ROTATE, MUL_ACTUAL0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	domul2(MUL_BITSHIFTER, MUL_BITSHIFTER);
	if (!(ram[MUL_BITSHIFTER]&0x8000))
		goto mulloop;
	do32dwn12(MUL_MULBUFF1, MUL_MULBUFF0, MUL_MULBUFF1, MUL_MULBUFF0);
	if (ram[MUL_ENDSIGN]&0x8000)
		calltwocomp(MUL_MULBUFF0, MUL_MULBUFF0);
	ram[prod] = ram[MUL_MULBUFF0];
}

void main(int argc, char**argv) {
	lcd_init();
	lcd_beginwrite();
/*
	if (argc != 2) {
		printf("need args\n");
		exit(0x01);
	}
	ram[0] = atoi(argv[1])/65536;
	ram[1] = atoi(argv[1])%65536;
	do32mul2(3, 2, 0, 1);
	printf("product:%d\n", ram[3]*65536+ram[2]);
*/
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
	calltwocomp(MAIN_ZIS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZRS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZR, MAIN_CR);
	calltwocomp(MAIN_ZRN, MAIN_ZR);
	calltwocomp(MAIN_ZIN, MAIN_ZI);
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
		putpixel(YEL);
	else if (!(ram[MAIN_I]&0xfff8))
		putpixel(GRN);
	else if (!(ram[MAIN_I]&0xfff0))
		putpixel(CYA);
	else if (!(ram[MAIN_I]&0xffe0))
		putpixel(BLU);
	else
		putpixel(BLK);
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

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
