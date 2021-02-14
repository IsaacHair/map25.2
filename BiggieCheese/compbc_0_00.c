#include <stdlib.h>
#include <stdio.h>

struct line {
	char* content;
	struct line *nested;
	struct line *next;
}

void main(int argc, char** argv) {
	//note that everything is de-allocated when the program stops,
	//so don't have to be anal about garbage collection
	
	struct line program;
	FILE* source;
	if (argc != 2) {
		printf("need source file\n");
		exit(0x01);
	}
	ramparse (source, *program);
}

//this bad boy just does some very basic opening etc
void ramparse(FILE* source, struct line *program) {}
