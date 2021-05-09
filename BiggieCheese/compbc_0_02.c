#include <stdlib.h>
#include <stdio.h>

#define END 0
#define START 1
#define MAIN 2
#define HEAP 3
#define MACRO 4
#define ASM 5
#define FOR 6
#define IF 7
#define ELSE 8
#define ELIF 9
#define FORDEF 10
#define IFDEF 11
#define ELSEDEF 12
#define ELIFDEF 13
#define FX 14
#define CALL 15
#define EVAL 16
#define UNKNOWN 17
#define BASTARD 18
#define BASTARDSTART 19
#define GOTO 20
#define LABEL 21
#define BASTARDFX 22
#define BASTARDGHEAP 23
struct progline {
	char line[1000];
	int type;
	int depth;
	struct progline* next;
	struct progline* previous;
};

void naiveparse(FILE* source, struct progline *programhead) {
	//Just remove comments and blank lines and depth tabs/spaces
	//Also allocate programhead and do the depth
	//NO \n AT END OF LINE, JUST \0
	int i;
	struct progline* currpos;
	programhead->type = START;
	programhead->previous = NULL;
	programhead->line[0] = '\0';
	programhead->depth = 0;
	programhead->next = malloc(sizeof(struct progline));
	currpos = programhead->next;
	currpos->previous = programhead;
	char c;
	int linestart;
	for (c = fgetc(source); c != EOF; c = fgetc(source)) {
		for (i = 0; c == ' ' || c == '\t'; c = fgetc(source), i++)
			;
		currpos->depth = i;
		currpos->type = UNKNOWN;
		for (i = 0; c != '\n' && c != EOF; c = fgetc(source), i++)
			currpos->line[i] = c;
		currpos->line[i] = '\0';
		if (i == 0 || (i > 1 ? (currpos->line[0] == '/' && currpos->line[1] == '/') : 0))
			continue; //just override with the next line if this line is blank or comment
		currpos->next = malloc(sizeof(struct progline));
		currpos->next->previous = currpos;
		currpos = currpos->next;
	}
	currpos->next = NULL;
	currpos->type = END;
	currpos->line[0] = '\0';
	currpos->depth = 0;
}

int compare(char* str0, char* str1) {
	//strings can be zero length
	int i;
	for (i = 0; str0[i] == str1[i]; i++)
		if (str0[i] == '\0')
			return 1;
	return 0;
}

int linetype(char* buffer) {
	//buffer must have at least 1 character
	int i;
	for (i = 0; buffer[i] != '\0'; i++)
		;
	if (buffer[i-1] == ':')
		return LABEL;
	if (compare(buffer, "goto"))
		return GOTO;
	if (buffer[0] == '!') {
		if (compare(buffer, "!main"))
			return MAIN;
		else
			return FX;
	}
	if (buffer[0] == '#')
		return MACRO;
	if (compare(buffer, "if"))
		return IF;
	if (compare(buffer, "else"))
		return ELSE;
	if (compare(buffer, "elif"))
		return ELIF;
	if (compare(buffer, "for"))
		return FOR;
	if (compare(buffer, "ifdef"))
		return IFDEF;
	if (compare(buffer, "elsedef"))
		return ELSEDEF;
	if (compare(buffer, "elifdef"))
		return ELIFDEF;
	if (compare(buffer, "fordef"))
		return FORDEF;
	if (compare(buffer, "heap"))
		return HEAP;
	if (compare(buffer, "eval"))
		return EVAL;
	if (compare(buffer, "imm") ||
	    compare(buffer, "0") ||
	    compare(buffer, "adr") ||
	    compare(buffer, "addr") ||
	    compare(buffer, "1") ||
	    compare(buffer, "gen") ||
	    compare(buffer, "2") ||
	    compare(buffer, "rol") ||
	    compare(buffer, "3") ||
	    compare(buffer, "ror") ||
	    compare(buffer, "4") ||
	    compare(buffer, "out") ||
	    compare(buffer, "5") ||
	    compare(buffer, "dir") ||
	    compare(buffer, "6") ||
	    compare(buffer, "inp") ||
	    compare(buffer, "in") ||
	    compare(buffer, "7") ||
	    compare(buffer, "ram") ||
	    compare(buffer, "8") ||
	    compare(buffer, "dnc") ||
	    compare(buffer, "nc") ||
	    compare(buffer, "9") ||
	    compare(buffer, "a") ||
	    compare(buffer, "b") ||
	    compare(buffer, "c") ||
	    compare(buffer, "d") ||
	    compare(buffer, "e") ||
	    compare(buffer, "f"))
		return ASM;
	return CALL;
}

int haskeyword(int type) {
	//check if doesn't because faster
	if (type == END || type == START ||
	    type == MACRO || type == ASM ||
	    type == FX || type == CALL ||
	    type == LABEL)
		return 0;
	return 1;
}

void asntype(struct progline *currpos) {
	char buffer[1000];
	int i, j;
	for (i = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++)
		buffer[i] = currpos->line[i];
	buffer[i] = '\0';
	currpos->type = linetype(buffer);
	if (haskeyword(currpos->type)) { //delete keyword if needed
		if (currpos->line[i] == '\0')
			i--; //dont want to be reading past '\0'
		for (i++, j = 0; currpos->line[i] != '\0'; i++, j++)
			buffer[j] = currpos->line[i];
		buffer[j] = '\0';
		for (i = 0; buffer[i] != '\0'; i++)
			currpos->line[i] = buffer[i];
		currpos->line[i] = '\0';
	}
	if (currpos->type == MACRO || currpos->type == FX || currpos->type == LABEL) { //get rid of marker character
		for (i = 0, j = 0; currpos->line[i] != '\0'; i++)
			if ((currpos->line[i] != '#' && currpos->line[i] != '!' &&
			    currpos->line[i] != ':') || j != i) {
				buffer[j] = currpos->line[i];
				j++;
			}
		buffer[j] = '\0';
		for (i = 0; buffer[i] != '\0'; i++)
			currpos->line[i] = buffer[i];
		currpos->line[i] = '\0';
	}
}

void insertbehind(struct progline* currpos) {
	currpos->previous->next = malloc(sizeof(struct progline));
	currpos->previous->next->previous = currpos->previous;
	currpos->previous->next->next = currpos;
	currpos->previous = currpos->previous->next;
}

void writestring(char* target, char* str) {
	int i;
	for (i = 0; str[i] != '\0'; i++)
		target[i] = str[i];
	target[i] = '\0';
}

void forparse(struct progline *programhead) {
	struct progline *currpos;
	struct progline *endpos;
	int i, j;
	char buff[1000];
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next)
		if (currpos->type == FOR) {
			i = 0;
			//setup part
			for (; currpos->line[i] != '\0'; i++) {
				for (; currpos->line[i] == ' ' || currpos->line[i] == '\t'; i++)
					;
				for (j = 0; currpos->line[i] != ';' && currpos->line[i] != '\0'; i++, j++)
					buff[j] = currpos->line[i];
				buff[j] = '\0';
				if (currpos->line[i] == '\0') {
					printf("incomplete for loop\n");
					exit(0x04);
				}
				if (j > 0) { //if the line isn't blank insert a line behind
					insertbehind(currpos);
					currpos->previous->depth = currpos->depth;
					for (j = 0; buff[j] != '\0'; j++)
						currpos->previous->line[j] = buff[j];
					asntype(currpos->previous);
				}
				if (currpos->line[i+1] == ':') {
					i += 2;
					break;
				}
			}
			//condition part
			insertbehind(currpos);
			writestring(currpos->previous->line, "forstart");
			currpos->previous->depth = currpos->depth;
			currpos->previous->type = LABEL;
			insertbehind(currpos);
			for (; currpos->line[i] == ' ' || currpos->line[i] == '\t'; i++)
				;
			for (j = 0; currpos->line[i] != ';' && currpos->line[i] != '\0'; i++, j++)
				buff[j] = currpos->line[i];
			buff[j] = '\0';
			if (currpos->line[i] == '\0') {
				printf("incomplete for loop\n");
				exit(0x04);
			}
			if (j == 0) {
				printf("for expected condition but was given none\n");
				exit(0x05);
			}
			if (currpos->line[i+1] != ':') {
				printf("for cannot have multiple conditions\n");
				exit(0x06);
			}
			writestring(currpos->previous->line, buff);
			currpos->previous->type = IF;
			currpos->previous->depth = currpos->depth;
			i += 2;
			//iteration part
			for (endpos = currpos->next; endpos->depth > currpos->depth && endpos->type != END; endpos = endpos->next)
				;
			for (; currpos->line[i] != '\0'; i++) {
				for (; currpos->line[i] == ' ' || currpos->line[i] == '\t'; i++)
					;
				for (j = 0; currpos->line[i] != ';' && currpos->line[i] != '\0'; i++, j++)
					buff[j] = currpos->line[i];
				buff[j] = '\0';
				if (j > 0) { //if the line isn't blank insert a line behind endpos
					insertbehind(endpos);
					endpos->previous->depth = currpos->depth+1;
					for (j = 0; buff[j] != '\0'; j++)
						endpos->previous->line[j] = buff[j];
					asntype(endpos->previous);
				}
				if (currpos->line[i] == '\0')
					break;
			}
			//loop part
			insertbehind(endpos);
			writestring(endpos->previous->line, "forstart");
			endpos->previous->type = GOTO;
			endpos->previous->depth = currpos->depth+1;
			//de-allocate the for statement now that it has been translated
			currpos->previous->next = currpos->next;
			currpos->next->previous = currpos->previous;
			free(currpos);
		}
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
	/*
	printf("*****now going backwards****\n");
	for (; currpos->type != START; currpos = currpos->previous)
		printf("type:%d,depth:%d,content:%s\n", currpos->type, currpos->depth, currpos->line);
	*/
}

void main(int argc, char** argv) {
	if (argc != 3) {
		printf("need <source> <target>\n");
		exit(0x01);
	}
	FILE* source = fopen(argv[1], "r");
	FILE* target = fopen(argv[2], "w");
	struct progline *currpos;
	struct progline *programhead = malloc(sizeof(struct progline));
	naiveparse(source, programhead);
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next)
		asntype(currpos);
	forparse(programhead);
	/*
	fxparse(programhead);
	ieparse(programhead);
	bastardize(programhead);
	globalheap(programhead);
	fxallocate(programhead);
	addmacros(programhead);
	produce(programhead, target);
	*/
	dump(programhead);
}