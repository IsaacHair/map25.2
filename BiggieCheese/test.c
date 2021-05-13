#include <stdlib.h>
#include <stdio.h>

void main() {
	char *buff;
	int i;
	buff = malloc(sizeof(char)*10);
	for (i = 0; i < 9; i++) {
		buff[i] = 'a'+i;
	}
	buff[i] = '\0';
	printf("%s\n", buff);
	buff = realloc(buff, sizeof(char)*26);
	for (i = 0; i < 23; i++) {
		buff[i] = 'a'+i;
	}
	buff[i] = '\0';
	printf("%s\n", buff);
}
