#include <stdlib.h>
#include <stdio.h>

int main() {
	FILE *source1 = fopen("source1", "r");
	FILE *source2 = fopen("source2", "r");
	FILE *target = fopen("target", "w");

	int num1, num2;

	fread(&num1, sizeof(num1), 1, source1);
	fread(&num2, sizeof(num1), 1, source2);
	while (!feof(source1) || !feof(source2)) {
		while (!feof(source1) && ((num1 < num2) || feof(source2))) {
			fwrite(&num1, sizeof(num1), 1, target);
			fread(&num1, sizeof(num1), 1, source1);
		}
		while (!feof(source2) && ((num1 >= num2) || feof(source1))) {
			fwrite(&num2, sizeof(num1), 1, target);
			fread(&num2, sizeof(num1), 1, source2);
		}
	}
}
