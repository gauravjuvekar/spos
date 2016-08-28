#include <stdio.h>

int main() {
	char name[1024];
	char number[1024];
	while (fscanf(stdin, "%s%s", name, number) != EOF) {
		fprintf(stdout, "%-*s%*s\n", 40, name, 80, number);
	}
	return 0;
}
