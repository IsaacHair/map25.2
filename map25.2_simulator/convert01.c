#include <stdlib.h>
#include <stdio.h>

void main(int argc, char** argv) {
	if (argc != 3) {
		printf("need <source> <target>");
		exit (0x01);
	}
	FILE* source;
	FILE* target;
	char program[655360];
	int i;

	source = fopen(argv[1], "r");
	target = fopen(argv[2], "w");

	for (i = 0; i < 655360; i++)
		program[i] = 'f';

	for (

	for (i = 0; i < 655360; i++)
		fprintf(target, "%c", program[i]);

	fclose(source);
	fclose(target);
}
