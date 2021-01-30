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
 * (eg pointer|0x0001). In other words, its all little endian.
 *
 * Note that x refers to side-to-side (real on complex plain)
 * and y refers to up-and-down (imaginary on complex plain).
 */

unsigned short MAIN_y;
unsigned short MAIN_x;
unsigned short MAIN_dy;
unsigned short MAIN_dx;
unsigned short MAIN_ddy;
unsigned short MAIN_ddx;
unsigned short MAIN_dummy;
unsigned short MAIN_yold;
unsigned short MAIN_xold;
unsigned short MAIN_yend;
unsigned short MAIN_xend;
unsigned short MAIN_yendold;
unsigned short MAIN_xendold;

void lcdspritefixedpoint_dwn6(unsigned short scpoint, unsigned short ecpoint,
			      unsigned short sppoint, unsigned short eppoint) {
	//note that this must be of size 5x5 because of the jank ass setup
	//PLEASE make this entire library setup cleaner before getting too much futher
	//this function is just a bit basher basically
	//
	//This funciton will also do a quick check to see if vertical acceleration is on;
	//if so, red pixels will be added in for the "rocket engine"
	
	char _red[5];
	char _end[5];
	makelabel(_red);
	makelabel(_end);

	lcdpauseframe();
	lcdsizeframepoint_dwn6(scpoint, ecpoint, sppoint, eppoint);
	lcdresetframe();

	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instnxt("dnc noop 0000", addr+2);
	instexpnxt("dnc noop 0000", _red);

	whitepx();
	blackpx();
	whitepx();
	whitepx();
	blackpx();

	blackpx();
	whitepx();
	blackpx();
	blackpx();
	whitepx();
	
	blackpx();
	blackpx();
	blackpx();
	blackpx();
	whitepx();

	blackpx();
	whitepx();
	blackpx();
	blackpx();
	whitepx();

	whitepx();
	blackpx();
	whitepx();
	whitepx();
	blackpx();

	instexpnxt("dnc noop 8008", _end);

	replacex88expimm(_red, addr);
	
	whitepx();
	blackpx();
	whitepx();
	whitepx();
	blackpx();

	blackpx();
	whitepx();
	redpx();
	redpx();
	whitepx();
	
	redpx();
	redpx();
	redpx();
	redpx();
	whitepx();

	blackpx();
	whitepx();
	redpx();
	redpx();
	whitepx();

	whitepx();
	blackpx();
	whitepx();
	whitepx();
	blackpx();

	replacex88expimm(_end, addr);

	lcdpauseframe();
}

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
	makeheap(&MAIN_y);
	heapcount++;
	makeheap(&MAIN_x);
	heapcount++;
	makeheap(&MAIN_dy);
	heapcount++;
	makeheap(&MAIN_dx);
	heapcount++;
	makeheap(&MAIN_ddy);
	heapcount++;
	makeheap(&MAIN_ddx);
	heapcount++;
	makeheap(&MAIN_dummy);
	heapcount++;
	makeheap(&MAIN_yold);
	heapcount++;
	makeheap(&MAIN_xold);
	heapcount++;
	makeheap(&MAIN_yend);
	heapcount++;
	makeheap(&MAIN_xend);
	heapcount++;
	makeheap(&MAIN_yendold);
	heapcount++;
	makeheap(&MAIN_xendold);
	heapcount++;
	inst("imm dir0 ffff");
	lcdinit();

	//init values
	set32immimm(MAIN_y, 0x14200000);
	set32immimm(MAIN_x, 0x16900000);
	set32immimm(MAIN_dy, 0x00000000);
	set32immimm(MAIN_dx, 0x00000000);
	set32immimm(MAIN_ddy, 0x00000000);
	set32immimm(MAIN_ddx, 0x00000000);
	//init screen
	lcdboxgreyimm(0,  0, 239, 0, 319);
	//loop to move ship
	loopaddr = addr;
	//delay
	inst("imm gen0 ffff");
	inst("imm gen1 0200");
	makeaddr_genpred16();
	delayaddr = addr;
	genpred16();
	makeaddrodd();
	inst("gen jzor ffff");
	instnxt("dnc noop 0000", addr+2);
	instnxt("dnc noop 0000", delayaddr);
	//log old values
	trans32immimm(MAIN_yold, MAIN_y);
	trans32immimm(MAIN_xold, MAIN_x);
	trans32immimm(MAIN_yendold, MAIN_yend);
	trans32immimm(MAIN_xendold, MAIN_xend);
	//set MAIN_ddy
	//start with gravity
	set32immimm(MAIN_ddy, 0xfffffb00);
	//accelerate up
	keygen();
	makeaddrodd();
	inst("gen jzor 0200");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0x00001215);
	add32(MAIN_ddy, MAIN_ddy, MAIN_dummy);
	replacex88expimm(_next, addr);
	//accelerate down
	keygen();
	makeaddrodd();
	inst("gen jzor 0100");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0xfffff4eb);
	add32(MAIN_ddy, MAIN_ddy, MAIN_dummy);
	replacex88expimm(_next, addr);
	//set MAIN_ddx
	//start with zero
	set32immimm(MAIN_ddx, 0x00000000);
	//accelerate right (negative)
	keygen();
	makeaddrodd();
	inst("gen jzor 2000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0xfffff8dd);
	add32(MAIN_ddx, MAIN_ddx, MAIN_dummy);
	replacex88expimm(_next, addr);
	//accelerate left (positive)
	keygen();
	makeaddrodd();
	inst("gen jzor 0020");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, 0x00000723);
	add32(MAIN_ddx, MAIN_ddx, MAIN_dummy);
	replacex88expimm(_next, addr);
	//do the calculations
	//integrate the acceleration
	add32(MAIN_dy, MAIN_ddy, MAIN_dy);
	add32(MAIN_dx, MAIN_ddx, MAIN_dx);
	add32(MAIN_y, MAIN_dy, MAIN_y);
	add32(MAIN_x, MAIN_dx, MAIN_x);
	//ensure in range
	//handle MAIN_y
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_y|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, (((240-(5-1))<<6))<<16);
	add32(MAIN_y, MAIN_dummy, MAIN_y);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	set32immimm(MAIN_dummy, (0x10000-((240-(5-1))<<6))<<16);
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
	//handle MAIN_x
	checkloopaddr = addr;
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_x|0x0001);
	makeaddrodd();
	inst("ram jzor 8000");
	instexpnxt("dnc noop 0000", _next);
	set32immimm(MAIN_dummy, (((320-(5-1))<<6))<<16);
	add32(MAIN_x, MAIN_dummy, MAIN_x);
	replacex88immimm(addr, checkloopaddr);
	replacex88expimm(_next, addr);
	checkloopaddr = addr;
	set32immimm(MAIN_dummy, (0x10000-((320-(5-1))<<6))<<16);
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
	//render
	set32immimm(MAIN_yend, ((5-1)*(1<<6))<<16);
	set32immimm(MAIN_xend, ((5-1)*(1<<6))<<16);
	add32(MAIN_yend, MAIN_yend, MAIN_y);
	add32(MAIN_xend, MAIN_xend, MAIN_x);
	lcdboxgreypointimm_dwn6(0,  MAIN_yold|0x0001, MAIN_yendold|0x0001,
				MAIN_xold|0x0001, MAIN_xendold|0x0001, 25);
	lcdspritefixedpoint_dwn6(MAIN_y|0x0001, MAIN_yend|0x0001, MAIN_x|0x0001, MAIN_xend|0x0001);
	inst("dnc noop 0000");
	//loop infinitely
	replacex88immimm(addr, loopaddr);

	addcode();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);
}
