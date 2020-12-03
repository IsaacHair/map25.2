#include <stdlib.h>
#include <stdio.h>

unsigned short curr;
unsigned short nxt;
unsigned short addr;
unsigned short gen;
unsigned short out;
unsigned short dir;
unsigned short bus;
unsigned short ram[65536];
unsigned short rom[655360];
unsigned short inst[3]; //0 is op code (lower 8), 1 is imm, 2 is nxt
long deltaclocks;
int sampleclocks;

void initscreen() {
	printf("\n\n\n\n\n\n\n\n\n\n\n");
}

void printregisters() {
	printf("\033[11A");
	printf("HEX VALUES:\n");
	printf("curr:%04x\nnxt:%04x\n", curr, nxt);
	printf("addr:%04x\ngen:%04x\nout:%04x\ndir:%04x\n", addr, gen, out, dir);
	printf("inst:%02x %04x %04x\n", inst[0], inst[1], inst[2]);
	printf("bus:%04x\n", bus);
	printf("DECIMAL VALUES:\n");
	printf("deltaclocks:%d\n", deltaclocks);
}

void fetchandbus() {
	int c, shift, rompointer;
	curr = nxt;
	rompointer = curr*10;
	inst[0] = 0x0000;
	inst[1] = 0x0000;
	inst[2] = 0x0000;
	for (shift = (1<<4); shift; shift = shift>>4) {
		c = rom[rompointer];
		rompointer++;
		if (c >= '0' && c <= '9')
			inst[0] |= (c-'0')*shift;
		else
			inst[0] |= (c-'a'+10)*shift;
	}
	for (shift = (1<<12); shift; shift = shift>>4) {
		c = rom[rompointer];
		rompointer++;
		if (c >= '0' && c <= '9')
			inst[1] |= (c-'0')*shift;
		else
			inst[1] |= (c-'a'+10)*shift;
	}
	for (shift = (1<<12); shift; shift = shift>>4) {
		c = rom[rompointer];
		rompointer++;
		if (c >= '0' && c <= '9')
			inst[2] |= (c-'0')*shift;
		else
			inst[2] |= (c-'a'+10)*shift;
	}

	switch ((inst[0]/16)%16) {
		case 0:
			bus = inst[1];
			break;
		case 1:
			bus = addr;
			break;
		case 2:
			bus = gen;
			break;
		case 3:
			bus = gen<<1;
			if (gen&0x8000)
				bus |= 0x0001;
			else
				bus &= ~0x0001;
			break;
		case 4:
			bus = gen>>1;
			if (gen&0x0001)
				bus |= 0x8000;
			else
				bus &= ~0x8000;
			break;
		case 5:
			bus = out;
			break;
		case 6:
			bus = dir;
			break;
		case 7:
			printf("not set up for input\n");
			exit(0x01);
			break;
		case 8:
			bus = ram[addr];
			break;
		default:
			bus = 0xffff;
			break;
	}
}

void executeandnxt() {
	switch(inst[0]%16) {
		case 0:
			nxt = inst[2]&0xfffe;
			if (bus&inst[1])
				nxt |= 0x0001;
			else
				nxt &= ~0x0001;
			break;
		case 1:
			nxt = bus;
			break;
		case 2:
			nxt = inst[2];
			out &= ~bus;
			break;
		case 3:
			nxt = inst[2];
			out |= bus;
			break;
		case 4:
			nxt = inst[2];
			addr &= ~bus;
			break;
		case 5:
			nxt = inst[2];
			addr |= bus;
			break;
		case 6:
			nxt = inst[2];
			dir &= ~bus;
			break;
		case 7:
			nxt = inst[2];
			dir |= bus;
			break;
		case 8:
			nxt = inst[2];
			gen &= ~bus;
			break;
		case 9:
			nxt = inst[2];
			gen |= bus;
			break;
		case 0xd:
			nxt = inst[2];
			ram[addr] = (ram[addr]&0xff00)|(bus&0x00ff);
			break;
		case 0xe:
			nxt = inst[2];
			ram[addr] = (ram[addr]&0x00ff)|(bus&0xff00);
			break;
		case 0xf:
			nxt = inst[2];
			ram[addr] = bus;
			break;
		default:
			nxt = inst[2];
			break;
	}
}

void main(int argc, char** argv) {
	int i;
	if (argc != 2) {
		printf("need <program file>\n");
		exit(0x01);
	}
	FILE* source = fopen(argv[1], "r");
	for (i = 0; i < 655360; i++)
		rom[i] = fgetc(source);
	fclose(source);
	sampleclocks = 0;
	deltaclocks = 0;
	nxt = 0x0000;
	initscreen();
	while (1) {
		//do the simulation
		if (!sampleclocks)
			printregisters();
		fetchandbus();
		executeandnxt();
		deltaclocks++;
		if (!sampleclocks)
			sampleclocks = (rand()%256)*256+(rand()%256);
		else
			sampleclocks--;
		//halt test specific to plot16_copy
		if (curr == 0x032d) {
			printregisters();
			printf("halt address reached\n");
			exit(0x01);
		}
	}
}
