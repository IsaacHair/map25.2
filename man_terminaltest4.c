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

void putpixel(char*color) {
	printf("%s  ", color);
	if (pos == 239) {
		printf("\n");
		pos = 0;
	}
	else
		pos++;
}

void main() {
	float zr, zi, cr, ci, zrs, zis;
	int i;

	lcd_init();

	cr = 1.0;
row:
	ci = -1.40625;
column:
	i = 0;
	zr = 0.0;
	zi = 0.0;
iterate:
	zrs = zr*zr;
	zis = zi*zi;
	zi = 2*zi*zr + ci;
	zr = zrs - zis + cr;
	if (zr > 2.0 || zi > 2.0 || zr < -2.0 || zi < -2.0)
		goto iterate_end;
	if (i < 50) {
		i += 1;
		goto iterate;
	}
iterate_end:
	if (i < 6)
		printf("%s  ", MAG);
	else if (i < 8)
		printf("%s  ", RED);
	else if (i < 10)
		printf("%s  ", YEL);
	else if (i < 14)
		printf("%s  ", GRN);
	else if (i < 20)
		printf("%s  ", CYA);
	else if (i < 50)
		printf("%s  ", BLU);
	else
		printf("%s  ", BLK);
	if (ci < 1.40625) {
		ci += 0.01171875;
		goto column;
	}
	if (cr > -2.75) {
		cr -= 0.01171875;
		printf("\n");
		goto row;
	}
	else
		goto end;
end:
	printf("\n");
}
