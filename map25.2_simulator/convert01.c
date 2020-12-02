//Expects good formatting for the input file
//Simple state machine
//All states grab the next character for the next state
//This design might create stack overflow issues.

#include <stdlib.h>
#include <stdio.h>

FILE* source;
FILE* target;
int page;
int data;
int line;
int chip;
char program[655360];
int c;

void nextchipindicator() {
	chip++;

	c = fgetc(source);
	switch(c) {
		case EOF:
			break;
		default:
			pageaddress();
			break;
	}
}

void nextpageindicator() {
	c = fgetc(source);
	switch(c) {
		case 'z':
			nextchipindicator();
			break;
		default:
			pageaddress();
			break;
	}
}

void lineanddata() {
	int shift, i;

	line = 0;
	for (shift = 1<<4; shift; shift = shift>>4) {
		if (c >= '0' && c <= '9')
			line |= (c-'0')*shift;
		else
			line |= (c-'a'+10)*shift;
		c = fgetc(source);
	}
	for (i = 0; i < 2; i++) {
		program[((page&0xff80)|(line&0x007f))*10+8-(2*chip)+i] = c;
		c = fgetc(source);
	}

	switch (c) {
		case 'x':
			nextpageindicator();
			break;
		default:
			lineanddata();
			break;
	}
}

void pageaddress() {
	int shift;

	page = 0x0000;
	for (shift = 1<<12; shift; shift = shift>>4) {
		if (c >= '0' && c <= '9')
			page |= (c-'0')*shift;
		else
			page |= (c-'a'+10)*shift;
		c = fgetc(source);
	}

	lineanddata();
}

void main(int argc, char** argv) {
	if (argc != 3) {
		printf("need <source> <target>");
		exit (0x01);
	}
	int i;

	source = fopen(argv[1], "r");
	target = fopen(argv[2], "w");
	chip = 0;

	for (i = 0; i < 655360; i++)
		program[i] = 'f';

	c = fgetc(source);
	pageaddress();

	for (i = 0; i < 655360; i++)
		fprintf(target, "%c", program[i]);

	fclose(source);
	fclose(target);
}
