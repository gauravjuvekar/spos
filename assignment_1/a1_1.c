#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char name[1024];
	char number[1024];

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
		exit(1);
	}
	FILE *input = fopen(argv[1], "r");
	FILE *output = fopen(argv[2], "w");

	if (input == NULL || output == NULL) {
		fprintf(stderr, "Unable to open input or output file\n");
		exit(1);
	}

	while (fscanf(input, "%s%s", name, number) != EOF) {
		fprintf(output, "%-*s%*s\n", 40, name, 80, number);
	}

	fclose(input);
	fclose(output);

	return 0;
}
