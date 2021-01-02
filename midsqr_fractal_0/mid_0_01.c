#include <stdlib.h>
#include <stdio.h>
#include <bl_map25.2.h>

unsigned short MAIN_seed;
unsigned short MAIN_rand;
unsigned short MAIN_randre;
unsigned short MAIN_randre2;
unsigned short MAIN_randim;
unsigned short MAIN_randim2;
unsigned short MAIN_flags;
unsigned short MAIN_count;

void main(int argc, char** argv) {
	unsigned short startaddr;
	unsigned short flagrstaddr;
	unsigned short loopaddr;
	char _loopiterate[5];
	char _nonadjusted[5];
	int i;
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	labelcount = 0;
	heapcount = 0;
	fd = fopen(argv[2], "w+");
	makemfplabel();
	makeaddlabel();
	libheap();
	makelabel(_loopiterate);
	makelabel(_nonadjusted);
	makeheap(&MAIN_seed);
	makeheap(&MAIN_rand);
	makeheap(&MAIN_randre);
	makeheap(&MAIN_randre2);
	makeheap(&MAIN_randim);
	makeheap(&MAIN_randim2);
	makeheap(&MAIN_count);
	while (heapcount&0x00ff)
		heapcount++;
	makeheap(&MAIN_flags);
	while (heapcount&0x00ff)
		heapcount++;
	inst("imm dir0 ffff");
	lcdinit();
	lcdboxgreyimm(42, 0, 239, 0, 319);
	
	lcdresetframe();
	setimmimm(MAIN_seed, 0xffef);

	startaddr = addr;
	transimmimm(MAIN_rand, MAIN_seed);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_flags|0x00ff);
	flagrstaddr = addr;
	inst("imm ramall 0000");
	addrpred16();
	makeaddrodd();
	inst("addr jzor 00ff");
	instnxt("dnc noop 6942", addr+2);
	instnxt("dnc noop 9999", flagrstaddr);
	setimmimm(MAIN_count, 0x00ff);

	loopaddr = addr;
	//test if flag is already set
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_rand);
	inst("imm gen0 ffff");
	inst("ram gen1 2422");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_flags);
	inst("imm gen0 ff00");
	inst("gen addr1 4555");
	makeaddrodd();
	inst("ram jzor 0001");
	instexpnxt("dnc noop 1234", _loopiterate);
	//see if halt because the number WAS zero
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_seed);
	makeaddrodd();
	inst("ram jzor ffff");
	instnxt("dnc noop 6969", addr); //this is where it halts
	inst("dnc noop 4321");
	//see if need to adjust column before decrementing to get desired results
	//gets a bit messy but should still be evident what is happening
	inst("imm gen0 ffff");
	inst("ram gen1 aaaa");
	makeaddrodd();
	inst("ram jzor 00ff");
	instnxt("dnc noop 6677", addr+2);
	instexpnxt("dnc noop fead", _nonadjusted);
	//this part will subtract from (for example) fa00 to f9ff,
	//then set f9ff to f9f0, then send this number to be decremented
	genpred16();
	inst("imm gen0 000f");
	//decrement
	replacex88expimm(_nonadjusted, addr);
	genpred16();
	inst("gen ramall eeee");
	//print and return to main loop
	buswrite(0);
	buswrite(0);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_count);
	inst("imm gen0 ffff");
	inst("ram gen1 0444");
	//shift up 2 spaces for clearer detail
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	inst("imm addr0 ffff");
	inst("rol addr1 0000");
	inst("imm gen0 ffff");
	inst("addr gen1 0000");
	buswritegen(); //last so you can see it persist on the register
	instnxt("dnc noop 0001", startaddr);
	//decrement counter
	replacex88expimm(_loopiterate, addr);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_count);
	inst("imm gen0 ffff");
	inst("ram gen1 7000");
	genpred16();
	inst("gen ramall 5555");
	//set corresponding flag to "1"
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_rand);
	inst("imm gen0 ffff");
	inst("ram gen1 2222");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_flags);
	inst("imm gen0 ff00");
	inst("gen addr1 4545");
	inst("imm ramall 0001");
	//iterate
	transimmimm(MAIN_randre, MAIN_rand);
	transimmimm(MAIN_randim, MAIN_rand);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("imm ramup 0000");
	inst("imm gen0 ffff");
	inst("ram gen1 bead");
	for (i = 0; i < 8; i++) {
		inst("imm addr0 ffff");
		inst("ror addr1 3838");
		inst("imm gen0 ffff");
		inst("addr gen1 765f");
	}
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("gen ramall 4268");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randre);
	inst("imm ramlow 0000");
	callmfp(MAIN_randre2, MAIN_randre, MAIN_randre);
	callmfp(MAIN_randim2, MAIN_randim, MAIN_randim);
	callmfp(MAIN_randim, MAIN_randim, MAIN_randre);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("imm gen0 ffff");
	inst("ram gen1 5487");
	inst("rol ramall 7777");
	inst("imm ramlow 0000");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim2);
	inst("imm gen1 ffff");
	inst("ram gen0 7433");
	genpred16();
	inst("gen ramall ffff");
	inst("imm gen1 ffff");
	inst("ram gen0 5873");
	inst("gen ramall ffff");
	calladd(MAIN_randre, MAIN_randre2, MAIN_randim2);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("imm ramlow 0000");
	inst("imm gen0 ffff");
	inst("ram gen1 bead");
	for (i = 0; i < 8; i++) {
		inst("imm addr0 ffff");
		inst("ror addr1 3838");
		inst("imm gen0 ffff");
		inst("addr gen1 765f");
	}
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("gen ramall 4268");
	transimmimm(MAIN_rand, MAIN_randre);
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_randim);
	inst("imm gen0 ffff");
	inst("ram gen1 b00b");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_rand);
	inst("gen ramlow b0b5");
	instnxt("dnc noop bbbb", loopaddr);

	mfpcode();
	replacemfpcall();
	addcode();
	replaceaddcall();
	removex88(argv[1]);
	fclose(fd);
}
