#include <stdlib.h>
#include <stdio.h>

struct line {
	char* content;
	struct line *nested;
	struct line *next;
};

//this will return the tab depth (or current depth if line is blank/comment)
int depthof(char* text, int curr_depth) {
	int i;
	for (i = 0; text[i] != '\0'; i++) {
		if (i == '\n')
			return curr_depth;
		else if (i == '/') {
			if (text[i+1] == '/')
				return curr_depth;
		}
		else if (text[i] != '\t')
			return i;
	}
}

//this bad boy just does some very basic opening etc
//also recursive yay
int rambuffer(FILE* source, struct line *program, int depth) {
	char c;
	int i;
	char* next;
	program->nested = NULL;
	program->next = NULL;
	for (i = 0, c = fgetc(source); c != '\n' && c != EOF; c = fgetc(source), i++)
		;
	if (c == EOF)
		fseek(source, -i, SEEK_CUR);
	else
		fseek(source, -i-1, SEEK_CUR);
	program->content = malloc((i+1)*sizeof(char));
	for (i = 0, c = fgetc(source); c != '\n' && c != EOF; c = fgetc(source), i++)
		program->content[i] = c;
	program->content[i] = '\0';

	for (i = 0, c = fgetc(source); c != '\n' && c != EOF; c = fgetc(source), i++)
		;
	if (c == EOF)
		fseek(source, -i, SEEK_CUR);
	else
		fseek(source, -i-1, SEEK_CUR);
	next = malloc((i+1)*sizeof(char));
	for (i = 0, c = fgetc(source); c != '\n' && c != EOF; c = fgetc(source), i++)
		next[i] = c;
	next[i] = '\0';
	if (c == EOF)
		fseek(source, -i, SEEK_CUR);
	else
		fseek(source, -i-1, SEEK_CUR);

	if (c == EOF || depthof(next, depth) < depth)
		return 0;
	else if (depthof(next, depth) > depth) {
		program->nested = malloc(sizeof(struct line));
		program->next = malloc(sizeof(struct line));
		rambuffer(source, program->nested, depth+1);
		rambuffer(source, program->next, depth);
	}
	else if (depthof(next, depth) == depth) {
		program->next = malloc(sizeof(struct line));
		rambuffer(source, program->next, depth);
	}
	else {
		printf("wtf\n");
		exit(0x02);
	}
}

//just a test program to see the struct contents
void dump(struct line program) {
	printf("%s\n", program.content);
	if (program.nested) {
		printf("depth++\n");
		dump(*program.nested);
		printf("depth--\n");
	}
	if (program.next) {
		dump(*program.next);
	}
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
	source = fopen(argv[1], "rw+");
	rambuffer(source, &program, 0);
	dump(program);
}
