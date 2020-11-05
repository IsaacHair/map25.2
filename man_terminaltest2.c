#include <stdlib.h>
#include <stdio.h>

/*
 * This serves as a way to simulate what will be going on in the map25.2
 * with its mandelbrot program.
 * Going to have a 240 by 320 character output.
 * Yeah, I'm just punting and using '#' and ' ' among other symbols to
 * create the display because I can't be bothered to learn
 * a graphics library right now.
 * So, make sure the terminal window is enlarged when executing.
 * You can fit this in the terminal by making the font smaller.
 * The 240 by 320 "display" is used because this needs to almost exactly
 * replicate what the map25.2 program will be.
 * This program should also replicate the functionality of the map25.2
 * Yes, that means using gotos.
 * It also means defining math functions on a bitwise level.
 */

void main() {
	//computes 25 repetitions of the rule z(n+1) = z(n)^2+c
	//if the value of z becomes >= 2.0 during 25 repetitions,
	//it is not in the set
	//real is vertical and imaginary horizontal
	
	float zr, zi, cr, ci, zrs, zis;
	int i;
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
	if (i < 25) {
		i += 1;
		goto iterate;
	}
iterate_end:
	if (i < 3)
		printf(" ");
	else if (i < 5)
		printf(".");
	else if (i < 10)
		printf("*");
	else if (i < 25)
		printf("x");
	else
		printf("#");
	if (ci < 1.40625) {
		ci += 0.0234375; ///need to adjust for 240 instead of 120
		goto column;
	}
	if (cr > -2.75) {
		cr -= 0.0234375;
		printf("\n");
		goto row;
	}
	else
		goto end;
end:
	printf("\n");
}
