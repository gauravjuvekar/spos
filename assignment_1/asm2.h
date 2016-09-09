#ifndef ASM2_H
#define ASM2_H 1

#include <stdio.h>
#include "table.h"

void pass_2(
	const Table *symbol_table,
	const Table *literal_table,
	FILE *interm_code_file,
	FILE *output_file);
#endif /* ifndef ASM2_H */
