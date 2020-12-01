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
	int mode = PAGE;
	int chip = 0;
	int shift = 8;
	char program[655360];
	int page;
	int line;
	int data;
	int c;
	int i;

	for (i = 0; i < 655360; i++)
		program[i] = 'f';

	c = fgetc(source);
	while (c != EOF) {
		if (c == 'x')
			mode = PAGE;
		else if (c == 'z')
			mode = CHIP;
		switch (mode) {
			case CHIP:
				chip++;
				shift -= 2;
				mode = PAGE;
				c = fgetc(source);
				if (c == 4)
					break;
				//dont break
			case PAGE:
				c = fgetc(source);
				if (c == 'z')
					break;
				for (page = 0, i = 0; i < 4;
				     i++, page = (i < 4 ? page*16 : page)) {
					if (c >= 'a' && c <= 'f')
						page += c-'a'+10;
					else
						page += c-'0';
					c = fgetc(source);
				}
				mode = LINE;
				break;
			case LINE:
				for (line = 0, i = 0; i < 2;
				     i++, line = (i < 2 ? line*16 : line)) {
					if (c >= 'a' && c <= 'f')
						line += c-'a'+10;
					else
						line += c-'0';
					c = fgetc(source);
				}
				mode = DATA;
				break;
			case DATA:
				program[((page&0xff80)|line)*10+shift] = c;
				c = fgetc(source);
				program[((page&0xff80)|line)*10+shift+1] = c;
				c = fgetc(source);
				mode = LINE;
				break;
		}
	}

	for (i = 0; i < 655360; i++)
		fprintf(target, "%c", program[i]);

	fclose(source);
	fclose(target);
}
