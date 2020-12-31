#include <stdlib.h>
#include <stdio.h>
#include <bl_map25.2.h>

unsigned short MAIN_seed;
unsigned short MAIN_rand;
unsigned short MAIN_flags;
unsigned short MAIN_count;

void main(int argc, char** argv) {
	unsigned short startaddr;
	unsigned short flagrstaddr;
	unsigned short loopaddr;
	if (argc != 3) {
		printf("need <target> <buffer>\n");
		exit(0x01);
	}
	addr = 0;
	labelcount = 0;
	heapcount = 0;
	fd = fopen(argv[2], "w+");
	makemfplabel();
	libheap();
	makeheap(&MAIN_seed);
	makeheap(&MAIN_rand);
	makeheap(&MAIN_count);
	while (heapcount&0x00ff)
		heapcount++;
	makeheap(&MAIN_flags);
	while (heapcount&0x00ff)
		heapcount++;
	inst("imm dir0 ffff");
	lcdinit();
	lcdboxgreyimm(42, 0, 239, 0, 319);
	
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
	//test flag escape
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_rand);
	inst("imm gen0 ffff");
	inst("ram gen1 2422");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_flags);
	inst("imm gen0 ff00");
	inst("gen addr1 4555");
	makeaddrodd();
	inst("addr jzor 0001");
	instnxt("dnc noop 1234", addr+2);
	instnxt("dnc noop 
	//set corresponding flag to "1"
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_rand);
	inst("imm gen0 ffff");
	inst("ram gen1 2222");
	inst("imm addr0 ffff");
	instval("imm addr1", MAIN_flags);
	inst("imm gen0 ff00");
	inst("gen addr1 4545");
	instnxt("imm ramall 0001", loopaddr);

	mfpcode();
	replacemfpcall();
	removex88(argv[1]);
	fclose(fd);
}
