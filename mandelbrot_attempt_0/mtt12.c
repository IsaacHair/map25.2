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
#define MAIN_BUFF 0x8009

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

void calltwocomp(unsigned short*comp, unsigned short* before) {
	*comp = (~(*before))+1;
}

void doasnimm(unsigned short*var, unsigned short val) {
	*var = val;
}

void doasn(unsigned short* var, unsigned short* val) {
	*var = *val;
}

void putpixel(char*color) {
	printf("%s  ", color);
	pos++;
	if (pos > 239) {
		printf("\n");
		pos = 0;
	}
}

void calladd32(unsigned short*sumhigh, unsigned short*sumlow, unsigned short*addendhigh, unsigned short*addendlow) {
	unsigned int sum = 0;
	sum += (*sumhigh)*65536+(*sumlow);
	sum += (*addendhigh)*65536+(*addendlow);
	*sumhigh = sum/65536;
	*sumlow = sum%65536;
}

void do32mul2(unsigned short*endhigh, unsigned short*endlow, unsigned short*inhigh, unsigned short*inlow) {
	unsigned int sum = (*inhigh)*65536+(*inlow);
	sum *= 2;
	*endhigh = sum/65536;
	*endlow = sum%65536;
}

void do32ror12(unsigned short*endhigh, unsigned short*endlow, unsigned short*inhigh, unsigned short*inlow) {
	unsigned int num = (*inhigh)*65536+(*inlow);
	num = num>>12;
	*endhigh = num/65536;
	*endlow = num%65536;
}

void domul2(unsigned short*prod, unsigned short* factor) {
	//this is different from rol
	*prod = 2*(*factor);
}

void calladd(unsigned short*sum, unsigned short* addend0, unsigned short* addend1) {
	*sum = (*addend0)+(*addend1);
}

void callmultiply(unsigned short*prod, unsigned short* factor0, unsigned short* factor1) {
	unsigned short mulbuff0, mulbuff1, factor0rotate, bitshifter, endsign, actual0, actual1;
	doasnimm(&mulbuff0, 0x0000);
	doasnimm(&mulbuff1, 0x0000);
	doasnimm(&factor0rotate, 0x0000);
	doasnimm(&bitshifter, 0x0001);
	if ((*factor0)&0x8000) {
		calltwocomp(&actual0, factor0);
		doasnimm(&endsign, 0x8000);
	}
	else {
		doasn(&actual0, factor0);
		doasnimm(&endsign, 0x0000);
	}
	if ((*factor1)&0x8000) {
		calltwocomp(&actual1, factor1);
		if (endsign&0x8000)
			doasnimm(&endsign, 0x0000);
		else
			doasnimm(&endsign, 0x8000);
	}
	else
		doasn(&actual1, factor1);
mulloop:
	if (actual1&bitshifter)
		calladd32(&mulbuff1, &mulbuff0, &factor0rotate, &actual0);
	do32mul2(&factor0rotate, &actual0, &factor0rotate, &actual0);
	domul2(&bitshifter, &bitshifter);
	if (!(bitshifter&0x8000))
		goto mulloop;
	do32ror12(&mulbuff1, &mulbuff0, &mulbuff1, &mulbuff0);
	if (endsign&0x8000)
		calltwocomp(&mulbuff0, &mulbuff0);
	*prod = mulbuff0;
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
	unsigned short zr, zi, cr, ci, zrs, zis, zrn, zin; //ones place is at (1<<12)
	unsigned short i; //ones place is at (1<<0)
	unsigned short jmpbuff;

	lcd_init();
	lcd_beginwrite();

	doasnimm(&cr, 0x1000);
row:
	doasnimm(&ci, 0xe980);
column:
	doasnimm(&i, 0x0000);
	doasnimm(&zr, 0x0000);
	doasnimm(&zi, 0x0000);
iterate:
	callmultiply(&zrs, &zr, &zr);
	callmultiply(&zis, &zi, &zi);
	domul2(&zi, &zi);
	callmultiply(&zi, &zi, &zr);
	calladd(&zi, &zi, &ci);
	calltwocomp(&zis, &zis);
	calladd(&zr, &zrs, &zis);
	calladd(&zr, &zr, &cr);
	calltwocomp(&zrn, &zr);
	calltwocomp(&zin, &zi);
	doasnimm(&jmpbuff, 0x0001);
	calladd(&i, &i, &jmpbuff); //putting this here instead of after checking zr
	if (((zi&0x6000)&&!(zi&0x8000)) || (((zin)&0x6000&&!((zin)&0x8000))) ||
	    ((zr&0x6000)&&!(zr&0x8000)) || (((zrn)&0x6000&&!((zrn)&0x8000))))
		goto iterate_end;
	if (!(i&0xffe0))
		goto iterate;
iterate_end:
	if (!(i&0xfffc))
		putpixel(YEL);
	else if (!(i&0xfff8))
		putpixel(GRN);
	else if (!(i&0xfff0))
		putpixel(CYA);
	else if (!(i&0xffe0))
		putpixel(BLU);
	else
		putpixel(BLK);
	doasnimm(&jmpbuff, 0x0030);
	calladd(&ci, &ci, &jmpbuff);
	doasnimm(&jmpbuff, 0xe980);
	calladd(&jmpbuff, &ci, &jmpbuff);
	if (jmpbuff&0x8000)
		goto column;
	doasnimm(&jmpbuff, 0xffd0);
	calladd(&cr, &cr, &jmpbuff);
	doasnimm(&jmpbuff, 0x2c00);
	calladd(&jmpbuff, &cr, &jmpbuff);
	if (!(jmpbuff&0x8000))
		goto row;

	lcd_endwrite();
end:
	1; //would be goto end in the map25.2
}
