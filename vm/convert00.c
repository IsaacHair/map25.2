#include <stdlib.h>
#include <stdio.h>

//mode definitions
#define PAGE 0
#define LINE 1
#define DATA 2
#define CHIP 3

void main(int argc, char** argv) {
	//expects good formatting on input file
	if (argc != 3) {
		printf("need <source> <target>\n");
		exit(0x01);
	}
	FILE* source = fopen(argv[1], "r");
	FILE* target = fopen(argv[2], "w");
	int page = 0x0000;
	int line = 0x00;
	int deltaclocks = 0;
	int mode = PAGE;
	int chip = 0;
	int shift = 8;
	char c;
	int i;

	for (i = 0; i < 655360; i++)
		fprintf(target, "f");
	fseek(target, 0, SEEK_SET);

	c = fgetc(source);
	while (1) {
		if (c == 'x')
			mode = PAGE;
		else if (c == 'z')
			mode = CHIP;
		switch (mode) {
			case CHIP:
				chip++;
				shift -= 2;
				mode = PAGE;
				if (chip > 4)
					exit(0x00);
				c = fgetc(source);
				break;
			case PAGE:
				for (page = 0, i = 0; i < 4;
				     i++, page = (i < 3 ? page*16 : page)) {
					c = fgetc(source);
					if (c == 
				break;
			case LINE:
				break;
			case DATA:
				break;
		}
	}

	fclose(source);
	fclose(target);
}
