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
	makeheap(&MAIN_dummy);
	heapcount++;
	makeheap(&MAIN_ddx);
	heapcount++;
	inst("imm dir0 ffff");

	//init values
	set32immimm(MAIN_ddx, 0x14200000);
	//loop to move ship
	loopaddr = addr;

	//set MAIN_ddx
	//start with gravity
	//accelerate up (positive)
	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0x00000815);
	add32(MAIN_ddx, MAIN_ddx, MAIN_dummy);
	replacex88expimm(_next, addr);
	//accelerate down (negative)
	keygen();
	makeaddrodd();
	inst("gen jzor 0100");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0xffffee77);
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
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy);
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
	instval("imm addr1", MAIN_dummy|0x0001);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);

	set32immimm(MAIN_dummy, 0xffffee77);
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy);
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
	instval("imm addr1", MAIN_dummy|0x0001);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);

	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy);
	inst("imm ramall 6969");
	inst("imm addr1 0001");
	inst("imm ramall 4242");
	inst("imm gen0 ffff");
	inst("imm gen1 ffff");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_dummy);
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
	instval("imm addr1", MAIN_dummy|0x0001);
	inst("imm out0 ffff");
	inst("ram out1 0000");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	
	//loop infinitely
	replacex88immimm(addr, loopaddr);

	addcode();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);
}
