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
	addr+1;
}

void comm(int val) {
	char buff[100];
	int i;
	inst("imm out0 00ff");
	for (i = 

void main() {
	fd = fopen("vma412_test_0.a25", "w");
	addr = 0;
	int placeholder;
	inst("imm dir1 ffff");
	inst("imm out1 ffff");
	inst("imm out0 0800");
	//rst(); //if I am correct, this is unnecessary
	comm(0x38); //out of idle
	comm(0x11); //out of sleep
	comm(0x13); //display is on
	comm(0x20); //inversion is off
	comm1dat(0x0c, 0xe5); //set COLMOD
	comm4dat(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4dat(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
	comm(0x2c); //begin frame write
	mark(&placeholder); //just keep dumping data into the frame infinitely
	data(0x00); //red (upper 6 bits)
	data(0xc8); //green (upper 6 bits)
	data(0xd8); //blue (upper 6 bits)
	jump(placeholder);
	close(fd);
}
