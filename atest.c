#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  void page();
  void assemble();

  if (argc != 4) {
    printf("usage: ./vas <source file> <target file> <buffer file> \n");
    return (-1);
  }
  
  FILE *source = fopen(argv[1], "r");
  FILE *target = fopen(argv[2], "w");
  FILE *clean = fopen(argv[3], "w");
  printf("hola bitches\n");
  assemble(source, clean);
  fclose(clean);
  fopen(argv[3], "r");
  page(clean, target);
  return (0);
}

void assemble(FILE *source, FILE *clean) {
	char c;
	char* opcode = malloc(1*sizeof(char));
	int i, j, k;
	char opdecode();
	printf("ok aboutta get the first char\n");
	c = fgetc(source);
	printf("got that bitch\n");
	while (c != EOF) {
		printf("char\n");
		if (c == '<') {
			while ((c = fgetc(source)) != '>')
				if (c == EOF) {
					printf("Comment error\n");
					exit(0x01);
				}
			while (((c = fgetc(source)) == 10 || c == 13 ) &&
			       c != EOF)
				;
			continue;
		}

		for (i = 0; i < 4; i++) {
			if (!(((c <= '9') && (c >= '0')) || ((c <= 'f')
							&& (c >= 'a')))) {
				printf("invalid hex\n");
				exit(0x05);
			}
			fputc(c, clean);
			c = fgetc(source);
		}

		for (k = 0; k < 2; k++) {
			c = fgetc(source);
			for (j = 0; c != ' '; j++) {
				if (c == EOF) {
					printf("unexpected end\n");
					exit(0x04);
				}
				c = fgetc(source);
			}
			fseek(source, -j-1, SEEK_CUR);
			c = fgetc(source);
			opcode = realloc(opcode, (j+1)*sizeof(char));
			for (i = 0; c != ' '; i++) {
				opcode[i] = c;
				c = fgetc(source);
			}
			opcode[j] = '\0';
			fputc(opdecode(opcode), clean);
		}

		for (j = 0; j < 2; j++) {
			for (i = 0; i < 4; i++) {
				c = fgetc(source);
				if (!(((c <= '9') && (c >= '0')) || ((c <= 'f')
				      && (c >= 'a')))) {
					printf("invalid hex\n");
					exit(0x05);
				}
				fputc(c, clean);
			}
			c = fgetc(source);
		}

		while (c != 10 && c != 13)
			c = fgetc(source);
		while (c == 10 || c == 13)
			c = fgetc(source);
	}
}

int compare(char* str0, char* str1) {
	int i;
	for (i = 0; str0[i] == str1[i]; i++)
		if (str0[i] == '\0')
			return 1;
	return 0;
}

char opdecode(char* opcode) {
	int compare();

	if (compare("imm", opcode))
		return '0';
	else if (compare("adr", opcode) || compare("addr", opcode))
		return '1';
	else if (compare("gen", opcode))
		return '2';
	else if (compare("rol", opcode))
		return '3';
	else if (compare("ror", opcode))
		return '4';
	else if (compare("out", opcode))
		return '5';
	else if (compare("dir", opcode))
		return '6';
	else if (compare("inp", opcode) || compare("in", opcode))
		return '7';
	else if (compare("ram", opcode))
		return '8';
	else if (compare("dnc", opcode) || compare("nc", opcode))
		return '9';
	else if (compare("jzor", opcode) || compare("jumpor", opcode))
		return '0';
	else if (compare("asnx", opcode) || compare("asnnext", opcode))
		return '1';
	else if (compare("out0", opcode))
		return '2';
	else if (compare("out1", opcode))
		return '3';
	else if (compare("adr0", opcode) || compare("addr0", opcode))
		return '4';
	else if (compare("adr1", opcode) || compare("addr1", opcode))
		return '5';
	else if (compare("dir0", opcode))
		return '6';
	else if (compare("dir1", opcode))
		return '7';
	else if (compare("gen0", opcode))
		return '8';
	else if (compare("gen1", opcode))
		return '9';
	else if (compare("noop", opcode))
		return 'a';
	else if (compare("rlow", opcode) || compare("ramlow", opcode))
		return 'd';
	else if (compare("rupp", opcode) || compare("ramup", opcode)
		 || compare("rup", opcode))
		return 'e';
	else if (compare("rall", opcode) || compare("ramall", opcode))
		return 'f';
	else if ((opcode[0] >= '0' && opcode[0] <= '9')
		 || (opcode[0] >= 'a' && opcode[0] <= 'f'))
		if (opcode[1] == '\0')
			return opcode[0];
	else {
		printf("unrecognized opcode: %s\n", opcode);
		exit(0x06);
	}
}

void page(FILE *clean, FILE *target) {
  //Input format (clean): <instruction addr, 4 hex digits><op code, 2 hex
  //digits><immediate, 4 hex digits><next addr, 4 hex digits>
  //Note: no spaces or anything else, just a stream of hex data
  //Output format (target): <page addr, 4 hex digits><page line, 2 hex digits>
  //<line data, 2 hex digits>
  //line/data is repeated until everything for the page has been written
  //after each page is "x"
  //after each chip is "z"
  int i, j, k;
  int ba;
  int v;
  int data;
  char c;
  for (k = 0; k < 5; k++) {
    for (j = 0; j < (1 << 16); j += 128) {
      //reset to beginning of file to check for the next page
      rewind(clean);
      //see if there is data at page j
      for (data = 0, c = fgetc(clean); c != EOF; c = fgetc(clean)) {
	for (i = 0, ba = 0; i < 4;
	     i++, ba = (ba << 4) + v, i < 4 ? c = fgetc(clean) : 0)
	  if (c >= '0' && c <= '9')
	    v = c-'0';
	  else
	    v = c-'a'+10;
	if ((ba/128)*128 == j) {
	  data = 1;
	  break;
	}
	for (i = 0; i < 10; i++)
	  fgetc(clean);
      }
      //go to next possible page if no data at this page (page j)
      if (!data)
	continue;
      //copy down 4 characters for the page address in hex
      for (i = 0, ba = j; i < 4; i++, ba = (ba << 4) % (1 << 16))
	if (ba / (1 << 12) <= 9)
	  fprintf(target, "%c", (ba/(1 << 12))+'0');
	else
	  fprintf(target, "%c", (ba/(1 << 12))+'a'-10);
      //reset to beginning of file to find all entries in this page
      rewind(clean);
      for (c = fgetc(clean); c != EOF; c = fgetc(clean)) {
	for (i = 0, ba = 0; i < 4;
	     i++, ba = (ba << 4) + v, i < 4 ? c = fgetc(clean) : 0)
	  if (c >= '0' && c <= '9')
	    v = c-'0';
	  else
	    v = c-'a'+10;
	//if this entry is the page we want
	if ((ba/128)*128 == j) {
	  //print 2 characters for the line on the page in hex
	  for (i = 0, v = ba%128; i < 2; i++, v = (v << 4)%(1 << 8))
	    if (v/(1 << 4) <= 9)
	      fprintf(target, "%c", (v/(1 << 4))+'0');
	    else
	      fprintf(target, "%c", (v/(1 << 4))+'a'-10);
	  //pad out to the correct chip
	  for (i = 0; i < 8-2*k; i++)
	    fgetc(clean);
	  //print data for the chip at that spot
	  for (i = 0; i < 2; i++)
	    fprintf(target, "%c", fgetc(clean));
	  //pad out to next entry
	  for (i = 0; i < 2*k; i++)
	    fgetc(clean);
	}
	//if the entry isn't the desired page, pad out to next entry
	else
	  for (i = 0; i < 10; i++)
	    fgetc(clean);
      }
      //done with everything for this page
      fprintf(target, "x");
    }
    //done with everything for this chip
    fprintf(target, "z");
  } 
}
