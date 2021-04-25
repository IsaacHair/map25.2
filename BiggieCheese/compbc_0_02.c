#include <stdlib.h>
#include <stdio.h>

//line types
#define MAIN 0
#define 
struct progline {
	char line[1000];
	int type;
	int depth;
};

void main(int argc, char** argv) {
	if (argc != 3) {
		printf("need <source> <target>\n");
		exit(0x01);
	}
	FILE* source = fopen(argv[1], "r");
	FILE* target = fopen(argv[2], "w");
	struct progline *wholeprogram;
	naiveparse(source, wholeprogram);
	addmacros(target);
	bastardize(wholeprogram, target);
}

void naiveparse(FILE* source, struct progline *wholeprogram) {
	//Just remove comments, blank lines
	//and separate out the ::'s and ;'s into
	//separate lines if after the for keyword
	//or after no keyword.
	//Also allocate wholeprogram.
	
}

void addmacros(FILE* target) {
	//Self-explanatory, just adds all those basic
	//macros like genpred16
	//to the bastardized c output.
}

void bastardize(struct progline *wholeprogram, FILE* target) {
	//Convert each line into bastardized C.
	//This means making the proper structures,
	//macros, functions, etc.
	//It will have to deal with for keyword and
	//with managing global OR local heap keyword
	//uses.
	//It will also need to decide which macros are
	//actually allocated in the final result.
}
