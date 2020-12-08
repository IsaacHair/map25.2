#include <stdlib.h>
#include <stdio.h>

void main(int argc, char** argv) {
	if (argc != 2) {
		printf("enter file path to test\n");
		exit(0x01);
	}
	FILE* fd = fopen(argv[1], "r");
	char c;
	char addr[100000][4]; //more than possible length because
			     //collisions mean doubling up
	int i, j, line, lineidx, comment, eql;
	for (line = 1, comment = 0, lineidx = 0, c = fgetc(fd); c != EOF; c = fgetc(fd)) {
		if (c == '<')
			comment = 1;
		if (c == '>')
			comment = 2;
		if (lineidx < 4 && !comment)
			addr[line][lineidx] = c;
		if (lineidx < 4 && comment)
			addr[line][lineidx] = '\0';
		if (lineidx == 3 && !comment)
			for (j = 0; j < line; j++) {
				for (i = 0, eql = 1; i < 4; i++)
					if (!(addr[line][i] == addr[j][i]))
						eql = 0;
				if (eql) //not comment if equal by default
					printf("Collision:\nLines(decimal):%d,%d\n", line, j);
			}
		lineidx++;
		if (c == '\n') {
			line++;
			lineidx = 0;
			if (comment == 2)
				comment = 0;
		}
	}
}
