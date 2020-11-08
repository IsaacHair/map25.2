#include <stdlib.h>
#include <stdio.h>

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("enter file path to test\n");
		exit(0x01);
	}
	FILE* fd = fopen(argv[1], "r");
	char c;
	char addr[70000][4]; //more than possible length because
			     //collisions mean doubling up
	int i, line;
	for (line = 1, c = fgetc(fd); c != EOF; c = fgetc(fd), line++) {
		addr[line][0] = c;
		addr[line][1] = fgetc(fd);
		addr[line][2] = fgetc(fd);
		addr[line][3] = fgetc(fd);
		while ((c = fgetc(fd)) != '\n' && c != EOF)
			;
		for (i = line-1; i >= 0; i--)
			if (addr[line][0] == addr[i][0] &&
			    addr[line][1] == addr[i][1] &&
			    addr[line][2] == addr[i][2] &&
			    addr[line][3] == addr[i][3])
				if ((addr[line][0] >= '0' && addr[line][0] <= '9') ||
				    (addr[line][0] >= 'a' && addr[line][0] <= 'f'))
					//need to check if comment or not
					printf("collision:\nline(decimal) %08d repeats line(decimal) %08d\n", line, i);
	}
}
