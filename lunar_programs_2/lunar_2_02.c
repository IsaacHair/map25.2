#include <stdlib.h>
#include <stdio.h>
#include <bl_map25.2.h>

/*
 * Creates a version of the classic lunar lander game.
 * Note: As usual, function variables are pointed to by
 * immediate pointers and are not on a stack.
 *
 * Arithmatic uses 32 bits where the pointer to the data
 * is even, the lower data word is at the pointer, and
 * the upper data word is at the odd version of the pointer
 * (eg pointer|0x0001).
 */

unsigned short MAIN_x;
unsigned short MAIN_y;
unsigned short MAIN_dx;
unsigned short MAIN_dy;
unsigned short MAIN_ddx;
unsigned short MAIN_ddy;
unsigned short MAIN_dummy;
unsigned short MAIN_xold;
unsigned short MAIN_yold;
unsigned short MAIN_xend;
unsigned short MAIN_yend;
unsigned short MAIN_xendold;
unsigned short MAIN_yendold;

void main(int argc, char** argv) {
	//note that column index is X and page index is Y
	int loopaddr, delayaddr, checkloopaddr;
	char _next[5];
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	labelcount = 0;
	heapcount = 0;
	fd = fopen(argv[2], "w+");
	makeaddlabel();
	makelabel(_next);
	libheap();
	if (heapcount&0x0001)
		heapcount++;
	makeheap(&MAIN_x);
	heapcount++;
	makeheap(&MAIN_y);
	heapcount++;
	makeheap(&MAIN_dx);
	heapcount++;
	makeheap(&MAIN_dy);
	heapcount++;
	makeheap(&MAIN_ddx);
	heapcount++;
	makeheap(&MAIN_ddy);
	heapcount++;
	makeheap(&MAIN_dummy);
	heapcount++;
	makeheap(&MAIN_xold);
	heapcount++;
	makeheap(&MAIN_yold);
	heapcount++;
	makeheap(&MAIN_xend);
	heapcount++;
	makeheap(&MAIN_yend);
	heapcount++;
	makeheap(&MAIN_xendold);
	heapcount++;
	makeheap(&MAIN_yendold);
	heapcount++;
	inst("imm dir0 ffff");

	//init values
	set32immimm(MAIN_x, 0x14200000);
	set32immimm(MAIN_y, 0x16900000);
	set32immimm(MAIN_dx, 0x00000000);
	set32immimm(MAIN_dy, 0x00000000);
	set32immimm(MAIN_ddx, 0x00000000);
	set32immimm(MAIN_ddy, 0x00000000);
	//loop to move ship
	loopaddr = addr;
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 0500");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//log old values
	trans32immimm(MAIN_xold, MAIN_x);
	trans32immimm(MAIN_yold, MAIN_y);
	trans32immimm(MAIN_xendold, MAIN_xend);
	trans32immimm(MAIN_yendold, MAIN_yend);

	//set MAIN_ddx
	//start with gravity
	//accelerate up (positive)
	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0x00000115);
	add32(MAIN_ddx, MAIN_ddx, MAIN_dummy);
	replacex88expimm(_next, addr);
	//accelerate down (negative)
	keygen();
	makeaddrodd();
	inst("gen jzor 0100");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0xfffffeeb);
	add32(MAIN_ddx, MAIN_ddx, MAIN_dummy);
	replacex88expimm(_next, addr);
	//delay and print
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ddx);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ddx|0x0001);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	
	//set MAIN_ddy
	//start with zero
	//accelerate right (negative)
	keygen();
	makeaddrodd();
	inst("gen jzor 2000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0xfffffdeb);
	add32(MAIN_ddy, MAIN_ddy, MAIN_dummy);
	replacex88expimm(_next, addr);
	//accelerate left (positive)
	keygen();
	makeaddrodd();
	inst("gen jzor 0020");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0x00000215);
	add32(MAIN_ddy, MAIN_ddy, MAIN_dummy);
	replacex88expimm(_next, addr);
	//delay and print
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ddy);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_ddy|0x0001);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);

	//do the calculations
	//integrate the acceleration
	add32(MAIN_dx, MAIN_ddx, MAIN_dx);
	add32(MAIN_dy, MAIN_ddy, MAIN_dy);
	add32(MAIN_x, MAIN_dx, MAIN_x);
	add32(MAIN_y, MAIN_dy, MAIN_y);
	//ensure in range
	//handle MAIN_x
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_x|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, (((240-(5-1))<<6))<<16);
	add32(MAIN_x, MAIN_dummy, MAIN_x);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	set32immimm(MAIN_dummy, (0x10000-((240-(5-1))<<6))<<16);
	add32(MAIN_dummy, MAIN_dummy, MAIN_x);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", _next);
	trans32immimm(MAIN_x, MAIN_dummy);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	//handle MAIN_y
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_y|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, (((320-(5-1))<<6))<<16);
	add32(MAIN_y, MAIN_dummy, MAIN_y);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	set32immimm(MAIN_dummy, (0x10000-((320-(5-1))<<6))<<16);
	add32(MAIN_dummy, MAIN_dummy, MAIN_y);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", _next);
	trans32immimm(MAIN_y, MAIN_dummy);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	//render
	set32immimm(MAIN_xend, ((5-1)*(1<<6))<<16);
	set32immimm(MAIN_yend, ((5-1)*(1<<6))<<16);
	add32(MAIN_xend, MAIN_xend, MAIN_x);
	add32(MAIN_yend, MAIN_yend, MAIN_y);
	inst("dnc noop 0000");
	//loop infinitely
	replacex88immimm(addr, loopaddr);

	addcode();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);
}
