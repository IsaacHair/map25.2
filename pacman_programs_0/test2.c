#include <stdlib.h>
#include <stdio.h>

void main() {
	int c;
	for (c = 0; c < 256; c++)
		printf("%d is: %c\n", c, c);
}
