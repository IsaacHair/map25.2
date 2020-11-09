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
 * The protol for writing funcitons is that addr register
 * and general register are NOT PRESERVED between functions or
 * code blocks.
 * Note: this version still hasnt actually separated the map25.2 functions
 * out properly; I am going to do this once everything is scripted in terms
 * of the instrucitons the map25.2 can handle.
 * This is actually gonna simulate everything down to the 64k ram.
 */

#define RED "\x1B[41m"
#define MAG "\x1B[45m"
#define YEL "\x1B[43m"
#define GRN "\x1B[42m"
#define CYA "\x1B[46m"
#define BLU "\x1B[44m"
#define BLK "\x1B[40m"

//ram and addresses of variables
unsigned short ram[65536];
#define MAIN_ZR 0x8000
#define MAIN_ZI 0x8001
#define MAIN_CR 0x8002
#define MAIN_CI 0x8003
#define MAIN_ZRS 0x8004
#define MAIN_ZIS 0x8005
#define MAIN_ZRN 0x8006
#define MAIN_ZIN 0x8007
#define MAIN_I 0x8008
#define MAIN_JMPBUFF 0x8009

#define MUL_MULBUFF0 0x800a
#define MUL_MULBUFF1 0x800b
#define MUL_FACTOR0ROTATE 0x800c
#define MUL_BITSHIFTER 0x800d
#define MUL_ENDSIGN 0x800e
#define MUL_ACTUAL0 0x800f
#define MUL_ACTUAL1 0x8010

#define ADD32_BUFF 0x8011
#define DWN12_BUFF 0x8012
#define DO32MUL2_BUFF 0x8013

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

void calltwocomp(unsigned short comp, unsigned short before) {
	ram[comp] = (~ram[before])+1;
}

void doasnimm(unsigned short var, unsigned short val) {
	ram[var] = val;
}

void doasn(unsigned short var, unsigned short val) {
	ram[var] = ram[val];
}

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void domul2(unsigned short prod, unsigned short factor) {
	//this is different from rol
	ram[prod] = 2*ram[factor];
}

void calladd(unsigned short sum, unsigned short addend0, unsigned short addend1) {
	ram[sum] = ram[addend0]+ram[addend1];
}

void dosuccessor(unsigned short num) {
	ram[num] += 1;
}

void calladd32(unsigned short sumhigh, unsigned short sumlow, unsigned short addendhigh, unsigned short addendlow) {
	doasn(ADD32_BUFF, sumlow);
	calladd(sumlow, addendlow, sumlow);
	calladd(sumhigh, addendhigh, sumhigh);
	if (((ram[ADD32_BUFF]&0x8000)&&(ram[addendlow]&0x8000)) ||
	    ((!(ram[sumlow]&0x8000))&&((ram[ADD32_BUFF]&0x8000)||(ram[addendlow]&0x8000))))
		dosuccessor(sumhigh);
}

void do32mul2(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//positive inputs only, and output is sign adjusted
	doasn(DO32MUL2_BUFF, inlow);
	domul2(endlow, inlow);
	domul2(endhigh, inhigh);
	if (ram[DO32MUL2_BUFF]&0x8000)
		ram[endhigh] |= 0x0001;
}

void do32dwn12(unsigned short endhigh, unsigned short endlow, unsigned short inhigh, unsigned short inlow) {
	//endhigh is not actually changed
	ram[DWN12_BUFF] = 0x0000;
	ram[DWN12_BUFF] |= (ram[inlow]/4096)&0x000f;
	ram[DWN12_BUFF] |= (ram[inhigh]*16)&0xfff0;
	ram[endlow] = ram[DWN12_BUFF];
}

void callmultiply(unsigned short prod, unsigned short factor0, unsigned short factor1) {
	doasnimm(MUL_MULBUFF0, 0x0000);
	doasnimm(MUL_MULBUFF1, 0x0000);
	doasnimm(MUL_FACTOR0ROTATE, 0x0000);
	doasnimm(MUL_BITSHIFTER, 0x0001);
	if ((ram[factor0])&0x8000) {
		calltwocomp(MUL_ACTUAL0, factor0);
		doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else {
		doasn(MUL_ACTUAL0, factor0);
		doasnimm(MUL_ENDSIGN, 0x0000);
	}
	if ((ram[factor1])&0x8000) {
		calltwocomp(MUL_ACTUAL1, factor1);
		if (ram[MUL_ENDSIGN]&0x8000)
			doasnimm(MUL_ENDSIGN, 0x0000);
		else
			doasnimm(MUL_ENDSIGN, 0x8000);
	}
	else
		doasn(MUL_ACTUAL1, factor1);
mulloop:
	if (ram[MUL_ACTUAL1]&ram[MUL_BITSHIFTER])
		calladd32(MUL_MULBUFF1, MUL_MULBUFF0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	do32mul2(MUL_FACTOR0ROTATE, MUL_ACTUAL0, MUL_FACTOR0ROTATE, MUL_ACTUAL0);
	domul2(MUL_BITSHIFTER, MUL_BITSHIFTER);
	if (!(ram[MUL_BITSHIFTER]&0x8000))
		goto mulloop;
	do32dwn12(MUL_MULBUFF1, MUL_MULBUFF0, MUL_MULBUFF1, MUL_MULBUFF0);
	if (ram[MUL_ENDSIGN]&0x8000)
		calltwocomp(MUL_MULBUFF0, MUL_MULBUFF0);
	ram[prod] = ram[MUL_MULBUFF0];
}

/*void addcode() {
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
}*/

void main(int argc, char**argv) {
	lcd_init();
	lcd_beginwrite();

/*	if (argc != 3) {
		printf("need args\n");
		exit(0x01);
	}
	ram[0] = atoi(argv[1]);
	ram[1] = atoi(argv[2]);
	callmultiply(2, 1, 0);
	printf("product:%04x\n", ram[2]);
*/
	doasnimm(MAIN_CR, 0x1000);
row:
	doasnimm(MAIN_CI, 0xe980);
column:
	doasnimm(MAIN_I, 0x0000);
	doasnimm(MAIN_ZR, 0x0000);
	doasnimm(MAIN_ZI, 0x0000);
iterate:
	callmultiply(MAIN_ZRS, MAIN_ZR, MAIN_ZR);
	callmultiply(MAIN_ZIS, MAIN_ZI, MAIN_ZI);
	domul2(MAIN_ZI, MAIN_ZI);
	callmultiply(MAIN_ZI, MAIN_ZI, MAIN_ZR);
	calladd(MAIN_ZI, MAIN_ZI, MAIN_CI);
	calltwocomp(MAIN_ZIS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZRS, MAIN_ZIS);
	calladd(MAIN_ZR, MAIN_ZR, MAIN_CR);
	calltwocomp(MAIN_ZRN, MAIN_ZR);
	calltwocomp(MAIN_ZIN, MAIN_ZI);
	doasnimm(MAIN_JMPBUFF, 0x0001);
	calladd(MAIN_I, MAIN_I, MAIN_JMPBUFF); //putting this here instead of after checking zr
	if (((ram[MAIN_ZI]&0x6000)&&!(ram[MAIN_ZI]&0x8000)) ||
	    (((ram[MAIN_ZIN])&0x6000&&!((ram[MAIN_ZIN])&0x8000))) ||
	    ((ram[MAIN_ZR]&0x6000)&&!(ram[MAIN_ZR]&0x8000)) ||
	    (((ram[MAIN_ZRN])&0x6000&&!((ram[MAIN_ZRN])&0x8000))))
		goto iterate_end;
	if (!(ram[MAIN_I]&0xffe0))
		goto iterate;
iterate_end:
	if (!(ram[MAIN_I]&0xfffc))
		putpixel(YEL);
	else if (!(ram[MAIN_I]&0xfff8))
		putpixel(GRN);
	else if (!(ram[MAIN_I]&0xfff0))
		putpixel(CYA);
	else if (!(ram[MAIN_I]&0xffe0))
		putpixel(BLU);
	else
		putpixel(BLK);
	doasnimm(MAIN_JMPBUFF, 0x0030);
	calladd(MAIN_CI, MAIN_CI, MAIN_JMPBUFF);
	doasnimm(MAIN_JMPBUFF, 0xe980);
	calladd(MAIN_JMPBUFF, MAIN_CI, MAIN_JMPBUFF);
	if (ram[MAIN_JMPBUFF]&0x8000)
		goto column;
	doasnimm(MAIN_JMPBUFF, 0xffd0);
	calladd(MAIN_CR, MAIN_CR, MAIN_JMPBUFF);
	doasnimm(MAIN_JMPBUFF, 0x2c00);
	calladd(MAIN_JMPBUFF, MAIN_CR, MAIN_JMPBUFF);
	if (!(ram[MAIN_JMPBUFF]&0x8000))
		goto row;

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
