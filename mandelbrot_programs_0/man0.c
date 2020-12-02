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
 * This guy is gonna introduce a 16 bit adder function
 * using rotation and masks. Takes two 16 bit inputs
 * and shaves the output down to 16 bits.
 * Gonna write a gradient of data to the vma412, leaving
 * screen on during the writing and computation.
 * Should look kinda cool hopefully.
 *
 * Ok so for this version, I am expecting the user
 * to type the target file name.
 * I am also going to not require the user to pull input0
 * on the map25.2 high then low to reset. Instead, 
 * the map25.2 will schedule the reset itself.
 *
 * For this version, I am simply going to just create a color
 * gradient based on the x and y coordinates of each pixel.
 * 
 * Future versions (eg man1.c, etc) should make better use
 * of c macros and functions; this one kind of just bangs
 * everything out manually with very little use of what c can do.
 * Future versions also need to use a faster addition algorithm.
 * The overall efficiency of this program is just disgusting.
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

void buswritegen() {
	//destroys upper part of gen
	int i;
	inst("imm out0 00ff");
	inst("imm gen0 ff00");
	inst("gen out1 0000");
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

void to(int mark) {
	fprintf(fd, "%04x nc noop 0000 %04x\n", addr, mark);
	addr++;
}

void rst() {
	int marklow, markhigh;
	inst("imm out0 1000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("imm ramall ff80");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0f");
	inst("imm ramall 0001");
	inst("imm addr0 ffff");
	inst("imm addr1 ff00");
	fprintf(fd, "%04x imm ramall %04x %04x\n", addr, addr+2, addr+1);
	addr++;
	marklow = addr;
	to(0xff00);
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("gen ramall 0000");
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor ffff");
	to(addr+2);
	to(marklow);

	inst("imm out1 1000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("imm ramall f000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0f");
	inst("imm ramall 0001");
	inst("imm addr0 ffff");
	inst("imm addr1 ff00");
	fprintf(fd, "%04x imm ramall %04x %04x\n", addr, addr+2, addr+1);
	addr++;
	markhigh = addr;
	to(0xff00);
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("gen ramall 0000");
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("gen jzor ffff");
	to(addr+2);
	to(markhigh);
	inst("dnc noop 0000");
}

void addcode() {
	//adds two numbers together
	//this code is located in rom at 0xff00
	//return address is ram 0xff00
	//operand 0 is ram 0xff0e
	//operand 1 is ram 0xff0f
	//sum is stored at ram 0xff01
	//ram 0xff02 thru 0xff0c is used to buffer data for this function
	//all other ram values, including parameters, are preserved
	//the states of the gen register and addr register are NOT PRESERVED

	int buff;
	int partialsumaddr;
	buff = addr;
	addr = 0xff00;

	//transfer operands to their placeholder locations
	//0xff01 buffers operand 0
	//0xff02 buffers operand 1
	//these values in 0xff01 and 0xff02 will be written over
	//0xff02 is the left rotated "and" (eg the carry)
	//0xff01 is the "xor" (eg the partial sum)
	//keeps repeating until carry is zero
	inst("imm addr0 ffff");
	inst("imm addr1 ff0e");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff0f");
	inst("imm gen0 ffff");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff02");
	inst("gen ramall 0000");

	partialsumaddr = addr;
	
	//"or" the values at 0xff01 and 0xff02 and store at 0xff04
	inst("imm gen0 ffff");
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff02");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff04");
	inst("gen ramall 0000");
	
	//"one's complement" operand 0 and operand 1, storing at 0xff05
	//and 0xff06 respectively
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff05");
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff02");
	inst("imm gen1 ffff");
	inst("ram gen0 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff06");
	inst("gen ramall 0000");

	//"nor" the inverses and store at 0xff07
	//this is the same as "and"ing the values
	//store the rol of this value in 0xff02;
	//this is the carry value
	//note: the upper most bit is put to zero before loading
	//to avoid carry wrapping back around
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	inst("imm addr1 ff05");
	inst("ram gen0 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff06");
	inst("ram gen0 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff07");
	inst("gen ramall 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff02");
	inst("imm gen0 8000");
	inst("rol ramall 0000");

	//"xor" the values from 0xff01 and 0xff02
	//this is equivalent to setting gen to zero
	//then writing one with the "or" result at 0xff04
	//then writing zero with the "and" result at 0xff07
	//"xor" result is stored at 0xff01
	inst("imm gen0 ffff");
	inst("imm addr0 ffff");
	inst("imm addr1 ff04");
	inst("ram gen1 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff07");
	inst("ram gen0 0000");
	inst("imm addr0 ffff");
	inst("imm addr1 ff01");
	inst("gen ramall 0000");

	//repeat addition with the carry and partial sum if carry
	//is non-zero; if carry is non-zero, partial sum is sum
	//and the program can return to rom address stored
	//at ram address 0xff00
	inst("imm addr0 ffff");
	inst("imm addr1 ff02");
	if (!(addr%2))
		inst("dnc noop 0000");
	inst("ram jzor ffff");
	to(addr+2);
	to(partialsumaddr);
	inst("imm addr0 ffff");
	inst("imm addr1 ff00");
	inst("ram asnx 0000");

	addr = buff;
}

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("provide a target file please\nUsage: ./man0 <target file path/name>\n");
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
