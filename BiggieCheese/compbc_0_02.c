#include <stdlib.h>
#include <stdio.h>

//line types
#define MAIN 1
#define END 0
#define START 2
struct progline {
	char line[1000];
	int type;
	int depth;
	struct progline* next;
	struct progline* previous;
};

void naiveparse(FILE* source, struct progline *programhead) {
	//Just remove comments, blank lines
	//and separate out the ::'s and ;'s into
	//separate lines if after the for keyword
	//or after no keyword.
	//Also allocate programhead and do the basic
	//info: the depth and the type.
	//NO \n AT END OF LINE, JUST \0
	//***For now, just copying all lines directly
	int i;
	struct progline* currpos;
	programhead = malloc(sizeof(struct progline));
	programhead->type = START;
	programhead->previous = NULL;
	programhead->line[0] = '\0';
	programhead->depth = 0;
	programhead->next = malloc(sizeof(struct progline));
	currpos = programhead->next;
	currpos->previous = programhead;
	/*char c;
	for (c = fgetc(source); c != EOF; c = fgetc(source)) {
		for (i = 0; c != '\n'; c = fgetc(source), i++)
			currpos->line[i] = c;
		currpos->line[i] = '\0';
		//XXX weird line for testing
		currpos->type = i;
		currpos->depth = i+1;
		currpos->next = malloc(sizeof(struct progline));
		currpos->next->previous = currpos;
		currpos = currpos->next;
	}*/
	currpos->next = NULL;
	currpos->type = END;
	currpos->line[0] = '\0';
	currpos->depth = 0;
}

void bastardize(struct progline *programhead) {
	//Convert each line into bastardized C.
	//This means making the proper structures,
	//macros, functions, etc.
	//It will have to deal with for keyword and
	//with managing global OR local heap keyword
	//uses.
	//It will also need to decide which macros are
	//actually allocated in the final result.
}

void globalheap(struct progline *programhead) {}

void programallocate(struct progline *programhead) {
	//this should also do the #include's at the
	//top of the c file
}

void addmacros(FILE* target) {
	//Self-explanatory, just adds all those basic
	//macros like genpred16
	//to the bastardized c output.
	//It doesn't have to worry about adding only
	//the ones you use because, if they aren't used,
	//then they won't appear in the file that
	//the gcc executable creates as output
	//also includes the library
}

void produce(struct progline *programhead, FILE* target) {
	//just copy into target
}

void dump(struct progline *programhead) {
	struct progline* currpos;
	for (currpos = programhead; currpos->type != END; currpos = currpos->next)
		printf("type:%d,depth:%d,content:%s\n", currpos->type, currpos->depth, currpos->line);
}

void main(int argc, char** argv) {
	if (argc != 3) {
		printf("need <source> <target>\n");
		exit(0x01);
	}
	FILE* source = fopen(argv[1], "r");
	FILE* target = fopen(argv[2], "w");
	struct progline *programhead;
	naiveparse(source, programhead);
	programhead->line[0] = '\0';
	printf("head:%d, %s\n", programhead->type, programhead->line);
	/*
	bastardize(programhead);
	globalheap(programhead);
	functionallocate(programhead);
	addmacros(programhead);
	produce(programhead, target);
	*/
	dump(programhead);
}
