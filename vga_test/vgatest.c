/* produces an assembly file to run the program */
#include <stdio.h>
#include <stdlib.h>

void main() {
	FILE* target = fopen("vgatest.a25", "w");
	int addr = 0;
	long i, j;
	//Note: pulses are opposite what they should be so that
	//bread board can invert them to protect map25.2
	fprintf(target, "%04x imm dir1 ffff %04x", addr, addr+1);
	addr++;
	fprintf(target, "%04x imm out0 ffff %04x", addr, addr+1);
	addr++;
	fprintf(target, "%04x imm out1 fffc %04x", addr, addr+1);
	addr++;
	for (i = 0; i < 525; i++) {
		fprintf(target, "%04x imm addr0 ffff %04x", addr, addr+1);
		addr++;
		fprintf(target, "%04x imm addr1 %04x %04x", addr, i, addr+1);
		addr++;
		if (i == 524)
			j = 0;
		else if (i == 490)
			j = 490|0x8000;
		else if (i == 491)
			j = 491|0x8000;
		else
			j = i+1;
		fprintf(target, "%04x imm ramall %04x %04x", addr, j, addr+1);
		addr++;
	}
	fprintf(target, "%04x imm addr0 ffff %04x", addr, addr+1);
	addr++;
	for (i = 0; i < 20; i++) {
		if (i == 0) {
			fprintf(target, "%04x imm gen0 ffff %04x", addr, addr+1);
			addr++;
		}
		if (i == 1) {
			fprintf(target, "%04x ram gen1 0000 %04x", addr, addr+1);
			addr++;
		}
		if (i == 2) {
			fprintf(target, "%04x imm addr0 ffff %04x", addr, addr+1);
			addr++;
		}
		if (i == 3) {
			fprintf(target, "%04x gen addr1 ffff %04x", addr, addr+1);
			addr++;
		}
		if (i == 4) {
			fprintf(target, "%04x imm out0 0001 %04x", addr, addr+1);
			addr++;
		}
		if (i == 5) {
			fprintf(target, "%04x rol out1 0001 %04x\n", addr, addr+1);
			addr++;
		}
		if (i == 12) {
			fprintf(target, "%04x imm out0 0004 %04x", addr, addr+1);
			addr++;
		}
		else if (i == 13) {
			fprintf(target, "%04x imm out1 0004 %04x", addr, addr+1);
			addr++;
		}
		else if (i == 18) {
			fprintf(target, "%04x imm out1 0002 %04x", addr, addr+1);
			addr++;
		}
		else if (i == 19) {
			fprintf(target, "%04x imm out0 0002 %04x", addr, addr+1);
			addr++;
		}
		else
			fprintf(target, "%04x imm noop 0000 %04x", addr, addr+1);
			addr++;
	}
}
