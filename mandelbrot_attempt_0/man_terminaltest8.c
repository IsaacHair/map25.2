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

void doasn(unsigned short*var, unsigned short val) {
	*var = val;
}

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void callmultiply(unsigned short*prod, unsigned short factor0, unsigned short factor1) {
	double f0, f1;
	f0 = ((double)(factor0&0x7fff))/4096.0;
	if (factor0&0x8000)
		f0 -= 8;
	f1 = ((double)(factor1&0x7fff))/4096.0;
	if (factor1&0x8000)
		f1 -= 8;
	*prod = (unsigned short)((f1*f0)*4096.0);
}

void domul2(unsigned short*prod, unsigned short factor) {
	*prod = 2*factor;
}

void calladd(unsigned short*sum, unsigned short addend0, unsigned short addend1) {
	*sum = addend0+addend1;
}

void dotwocomp(unsigned short*comp, unsigned short before) {
	*comp = (~before)+1;
}

double read(unsigned short num) {
	double result;
	result = ((double)(num&0x7fff))/4096.0;
	if (num&0x8000)
		result -= 8;
	return result;
}

void main(int argc, char**argv) {
	unsigned short zr, zi, cr, ci, zrs, zis, zrn, zin; //ones place is at (1<<12)
	unsigned short i; //ones place is at (1<<0)
	unsigned short jmpbuff;

	lcd_init();
	lcd_beginwrite();

	doasn(&cr, 0x1000);
row:
	doasn(&ci, 0xe980);
column:
	doasn(&i, 0x0000);
	doasn(&zr, 0x0000);
	doasn(&zi, 0x0000);
iterate:
	callmultiply(&zrs, zr, zr);
	callmultiply(&zis, zi, zi);
	domul2(&zi, zi);
	callmultiply(&zi, zi, zr);
	calladd(&zi, zi, ci);
	dotwocomp(&zis, zis);
	calladd(&zr, zrs, zis);
	calladd(&zr, zr, cr);
	dotwocomp(&zrn, zr);
	dotwocomp(&zin, zi);
	calladd(&i, i, 0x0001); //putting this here instead of after checking zr
	if (((zi&0x6000)&&!(zi&0x8000)) || (((zin)&0x6000&&!((zin)&0x8000))) ||
	    ((zr&0x6000)&&!(zr&0x8000)) || (((zrn)&0x6000&&!((zrn)&0x8000))))
		goto iterate_end;
	if (i < 0x0020)
		goto iterate;
iterate_end:
	if (i < 0x0004)
		putpixel(YEL);
	else if (i < 0x0008)
		putpixel(GRN);
	else if (i < 0x0010)
		putpixel(CYA);
	else if (i < 0x0020)
		putpixel(BLU);
	else
		putpixel(BLK);
	calladd(&ci, ci, 0x0030);
	calladd(&jmpbuff, ci, 0xe980);
	if (jmpbuff&0x8000)
		goto column;
	calladd(&cr, cr, 0xffd0);
	calladd(&jmpbuff, cr, 0x2c00);
	if (!(jmpbuff&0x8000))
		goto row;

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
