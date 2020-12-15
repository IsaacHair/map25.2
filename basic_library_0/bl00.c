#include <stdlib.h>
#include <stdio.h>

unsigned short addr;
FILE* fd;

void inst(char*op) {
	fprintf(fd, "%04x %s \x88%04x\n", addr, op, addr+1);
	addr++;
}

void instval(char*op, unsigned short val) {
	fprintf(fd, "%04x %s %04x \x88%04x\n", addr, op, val, addr+1);
	addr++;
}

void instnxt(char*op, unsigned short nxt) {
	fprintf(fd, "%04x %s \x88%04x\n", addr, op, nxt);
	addr++;
}

void instvalnxt(char*op, unsigned short val, unsigned short nxt) {
	fprintf(fd, "%04x %s %04x \x88%04x\n", addr, op, val, nxt);
	addr++;
}

void instexpnxt(char*op, char*nxt) {
	fprintf(fd, "%04x %s \x88%s\n", addr, op, nxt);
	addr++;
}

void instvalexpnxt(char*op, unsigned short val, char*nxt) {
	fprintf(fd, "%04x %s %04x \x88%s\n", addr, op, val, nxt);
	addr++;
}

void replacex88(char* old, char* new) {
	//leaves \x88 identifier
	int c;
	char str[4];
	int copy;
	fseek(fd, 0, SEEK_SET);
	for (c = fgetc(fd), copy = -1; c != EOF; c = fgetc(fd)) {
		if (c == 0x88) {
			copy = 0;
			continue;
		}
		if (copy > -1) {
			str[copy] = c;
			copy++;
		}
		if (copy == 4) {
			copy = -1;
			if (str[0] == old[0] && str[1] == old[1]
			    && str[2] == old[2] && str[3] == old[3]) {
				fseek(fd, -4, SEEK_CUR);
				fwrite(new, sizeof(char), 4, fd);
			}
		}
	}
}

void makeaddrodd() {
	//works at 0x0000, but the lack of instruction at 0x0000 means
	//the assembly code won't even run (as map25.2 resets to 0x0000)
	char old[5];
	char new[5];
	if (addr%2 == 0) {
		sprintf(old, "%04x", addr);
		sprintf(new, "%04x", addr+1);
		replacex88(old, new);
		addr++;
	}
}

void makeaddreven() {
	char old[5];
	char new[5];
	if (addr%2 == 1) {
		sprintf(old, "%04x", addr);
		sprintf(new, "%04x", addr+1);
		replacex88(old, new);
		addr++;
	}
}

void removex88(char* filepath) {
	FILE* newfd = fopen(filepath, "w");
	int c;
	fseek(fd, 0, SEEK_SET);
	for (c = fgetc(fd); c != EOF; c = fgetc(fd))
		if (c != 0x88)
			fprintf(newfd, "%c", c);
}

void addrpred16() {
	unsigned short mask, addrshift;
	int i;
	
	makeaddrodd();
	for (i = 16; i >= 2; i = i>>1)
		for (mask = 0xffff>>(i/2), addrshift = 16/i;
		     mask; mask = mask>>i, addrshift++)
			instvalnxt("addr jzor", mask, addr+addrshift);
	instvalnxt("addr jzor", 0xffff, addr+16);
	for (mask = 0x3fff; mask; mask = mask>>1)
		instvalnxt("imm addr1", mask, addr+18);
	instvalnxt("imm addr0", 0x0001, addr+18);
	instvalnxt("imm addr1", 0xffff, addr+17);
	instvalnxt("imm addr1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 15; mask >= 0x0002; mask = mask>>1, i--)
		instvalnxt("imm addr0", mask, addr+i);
}

void makeaddr_addrpred16() {
	makeaddrodd();
}

void genpred16() {
	unsigned short mask, addrshift;
	int i;
	
	makeaddrodd();
	for (i = 16; i >= 2; i = i>>1)
		for (mask = 0xffff>>(i/2), addrshift = 16/i;
		     mask; mask = mask>>i, addrshift++)
			instvalnxt("gen jzor", mask, addr+addrshift);
	instvalnxt("gen jzor", 0xffff, addr+16);
	for (mask = 0x3fff; mask; mask = mask>>1)
		instvalnxt("imm gen1", mask, addr+18);
	instvalnxt("imm gen0", 0x0001, addr+18);
	instvalnxt("imm gen1", 0xffff, addr+17);
	instvalnxt("imm gen1", 0x7fff, addr+1);
	for (mask = 0x8000, i = 15; mask >= 0x0002; mask = mask>>1, i--)
		instvalnxt("imm gen0", mask, addr+i);
}

void makeaddr_genpred16() {
	makeaddrodd();
}

void addgenram() {
	//adds values in ram and gen and puts answer in gen
	//requires average of 40.5 clocks
	//high efficiency rom packing (not perfect though)
	unsigned short mask;
	char str[5];

	//do the addition
	makeaddrodd();
	instexpnxt("ram jzor 0001", "N_NC");
	for (mask = 0x0001; mask; mask = mask<<1) {
		replacex88("N_NC", "__NC");
		replacex88("N__C", "___C");
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("__NC", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalnxt("imm gen1", mask, addr+2);
			instvalnxt("imm gen0", mask, addr+2);
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
		}
		makeaddreven();
		sprintf(str, "%04x", addr);
		replacex88("___C", str);
		instvalnxt("gen jzor", mask, addr+2);
		instvalnxt("gen jzor", mask, addr+3);
		if (mask != 0x8000) {
			instvalnxt("imm gen1", mask, addr+4);
			instvalnxt("imm gen0", mask, addr+4);
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
			instvalexpnxt("ram jzor", mask<<1, "N_NC");
			instvalexpnxt("ram jzor", mask<<1, "N__C");
		}
		else {
			instvalexpnxt("imm gen1", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen0", mask, "DONE");
			instvalexpnxt("imm gen1", mask, "DONE");
		}
	}
	sprintf(str, "%04x", addr);
	replacex88("DONE", str);
}

void makeaddr_addgenram() {
	makeaddrodd();
}

void main(int argc, char** argv) {
	//this is just a quick test program
	if (argc != 3) {
		printf("need buffer, target\n");
		exit(0x01);
	}
	addr = 0;
	fd = fopen(argv[2], "w+");
	unsigned short loopstartaddr;
	char str0[5];
	char str1[5];

	inst("dnc noop 0000");
	inst("dnc noop 0000");
	addrpred16();
	genpred16();
	inst("imm ramall 0001");
	makeaddr_addgenram();
	sprintf(str0, "%04x", addr);
	addgenram();
	sprintf(str1, "%04x", addr);
	replacex88(str1, str0);

	removex88(argv[1]);
}
