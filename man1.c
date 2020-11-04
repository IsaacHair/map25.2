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
 * Logic is value, not inverse.
 *
 * This program introduces a few functions:
 * 0. Edge-detection-based successor function.
 * 1. Fast adder built with the successor function and rotation.
 * 2. Multiplier with a fraction component
 * 3. Complex number multiplier with fraction component
 * 4. Various LCD functions
 * 5. Mandelbrot renderer.
 *
 * A mandelbrot set is rendered and displayed.
 */

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("provide a target file please\nUsage:\
			./man0 <target file path/name>\n");
		exit(0x01);
	}
	fd = fopen(argv[1], "w");
	addr = 0;
	int placeholder;
	inst("imm dir1 ffff");
	inst("imm out1 ffff");
	rst(); //reset
	inst("imm out0 0c00"); //cs and rs low
	buswrite(0x38); //out of idle
	buswrite(0x11); //out of sleep
	buswrite(0x13); //normal display mode
	buswrite(0x20); //inversion is off
	buswrite(0x29); //display is on
	comm1dat(0x0c, 0xe6); //set COLMOD
	comm4dat(0x2a, 0x00, 0x00, 0x00, 0xef); //set column min-max
	comm4dat(0x2b, 0x00, 0x00, 0x01, 0x3f); //set page min-max
	buswrite(0x2c); //begin frame write
	inst("imm out1 0400"); //RS high

	//initialize column and row value
	//column indexes are 0x0000 thru 0x00ef and rows are 0x0000 thru 0x013f
	//when printing, columns count up, then, once they reach 0x00ef,
	//it is reset to 0x0000 and rows increases by 1
	//when columns reach 0x013f, write is terminated with no-op to lcd
	//column counter at ram 0xfe00 and rows at 0xfe01
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("imm ramall 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 fe01");
	inst("imm ramall 0000");

	//initialize addition (second operand is set to 1)
	inst("imm addr0 ffff");
	inst("imm addr1 ff0f");
	inst("imm ramall 0001");

	placeholder = addr; //frame write loop
	
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	buswritegen(); //red (upper 6 bits) (aquires (column value)/4)
	inst("imm addr0 ffff");
	inst("imm addr1 fe01");
	inst("imm gen0 ffff");
	inst("ram gen1 ffff");
	inst("imm addr0 ffff");
	inst("ror addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 ffff");
	buswritegen(); //green (upper 6 bits) (aquires (row value)/8)
	buswritegen(); //blue (upper 6 bits) (aquires (row value)/8)
	
	//decide if column or row is being added
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor 00ef");
	to(addr+20);
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("gen ramall 0000");
	//call "add" and set return address
	inst("imm addr0 ffff");
	inst("imm addr1 ff00");
	fprintf(fd, "%04x imm ramall %04x %04x\n", addr, addr+2, addr+1);
	addr++;
	to(0xff00);
	//store result in column value
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("gen ramall 0000");
	//loop again
	to(placeholder);

	//this is to increment rows and set column to zero
	inst("imm addr0 ffff");
	inst("imm addr1 fe00");
	inst("imm ramall 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 fe01");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor 013f");
	to(addr+20);
	inst("imm addr0 ffff");
	inst("imm addr1 fe01");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("gen ramall 0000");
	//call "add" and set return address
	inst("imm addr0 ffff");
	inst("imm addr1 ff00");
	fprintf(fd, "%04x imm ramall %04x %04x\n", addr, addr+2, addr+1);
	addr++;
	to(0xff00);
	//store result in row value
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 fe01");
	inst("gen ramall 0000");
	//loop again
	to(placeholder);

	//the frame write is done, so exit (eg enter infinite loop)
	inst("imm out0 0400");
	buswrite(0x00);
	placeholder = addr;
	to(addr);

	addcode();
	fclose(fd);
}
