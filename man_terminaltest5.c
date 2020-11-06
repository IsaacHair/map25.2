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

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void callmultiply(double*prod, double factor0, double factor1) {
	*prod = factor0*factor1;
}

void domul2(double*prod, double factor) {
	*prod = 2*factor;
}

void calladd(double*sum, double addend0, double addend1) {
	*sum = addend0+addend1;
}

void dotwocomp(double*comp, double before) {
	*comp = -before;
}

void main() {
	double zr, zi, cr, ci, zrs, zis;
	int i;

	lcd_init();
	lcd_beginwrite();

	cr = 1.0;
row:
	ci = -1.40625;
column:
	i = 0;
	zr = 0.0;
	zi = 0.0;
iterate:
	callmultiply(&zrs, zr, zr);
	callmultiply(&zis, zi, zi);
	domul2(&zi, zi);
	callmultiply(&zi, zi, zr);
	calladd(&zi, zi, ci);
	dotwocomp(&zis, zis);
	calladd(&zr, zrs, zis);
	calladd(&zr, zr, cr);
	i += 1;
	if (zr > 2.0 || zi > 2.0 || zr < -2.0 || zi < -2.0)
		goto iterate_end;
	if (i < 32)
		goto iterate;
iterate_end:
	if (i < 1)
		putpixel(MAG);
	else if (i < 2)
		putpixel(RED);
	else if (i < 4)
		putpixel(YEL);
	else if (i < 8)
		putpixel(GRN);
	else if (i < 16)
		putpixel(CYA);
	else if (i < 32)
		putpixel(BLU);
	else
		putpixel(BLK);
	ci += 0.01171875;
	if (ci < 1.40625)
		goto column;
	cr -= 0.01171875;
	if (cr > -2.75)
		goto row;

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
