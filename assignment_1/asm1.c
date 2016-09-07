#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "table.h"
#include "asm.h"

#define DEBUG 0

int main() {
	char *line = NULL;
	size_t linelen = 0;
	ssize_t read;

	AsmSrcLine asm_src_line;

	while ((read = getline(&line, &linelen, stdin)) != 1) {
		if (line[read - 1] == '\n') {
			line[read - 1] = '\0';
		}
		asm_src_line = parse_line(line);
		validate_AsmSrcLine(&asm_src_line);
		printf("%s|%s|%s|%s:error:%s\n",
		       asm_src_line.label,
		       asm_src_line.mnemonic,
		       asm_src_line.operand1,
		       asm_src_line.operand2,
		       ASM_SRC_ERROR_STR[asm_src_line.error]);
	}
}
