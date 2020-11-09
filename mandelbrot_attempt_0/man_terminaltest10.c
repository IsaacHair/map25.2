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
 */

#define RED "\x1B[41m"
#define MAG "\x1B[45m"
#define YEL "\x1B[43m"
#define GRN "\x1B[42m"
#define CYA "\x1B[46m"
#define BLU "\x1B[44m"
#define BLK "\x1B[40m"

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

void calltwocomp(unsigned short*comp, unsigned short* before) {
	*comp = (~(*before))+1;
}

void doasnimm(unsigned short*var, unsigned short val) {
	*var = val;
}

void doasn(unsigned short* var, unsigned short* val) {
	*var = *val;
}

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void calladd32(unsigned short*sumhigh, unsigned short*sumlow, unsigned short*addendhigh, unsigned short*addendlow) {
	unsigned int sum = 0;
	sum += (*sumhigh)*65536+(*sumlow);
	sum += (*addendhigh)*65536+(*addendlow);
	*sumhigh = sum/65536;
	*sumlow = sum%65536;
}

void do32mul2(unsigned short*endhigh, unsigned short*endlow, unsigned short*inhigh, unsigned short*inlow) {
	unsigned int sum = (*inhigh)*65536+(*inlow);
	sum *= 2;
	*endhigh = sum/65536;
	*endlow = sum%65536;
}

void do32ror12(unsigned short*endhigh, unsigned short*endlow, unsigned short*inhigh, unsigned short*inlow) {
	unsigned int num = (*inhigh)*65536+(*inlow);
	num = num>>12;
	*endhigh = num/65536;
	*endlow = num%65536;
}

void domul2(unsigned short*prod, unsigned short* factor) {
	//this is different from rol
	*prod = 2*(*factor);
}

void calladd(unsigned short*sum, unsigned short* addend0, unsigned short* addend1) {
	*sum = (*addend0)+(*addend1);
}

void callmultiply(unsigned short*prod, unsigned short* factor0, unsigned short* factor1) {
	unsigned short mulbuff0, mulbuff1, factor0rotate, bitshifter, endsign, actual0, actual1;
	doasnimm(&mulbuff0, 0x0000);
	doasnimm(&mulbuff1, 0x0000);
	doasnimm(&factor0rotate, 0x0000);
	doasnimm(&bitshifter, 0x0001);
	if ((*factor0)&0x8000) {
		calltwocomp(&actual0, factor0);
		doasnimm(&endsign, 0x8000);
	}
	else {
		doasn(&actual0, factor0);
		doasnimm(&endsign, 0x0000);
	}
	if ((*factor1)&0x8000) {
		calltwocomp(&actual1, factor1);
		if (endsign&0x8000)
			doasnimm(&endsign, 0x0000);
		else
			doasnimm(&endsign, 0x8000);
	}
	else
		doasn(&actual1, factor1);
mulloop:
	if (actual1&bitshifter)
		calladd32(&mulbuff1, &mulbuff0, &factor0rotate, &actual0);
	do32mul2(&factor0rotate, &actual0, &factor0rotate, &actual0);
	domul2(&bitshifter, &bitshifter);
	if (!(bitshifter&0x8000))
		goto mulloop;
	do32ror12(&mulbuff1, &mulbuff0, &mulbuff1, &mulbuff0);
	if (endsign&0x8000)
		calltwocomp(&mulbuff0, &mulbuff0);
	*prod = mulbuff0;
}

void main(int argc, char**argv) {
	unsigned short zr, zi, cr, ci, zrs, zis, zrn, zin; //ones place is at (1<<12)
	unsigned short i; //ones place is at (1<<0)
	unsigned short jmpbuff;

	lcd_init();
	lcd_beginwrite();

	doasnimm(&cr, 0x1000);
row:
	doasnimm(&ci, 0xe980);
column:
	doasnimm(&i, 0x0000);
	doasnimm(&zr, 0x0000);
	doasnimm(&zi, 0x0000);
iterate:
	callmultiply(&zrs, &zr, &zr);
	callmultiply(&zis, &zi, &zi);
	domul2(&zi, &zi);
	callmultiply(&zi, &zi, &zr);
	calladd(&zi, &zi, &ci);
	calltwocomp(&zis, &zis);
	calladd(&zr, &zrs, &zis);
	calladd(&zr, &zr, &cr);
	calltwocomp(&zrn, &zr);
	calltwocomp(&zin, &zi);
	doasnimm(&jmpbuff, 0x0001);
	calladd(&i, &i, &jmpbuff); //putting this here instead of after checking zr
	if (((zi&0x6000)&&!(zi&0x8000)) || (((zin)&0x6000&&!((zin)&0x8000))) ||
	    ((zr&0x6000)&&!(zr&0x8000)) || (((zrn)&0x6000&&!((zrn)&0x8000))))
		goto iterate_end;
	if (!(i&0xffe0))
		goto iterate;
iterate_end:
	if (!(i&0xfffc))
		putpixel(YEL);
	else if (!(i&0xfff8))
		putpixel(GRN);
	else if (!(i&0xfff0))
		putpixel(CYA);
	else if (!(i&0xffe0))
		putpixel(BLU);
	else
		putpixel(BLK);
	doasnimm(&jmpbuff, 0x0030);
	calladd(&ci, &ci, &jmpbuff);
	doasnimm(&jmpbuff, 0xe980);
	calladd(&jmpbuff, &ci, &jmpbuff);
	if (jmpbuff&0x8000)
		goto column;
	doasnimm(&jmpbuff, 0xffd0);
	calladd(&cr, &cr, &jmpbuff);
	doasnimm(&jmpbuff, 0x2c00);
	calladd(&jmpbuff, &cr, &jmpbuff);
	if (!(jmpbuff&0x8000))
		goto row;

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
