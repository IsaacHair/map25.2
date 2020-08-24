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
  assemble(source, clean);
  fclose(clean);
  fopen(argv[3], "r");
  page(clean, target);
  fclose(source);
  fclose(target);
  fclose(clean);
  return (0);
}

void assemble(FILE *source, FILE *clean) {
	//Input format: <instruction addr, 4 hex digits><tab><op code upper>
	//<tab><op code lower><tab><immediate, 4 hex digits><tab><next addr,
	//4 hex digits><return>
	//Comments:type "/" on a fresh line, type comment on line after that,
	//then type "\" on a fresh line, then start code on line after that
	char c;
	char* opcode = malloc(1*sizeof(char));
	int i, j;
	char *opdecode();
}

char *opdecode(char* opcode) {
	return "a";
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
