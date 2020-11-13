#include <stdio.h>
#include <stdlib.h>

/*
 * This will simply display some test multiplications to the LED displays.
 * Addr and out will display the factors and dir will display the product.
 * The upper bit is the sign, 3 after that are int part, 12 after are fraction.
 * Gen is used to to count up and delay the output so that the test
 * numbers are displayed for like 1 second before being changed to the next
 * pair.
 *
 * Using jump multiply algorithm.
 */

void ramtrans(unsigned short varpoint, unsigned short valpoint) {
	//7 instructions
	immramaddr(valpoint);
	immgen0(0xffff);
	ramgen1();
	immramaddr(varpoint);
	genram();
}

void multiply(unsigned short f0pointer, unsigned short f1pointer, unsigned short prodpointer) {
	//transfer values
	ramtrans(MUL_F0, f0pointer);
	ramtrans(MUL_F1, f1pointer);
	//record sign as highest bit in MUL_PROD
	inst("imm addr0 ffff");
	instval("imm addr1", MUL_F0);
	instaddr("ram jzor 0x8000", addr%2 ? addr+1 : addr+2);
	instaddr("imm addr0 ffff", addr+2);
	instaddr("imm addr0 ffff", addr+);
	instvaladdr("imm addr1", MUL_F1, addr+2);
	instaddr("ram jzor 0x8000", addr+1);
	instaddr("imm addr0 ffff", addr+2);
	instaddr("imm addr0 ffff", addr+2);
	instval("imm addr1", MUL_PROD);
	//record result
}
