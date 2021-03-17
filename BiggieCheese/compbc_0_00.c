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
		else if (text[i] == '/') {
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

//need to have a dummy line to start in case the first line is a comment or blank, etc
//also inits depth at zero
//program needs to already be allocated
int rambufferstart(FILE* source, struct line *program) {
	program->nested = NULL;
	program->content = malloc(sizeof(char));
	program->content[0] = '\0';
	program->next = malloc(sizeof(struct line));
	rambuffer(source, program->next, 0);
}

void clean(struct line *program) {
	//remove blank lines and comment lines
	//don't have to worry about nesting for these
	//also delete '\t's
	int i, j;
	for (i = 0; program->content[i] == '\t'; i++)
		;
	for (j = i; program->content[j] != '\0'; j++)
		program->content[j-i] = program->content[j];
	program->content[j-i] = '\0';
	if (program->nested)
		clean(program->nested);
	if (program->next) {
		for (i = 0; program->next->content[i] == '\t'; i++)
			;
		if (program->next->content[i] == '\0' || program->next->content[i] == '/') {
			program->next = program->next->next;
			clean(program);
		}
		else
			clean(program->next);
	}
}

void firstword(char* content, char* ret) {
	int i;
	for (i = 0; content[i] != ' ' && content[i] != '\0'; i++)
		;
	ret = malloc((i+1)*sizeof(char));
	for (i = 0; content[i] != ' ' && content[i] != '\0'; i++)
		ret[i] = content[i];
	ret[i] = '\0';
}

int compare(char* str1, char* str2) {
	int i;
	for (i = 0; str1[i] == str2[i]; i++)
		if (str1[i] == '\0')
			return 1;
	return 0;
}

void insertmacro(char* name, struct line *program, struct line *top) {
	char* buff;
	int i;
	firstword(program->next->content[i], buff);
	if (compare(buff, name)) {
		replacenextwithcode(program, 
/*XXX There is a problem: you need a blank line before every nested thingy for this to be OK*/

void insertallmacro(struct line *program, struct line *top) {
	//insert macros and delete the original after
	char* name;
	int i;
	if (program->content[i] == '#') {
		firstword(program->content[i], name);
		//get rid of hashtag
		for (i = 1; name[i] != '\0'; i++)
			name[i-1] = name[i];
		name[i-1] = '\0';
		insertmacro(name, program, top);
	}
	//can skip nested stuff because macros only appear on bottom-most depth
	if (program->next)
		insertallmacro(program->next);
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
	source = fopen(argv[1], "r");
	rambufferstart(source, &program);
	clean(&program);
	insertallmacro(&program, &program);
	dump(program);
}
