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

void firstword(char* content, char** ret) {
	int i;
	for (i = 0; content[i] != ' ' && content[i] != '\0'; i++)
		;
	*ret = realloc(*ret, (i+1)*sizeof(char));
	for (i = 0; content[i] != ' ' && content[i] != '\0'; i++)
		(*ret)[i] = content[i];
	(*ret)[i] = '\0';
}

int compare(char* str1, char* str2) {
	int i;
	for (i = 0; str1[i] == str2[i]; i++)
		if (str1[i] == '\0')
			return 1;
	return 0;
}

void replacewithmacro(struct line *insertaddr, struct line *macroaddr) {
	//another recursive insert thingy
	struct line *aftermacro;
	int i;

	//only need aftermacro if it is the lowest depth
	if (insertaddr->next)
		aftermacro = insertaddr->next;
	else
		aftermacro = NULL;
	if (insertaddr->nested) {
		printf("wtf nested from macro\n");
		exit(0x02);
	}
	insertaddr->content = realloc(insertaddr->content, sizeof(macroaddr->content));
	for (i = 0; macroaddr->content[i] != '\0'; i++)
		insertaddr->content[i] = macroaddr->content[i];
	insertaddr->nested = NULL;
	insertaddr->next = NULL;
	if (macroaddr->nested) {
		insertaddr->nested = malloc(sizeof(struct line*));
		replacewithmacro(insertaddr->nested, macroaddr->nested);
	}
	if (macroaddr->next) {
		insertaddr->next = malloc(sizeof(struct line*));
		if (aftermacro)
			insertaddr->next->next = aftermacro;
		replacewithmacro(insertaddr->next, macroaddr->next);
	}
	else if (aftermacro)
		insertaddr->next = aftermacro;
}

void insertmacro(char* name, struct line *macroaddr, struct line *scan) {
	//"scan" scans down, macroaddr is the pointer to the macro, name is its label
	char* buff = malloc(1); //random size
	int i;
	firstword(scan->content, &buff);
	if (compare(buff, name))
		replacewithmacro(scan, macroaddr);
	if (scan->nested)
		insertmacro(name, macroaddr, scan->nested);
	if (scan->next)
		insertmacro(name, macroaddr, scan->next);
}

void insertallmacro(struct line *scan, struct line *top) {
	//insert macros and delete the original after
	//this function finds the macros by scanning for the # definition
	char* name = malloc(1); //just needs to have some size
	int i;
	if (scan->content[0] == '#') {
		firstword(scan->content, &name);
		//get rid of hashtag
		if (name[0] != '\n')
			for (i = 1; name[i] != '\0'; i++)
				name[i-1] = name[i];
		name[i-1] = '\0';
		printf("gotem:%s\n", name); //XXX
		/*XXX if (!scan->nested) {
			printf("empty macro dude\n");
			exit(0x03);
		}*/
		//XXX insertmacro(name, scan->nested, top);
	}
	//can skip nested stuff because macros only appear on bottom-most depth
	if (scan->next)
		insertallmacro(scan->next, top);
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
