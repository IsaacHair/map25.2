#include <stdlib.h>
#include <stdio.h>

//first arg is the source file, second is the target file
//third arg is the first buffer file, fourth is the other buffer file
//buffer files can be used to examine progress, but they are basically garbage

//note: everything ends with >lower case< hex
//default to lower case for all

int main(int argc, char* argv[]) {
  int vsbclean();
  void vsbmap();
  void vsbpage();

  if (argc != 4) {
    printf("usage: ./vas <source file> <target file> <buffer file> \n");
    return (-1);
  }
  
  FILE *source = fopen(argv[1], "r");
  FILE *target = fopen(argv[2], "w");
  FILE *clean = fopen(argv[3], "w");

  printf("started\n");
  vsbclean(source, clean);
  printf("cleaned\n");
  fclose(clean);
  vsbpage(target, argv[3]);
  printf("paged\n");

  fclose(source);
  fclose(target);
  fclose(clean);
  return (0);
}

int vsbclean(FILE *source, FILE *clean) {
  //remove comments
  //store the data as a continuous stream
  //4 characters for address, has not been re-ordered
  //2 characters for data to be written to each chip, has not been re-ordered
  //continuous stream of information
  char c;
  int i;
  for (c = fgetc(source); c != EOF; c = fgetc(source))
    if (c == '/') {
      while ((c = fgetc(source)) != '\n')
	if (c == EOF)
	  return (0);
    }
    else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')) {
      for (i = 0; i < 4; i++, c = fgetc(source))
	fprintf(clean, "%c", c);
      c = fgetc(source);
      fgetc(source);
      fgetc(source);
      switch (c) {
      case 'j':
	if ((c = fgetc(source)) == 'i')
	  fprintf(clean, "00");
	else
	  fprintf(clean, "20");
	break;
      case 'o':
	if ((c = fgetc(source)) == '0')
	  fprintf(clean, "40");
	else
	  fprintf(clean, "60");
	break;
      case 'a':
	if ((c = fgetc(source)) == '0')
	  fprintf(clean, "80");
	else
	  fprintf(clean, "a0");
	break;
      case 'b':
	if ((c = fgetc(source)) == '0')
	  fprintf(clean, "c0");
	else
	  fprintf(clean, "e0");
	break;
      }
      c = fgetc(source);
      for (i = 0; i < 4; i++)
	fprintf(clean, "%c", (c = fgetc(source)));
      c = fgetc(source);
      for (i = 0; i < 4; i++)
	fprintf(clean, "%c", (c = fgetc(source)));
    }
}
      
void vsbpage(FILE *target, char *location) {
  //write data as page instructions
  //4 characters for page
  //2 characters for line on that page
  //2 characters for data of 0th chip
  //continue this pattern of 8 bytes until end of page
  //terminate page with x
  //upon upload this is converted to ff
  //max line is 7f so no conflict
  //continue pattern for the remaining pages,
  //then terminate chip with z
  //upon upload this is converted to ffff
  //max page is ff80 so no conflict
  //repeat this whole process for the other chips
  //again, everything is in eeprom order
  //no spaces or any garbage inserted, just a stream of data

  FILE *mapped;
  int i, j, k;
  int ba;
  int v;
  int data;
  char c;
  mapped = fopen(location, "r");
  for (k = 0; k < 5; k++) {
    for (j = 0; j < (1 << 16); j += 128) {
      //reset to beginning of file to check for the next page
      fclose(mapped);
      mapped = fopen(location, "r");
      //see if there is data at page j
      for (data = 0, c = fgetc(mapped); c != EOF; c = fgetc(mapped)) {
	for (i = 0, ba = 0; i < 4;
	     i++, ba = (ba << 4) + v, i < 4 ? c = fgetc(mapped) : 0)
	  if (c >= '0' && c <= '9')
	    v = c-'0';
	  else
	    v = c-'a'+10;
	if ((ba/128)*128 == j) {
	  data = 1;
	  break;
	}
	for (i = 0; i < 10; i++)
	  fgetc(mapped);
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
      fclose(mapped);
      mapped = fopen(location, "r");
      for (c = fgetc(mapped); c != EOF; c = fgetc(mapped)) {
	for (i = 0, ba = 0; i < 4;
	     i++, ba = (ba << 4) + v, i < 4 ? c = fgetc(mapped) : 0)
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
	  for (i = 0; i < 2*k; i++)
	    fgetc(mapped);
	  //print data for the chip at that spot
	  for (i = 0; i < 2; i++)
	    fprintf(target, "%c", fgetc(mapped));
	  //pad out to next entry
	  for (i = 0; i < 8-2*k; i++)
	    fgetc(mapped);
	}
	//if the entry isn't the desired page, pad out to next entry
	else
	  for (i = 0; i < 10; i++)
	    fgetc(mapped);
      }
      //done with everything for this page
      fprintf(target, "x");
    }
    //done with everything for this chip
    fprintf(target, "z");
  } 
}
