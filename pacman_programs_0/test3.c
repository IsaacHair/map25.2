#include <stdlib.h>
#include <stdio.h>

void main() {
	register int ra, rb;
	int a, b;
	printf("\n");
	for (ra = 0, rb = 1; ra < (1<<24); (rb == 1) ? rb = 2 : (rb = 1)) {
		ra+=rb;
		printf("%d\x0d", ra);
	}
	printf("\n");
	for (a = 0, b = 1; a < (1<<24); (b == 1) ? b = 2 : (b = 1)) {
		a+=b;
		printf("%d\x0d", a);
	}
	printf("\n");
	for (a = 0; a < (1<<24);) {
		a+=1;
		printf("%d\x0d", a);
		a+=2;
		printf("%d\x0d", a);
	}
	printf("\n");
}
