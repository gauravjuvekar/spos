#ifndef ASM_H
#define ASM_H 1

#include "table.h"

#include <stdbool.h>

typedef int Address;

typedef enum MnemonicClass {
	MNEMONIC_CLASS_IS,
	MNEMONIC_CLASS_DS,
	MNEMONIC_CLASS_AD,
} MnemonicClass;

typedef struct Instruction {
	const char mnemonic[32];
	const MnemonicClass class;
	union {
		int opcode;
		int directive;
		int declaration;
	} info;
	const int n_min_operands;
	const int n_max_operands;
	const bool requires_label;
	const int length;
} Instruction;

typedef struct Operand {
	const char mnemonic[8];
	const int infoval;
} Operand;

typedef struct Symbol {
	char symbol[32];
	Address address;
	int length;
} Symbol;


typedef enum AsmSrcErrors {
	ASM_SRC_OK = 0,
	ASM_SRC_INVALID_INSTRUCTION,
	ASM_SRC_MISSING_OPERAND,
	ASM_SRC_EXTRA_OPERAND,
	ASM_SRC_MISSING_LABEL,
	ASM_SRC_RESERVED_KEYWORD_LABEL,
	ASM_SRC_RESERVED_KEYWORD_OPERAND_1,
	ASM_SRC_RESERVED_KEYWORD_OPERAND_2,
	ASM_SRC_TRAILING_CHARACTERS,
	ASM_SRC_BLANK_LINE
} AsmSrcErrors;

extern const char *ASM_SRC_ERROR_STR[];

typedef struct AsmSrcLine {
	char label[32];
	char mnemonic[32];
	char operand1[32];
	char operand2[32];
	AsmSrcErrors error;
} AsmSrcLine;

extern const Table instruction_table;
extern const Table operand_table;
int instruction_table_lookup_mnemonic(const void *search,
                                      const void *table_term);
int operand_table_lookup_mnemonic(const void *search, const void *table_term);


AsmSrcLine parse_line(char *line);
void validate_AsmSrcLine(AsmSrcLine *src_line);

#endif /* ifndef ASM_H */
