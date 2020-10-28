#include <stdlib.h>
#include <stdio.h>

/*
 * A note about the map25.2 to vma412 pinout:
 * (map25.2 pins are on the right, 0x0 is lsb and rightmost bit)
 * gnd = gnd
 * 3v3 = vpp
 * lcd_rst = 0xc
 * lcd_cs = 0xb
 * lcd_rs = 0xa
 * lcd_wr = 0x9
 * lcd_rd = 0x8
 * lcd_d[7:0] = [0x7:0x0]
 * 5v, sd_ss, sd_di, sd_do, sd_sck = n/c
 */

unsigned short addr;
FILE* fd;

void inst(char*str) {
	fprintf(fd, "%04x %s %04x\n", addr, str, addr+1);
	addr++;
}

void buswrite(int val) {
	int i;
	inst("imm out0 00ff");
	fprintf(fd, "%04x imm out1 %04x %04x\n", addr, val%256, addr+1);
	addr++;
	inst("imm out0 0200");
	inst("imm out1 0200");
}

void comm1dat(int a, int b) {
	buswrite(a);
	inst("imm out1 0400");
	buswrite(b);
	inst("imm out0 0400");
}

void comm4dat(int a, int b, int c, int d, int e) {
	buswrite(a);
	inst("imm out1 0400");
	buswrite(b);
	buswrite(c);
	buswrite(d);
	buswrite(e);
	inst("imm out0 0400");
}

void jump(int mark) {
	fprintf(fd, "%04x nc noop 0000 %04x\n", addr, mark);
	addr++;
}

void main() {
	fd = fopen("vma412_test_0.a25", "w");
	addr = 0;
	int placeholder;
	inst("imm dir1 ffff");
	inst("imm out1 ffff");
	//rst(); //if I am correct, this is unnecessary
	inst("imm out0 0c00"); //cs and rs low
	buswrite(0x38); //out of idle
	buswrite(0x11); //out of sleep
	buswrite(0x13); //display is on
	buswrite(0x20); //inversion is off
	comm1dat(0x0c, 0xe5); //set COLMOD
	comm4dat(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4dat(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
	buswrite(0x2c); //begin frame write
	inst("imm out1 0400");
	placeholder = addr; //just keep dumping data into the frame infinitely
	buswrite(0x00); //red (upper 6 bits)
	buswrite(0xc8); //green (upper 6 bits)
	buswrite(0xd8); //blue (upper 6 bits)
	jump(placeholder);
	fclose(fd);
}
