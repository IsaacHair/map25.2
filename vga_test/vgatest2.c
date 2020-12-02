/*gonna make this literally just 1:1 correspondance
 * between pixels and instructions
 * just to make it that simple.
 */
#include <stdio.h>
#include <stdlib.h>
int addr;
FILE* target;
void out(long imm, int val) {
	if (val)
		fprintf(target, "%04x imm out1 %04x %04x\n", addr, imm, addr+1);
	else
		fprintf(target, "%04x imm out0 %04x %04x\n", addr, imm, addr+1);
	addr++;
}

void noop() {
	fprintf(target, "%04x imm noop 0000 %04x\n", addr, addr+1);
	addr++;
}
void main() {
	target = fopen("vgatest2.a25", "w");
	int i;
	int j;
	addr = 0;
	//vsync is pin 0, hsync is pin 1, color is pin 2
	//all pins are their inverse value
	out(0xffff, 0);
	for (j = 0; j < 480; j++) {
		for (i = 0; i < 10; i++)
			noop();
		out(0xffff, 0);
		out(0xfffc, 1);
		for (i = 12; i < 18; i++)
			noop();
		out(0xfffe, 1);
		out(0x0002, 0);
	}
	for (j = 480; j < 490; j++) {
		for (i = 0; i < 18; i++)
			noop();
		out(0xfffe, 1);
		out(0x0002, 0);
	}
	for (j = 490; j < 492; j++) {
		out(0xfffd, 1);
		for (i = 1; i < 18; i++)
			noop();
		out(0xfffe, 1);
		out(0x0002, 0);
	}
	for (j = 492; j < 525; j++) {
		for (i = 0; i < 18; i++)
			noop();
		out(0xfffe, 1);
		if (j != 524)
			out(0x0002, 0);
		else {
			fprintf(target, "%04x imm out0 0002 0000\n", addr);
			addr++;
		}
	}
	fclose(target);
}
