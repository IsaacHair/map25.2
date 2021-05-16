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
#define UNKNOWN 16
#define BASTARD 17
#define BASTARDSTART 18
#define GOTO 19
#define LABEL 20
#define BASTARDFX 21
#define BASTARDGHEAP 22
#define ARR 23
struct progline {
	char *line;
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
	programhead->line = malloc(sizeof(char)*1000);
	programhead->line[0] = '\0';
	programhead->depth = 0;
	programhead->next = malloc(sizeof(struct progline));
	currpos = programhead->next;
	currpos->previous = programhead;
	currpos->line = malloc(sizeof(char)*1000);
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
		currpos->next->line = malloc(sizeof(char)*1000);
		currpos->next->previous = currpos;
		currpos = currpos->next;
	}
	currpos->next = NULL;
	currpos->type = END;
	currpos->line = malloc(sizeof(char)*1000);
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
		return BASTARD;
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
	if (compare(buffer, "array"))
		return ARR;
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
	char *buffer;
	int i, j;
	for (i = 0; currpos->line[i] != '\0'; i++)
		;
	buffer = malloc(sizeof(char)*(i+1));
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
	currpos->previous->next->line = malloc(sizeof(char)*1000);
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
	char *buff;
	for (i = 0; programhead->next->line[i] != '\0'; i++)
		;
	buff = malloc(sizeof(char)*(i+1));
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next) {
		for (i = 0; currpos->line[i] != '\0'; i++)
			;
		buff = realloc(buff, sizeof(char)*(i+1));
		if (currpos->type == FOR) {
			printf("for:%s\n", currpos->line); //XXX
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
}

void behindbastard(struct progline *currpos, char* str) {
	insertbehind(currpos);
	writestring(currpos->previous->line, str);
	currpos->previous->type = BASTARD;
	currpos->previous->depth = currpos->depth;
}

int ierecurse(struct progline *currhead, int headdepth) {
	//this guy is recursive based on depth
	//has to reduce depth of content once done
	struct progline *currpos;
	struct progline *linebuff;
	char *buff;
	int thisdepth;
	int i, j;
	currpos = currhead;
	for (i = 0; currpos->line[i] != '\0'; i++)
		;
	buff = malloc(sizeof(char)*(i+1));
	for (; currpos->depth >= headdepth && currpos->type != END;
	     currpos = currpos->next) {
		for (i = 0; currpos->line[i] != '\0'; i++)
			;
		buff = realloc(buff, sizeof(char)*(i+1001));
		if (currpos->type == IF || currpos->type == ELIF) { //elif if recursed already
			thisdepth = currpos->depth;
			printf("thisdepth:%d\n", thisdepth);
			behindbastard(currpos, "makeaddrodd();");
			writestring(buff, "instval(\"");
			j = 9;
			for (i = 0; currpos->line[i] != ' '; i++, j++) {
				if (currpos->line[i] == '\0') {
					printf("invalid if statement\n");
					exit(0x06);
				}
				buff[j] = currpos->line[i];
			}
			writestring(buff+j, " jzor\", ");
			j+= 8;
			i+= 1;
			for (; currpos->line[i] != '\0'; i++, j++) {
				buff[j] = currpos->line[i];
			}
			writestring(buff+j, ");");
			j+= 2;
			buff[j] = '\0';
			behindbastard(currpos, buff);
			behindbastard(currpos, "{char str[1000]; sprintf(str, \"L%03x\", currentlabel); instexpnxt(\"dnc noop 0000\", str);}");
			behindbastard(currpos, "currentlabel++;");
			currpos->previous->next = currpos->next;
			currpos->next->previous = currpos->previous;
			linebuff = currpos;
			currpos = currpos->previous;
			free(linebuff);
			//don't have do do any more because if there is another "if" it is already odd;
			//program addr won't be screwed up
			currpos = currpos->next;
			ierecurse(currpos, thisdepth+1);
			while (currpos->depth > thisdepth && currpos->type != END) {
				currpos->depth--;
				currpos = currpos->next;
			}
			if (currpos->type == ELSE) {
				behindbastard(currpos, "{char str[1000]; sprintf(str, \"L%03x\", currentlabel); instexpnxt(\"dnc noop 0000\", str);}");
				behindbastard(currpos, "currentlabel--;");
				currpos->previous->depth = thisdepth;
				behindbastard(currpos, "replacex88expimm(sprintf(\"L%03x\", currentlabel), addr);");
				currpos->previous->depth = thisdepth;
				currpos->previous->next = currpos->next;
				currpos->next->previous = currpos->previous;
				linebuff = currpos;
				currpos = currpos->previous;
				free(linebuff);
				ierecurse(currpos, thisdepth+1);
				currpos = currpos->next;
				while (currpos->depth > thisdepth && currpos->type != END) {
					currpos->depth--;
					currpos = currpos->next;
				}
				behindbastard(currpos, "currentlabel--;");
				currpos->previous->depth = thisdepth;
				behindbastard(currpos, "replacex88expimm(sprintf(\"L%03x\", currentlabel), addr);");
				currpos->previous->depth = thisdepth;
			}
			else if (currpos->type == ELIF) {
				ierecurse(currpos, thisdepth);
				behindbastard(currpos, "currentlabel--;");
				currpos->previous->depth = thisdepth;
				behindbastard(currpos, "replacex88expimm(sprintf(\"L%03x\", currentlabel), addr);");
				currpos->previous->depth = thisdepth;
				return 0; //don't have to loop again; the ierecurse() you just called did that
			}
			else {
				behindbastard(currpos, "currentlabel--;");
				currpos->previous->depth = thisdepth;
				behindbastard(currpos, "replacex88expimm(sprintf(\"L%03x\", currentlabel), addr);");
				currpos->previous->depth = thisdepth;
			}
			//have to make up for currpos advance
			currpos = currpos->previous;
		}
		else if (currpos->type == ELSE) {
			printf("else without preceding if\n");
			exit(0x05);
		}
	}
	printf("type:%d\n", currpos->type); //XXX
	return 0;
}

void ieparse(struct progline *programhead) {
	ierecurse(programhead->next, 0);
}

void arrayparse(struct progline *programhead) {
	struct progline *currpos;
	int i, j, k, namelen;
	char *buff;
	char str[1000];
	char name[1000], num[1000];
	buff = malloc(sizeof(char)*1000);
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next) {
		if (currpos->type == ARR) {
			if (currpos->previous->type != HEAP) {
				printf("array without preceding heap\n");
				exit(0x0a);
			}
			for (i = 0; currpos->line[i] != '\0';) {
				for (i = 0; currpos->previous->line[i] != '\0'; i++)
					;
				buff = realloc(buff, sizeof(char)*(i+1));
				while (currpos->line[i] == ' ')
					i++;
				namelen = 0;
				for (j = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++, j++)
					name[j] = currpos->line[i];
				name[j] = '\0';
				namelen+= j;
				while (currpos->line[i] == ' ')
					i++;
				for (j = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++, j++)
					num[j] = currpos->line[i];
				num[j] = '\0';
				k = atoi(num);
				namelen+= 6; //just gonna force it to be a 5 digit decimal number & a space after
				for (j = 0; currpos->previous->line[j] != '\0'; j++)
					buff[j] = currpos->previous->line[j];
				buff[j] = '\0';
				currpos->previous->line = realloc(currpos->previous->line, sizeof(char)*(j+namelen*k+1));
				for (j = 0; buff[j] != '\0'; j++)
					currpos->previous->line[j] = buff[j];
				currpos->previous->line[j] = '\0';
				for (; k > 1; k--) {
					if (j > 0)
						sprintf(str, " %s%05d", name, k-2);
					else
						sprintf(str, "%s%05d", name, k-2);
					writestring(currpos->previous->line+j, str);
					if (j > 0)
						j += namelen;
					else
						j += namelen-1; //since no space
				}
				sprintf(str, " %s", name);
				writestring(currpos->previous->line+j, str);
			}
		}
	}
}

void maininit(struct progline *programhead) {
	struct progline *mainpos;
	struct progline *toppos;
	char str[1000];
	toppos = programhead->next;
	for (mainpos = programhead->next; mainpos->type != MAIN; mainpos = mainpos->next)
		if (mainpos->type == END) {
			printf("failed to find !main\n");
			exit(0x0b);
		}
	mainpos = mainpos->next;
	sprintf(str, "int heapcount; int currentlabel;\
		      int fxframe[(1<<15)];");
	behindbastard(toppos, str);
	sprintf(str, "heapcount = 0; currentlabel = 0; progaddr = 0; setimmimm(0xffff, 0x8000);");
	behindbastard(mainpos, str);
}

void fxdef(struct progline *programhead) {
	//also initializes main
	struct progline *currpos;
	struct progline *insertpos;
	struct progline *mainpos;
	struct progline *fxpos;
	struct progline *endmainpos;
	struct progline *nonbaspos;
	struct progline *scanpos;
	char fxname[1000], varname[1000];
	char str[1000];
	int i, j;
	int fxdepth;
	insertpos = programhead->next;
	for (mainpos = programhead->next; mainpos->type != MAIN; mainpos = mainpos->next)
		if (mainpos->type == END) {
			printf("failed to find !main\n");
			exit(0x0b);
		}
	mainpos = mainpos->next->next; //since have to give one line for main init
	for (endmainpos = mainpos->next; endmainpos->type != END && endmainpos->depth > mainpos->previous->previous->depth; endmainpos = endmainpos->next)
		;
	behindbastard(endmainpos, "//separator");
	nonbaspos = endmainpos->previous;
	//search for functions and make/use the associated stuff without processing the actual one
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next) {
		if (currpos->type == FX) {
			fxdepth = currpos->depth;
			fxpos = currpos->next;
			for (i = 0; currpos->line[i] != ' '; i++) {
				if (currpos->line[i] == '\0') {
					break;
				}
				fxname[i] = currpos->line[i];
			}
			fxname[i] = '\0';
			sprintf(str, "_____%s", fxname);
			writestring(currpos->line, fxname);

			scanpos = fxpos;
			while (scanpos->depth > fxdepth && scanpos->type != END)
				scanpos = scanpos->next;
			printf("loc stopped at:%s\n", scanpos->line); //XXX
			sprintf(str, "inst(\"imm addr0 ffff\"); instval(\"imm addr1\", _____%sret); inst(\"ram asnx 0000\");", fxname);
			behindbastard(scanpos, str);
			scanpos->previous->depth = fxdepth+1;

			sprintf(str, "int idx_fxvar; idx_fxvar = 0;", fxname);
			behindbastard(fxpos, str);

			sprintf(str, "unsigned short _____%sret;", fxname);
			behindbastard(insertpos, str);
			sprintf(str, "makeheap(&_____%sret);", fxname);
			behindbastard(mainpos, str);
			sprintf(str, "fxframe[idx_fxvar] = _____%sret; idx_fxvar++;", fxname);
			behindbastard(fxpos, str);
			while(currpos->line[i] == ' ')
				i++;
			sprintf(str, "unsigned short _____%sarg[1000];", fxname);
			behindbastard(insertpos, str);
			for (; currpos->line[i] != '\0';) {
				for (j = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++, j++) {
					varname[j] = currpos->line[i];
				}
				while(currpos->line[i] == ' ')
					i++;
				varname[j] = '\0';
				sprintf(str, "unsigned short _____%s_%s;", fxname, varname);
				behindbastard(insertpos, str);
				sprintf(str, "makeheap(&_____%s_%s);", fxname, varname);
				behindbastard(mainpos, str);
				sprintf(str, "fxframe[idx_fxvar] = _____%s_%s;", fxname, varname);
				behindbastard(fxpos, str);
				sprintf(str, "_____%sarg[idx_fxvar] = _____%s_%s; idx_fxvar++;", fxname, fxname, varname);
				behindbastard(fxpos, str);
				sprintf(str, "int %s = _____%s_%s", varname, fxname, varname);
				behindbastard(fxpos, str);
			}
			sprintf(str, "_____%sarg[idx_fxvar] = -1;", fxname);
			behindbastard(fxpos, str);
			currpos = currpos->next;
			if (currpos->type == HEAP) {
				i = 0;
				while(currpos->line[i] == ' ')
					i++;
				for (; currpos->line[i] != '\0';) {
					for (j = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++, j++) {
						varname[j] = currpos->line[i];
					}
					while(currpos->line[i] == ' ')
						i++;
					varname[j] = '\0';
					sprintf(str, "unsigned short _____%s_%s;", fxname, varname);
					behindbastard(insertpos, str);
					sprintf(str, "makeheap(&_____%s_%s);", fxname, varname);
					behindbastard(mainpos, str);
					sprintf(str, "fxframe[idx_fxvar] = _____%s_%s; idx_fxvar++;", fxname, varname);
					behindbastard(fxpos, str);
					sprintf(str, "int %s = _____%s_%s", varname, fxname, varname);
					behindbastard(fxpos, str);
				}
			}
			sprintf(str, "unsigned short _____%sloc;", fxname);
			behindbastard(insertpos, str);
			sprintf(str, "_____%sloc = addr;", fxname);
			behindbastard(fxpos, str);
			sprintf(str, "fxframe[idx_fxvar] = -1;", fxname, varname);
			behindbastard(fxpos, str);
			sprintf(str, "char _____%slabel[5];", fxname);
			behindbastard(insertpos, str);
			sprintf(str, "makelabel(_____%slabel);", fxname);
			behindbastard(mainpos, str);
			sprintf(str, "replacex88expimm(_____%slabel, _____%sloc);", fxname, fxname);
			behindbastard(endmainpos, str);
			sprintf(str, "_____%s()", fxname);
			behindbastard(nonbaspos, str);
			currpos = currpos->previous; //have to make up for the advance check
		}
	}		
}

int fxmaccall(struct progline *programhead) {
	struct progline *currpos;
	struct progline *scanpos;
	int i, j;
	char str[1000];
	char buff[1000];
	int type;
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next) {
		if (currpos->type == CALL) {
			//see which type of call it is; types use same defn as line types
			type = MACRO;
			for (scanpos = programhead; scanpos->type != END; scanpos = scanpos->next) {
				if (scanpos->type == FX) {
					for (i = 0; scanpos->line[i] == ' ' || scanpos->line[i] == '\t'; i++)
						;
					for (j = 0; scanpos->line[i] != ' ' && scanpos->line[i] != '\0'; i++, j++)
						str[j] = scanpos->line[i];
					str[j] = '\0';
					if (compare(currpos->line, str)) {
						type = FX;
						break;
					}
				}
			}
			if (type == MACRO) {
				currpos->type == BASTARD;
				sprintf(str, "%s();", currpos->line);
				writestring(currpos->line, str);
				return 0;
			}
			//type must be FX then
			for (i = 0; currpos->line[i] == ' ' || currpos->line[i] == '\t'; i++)
				;
			for (i = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; i++)
				;
			while (currpos->line[i] == ' ') {
				for (j = 0; currpos->line[i] != ' ' && currpos->line[i] != '\0'; j++, i++) {
					str[j] = currpos->line[i];
				}
				str[j] = '\0';
				sprintf(buff, "instval(\"imm addr0\", ");
				behindbastard(currpos, buff);
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

void freeall(struct progline *programhead) {
	struct progline *currpos;
	for (currpos = programhead->next; currpos->type != END; currpos = currpos->next)
		free(currpos->previous);
	free(currpos->previous);
	free(currpos);
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
	ieparse(programhead);
	arrayparse(programhead);
	maininit(programhead);
	fxdef(programhead); //allocates functions and makes calls
	fxmaccall(programhead);
	/*
	globalheap(programhead);
	mainend(programhead);
	bastardize(programhead);
	produce(programhead, target);
	*/
	dump(programhead);
	freeall(programhead);
}
