#include <stdlib.h>
#include <stdio.h>

void main() {
	int i;
	int j;
	unsigned short or;
	int k;
	int longest;
	int current;
	double length;
	for (i = 0; i < 65536; i++) {
		if (!(i%1000))
			printf("i=%d\n", i);
		for (j = 0; j < 65536; j++) {
			or = 0x0000;
			or |= j;
			or |= i;
			for (k = 1, longest = current = 0; k < 0x10000; k *= 2) {
				if (or&k)
					current++;
				else
					current = 0;
				if (current > longest)
					longest = current;
			}
			length += (double)longest;
		}
	}
	length /= 0x100000000;
	printf("avg length: %f\n", length);
}
