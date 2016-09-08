#ifndef ASM_H
#define ASM_H 1

#include "table.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TOKEN_LEN (32)
#define MAX_TOKENS_PER_LINE (4)

typedef enum MnemonicClass {
	MNEMONIC_CLASS_IS,
	MNEMONIC_CLASS_DS,
	MNEMONIC_CLASS_AD,
} MnemonicClass;

typedef enum OperandType {
	OP_TYPE_NONE,
	OP_TYPE_CONDITION,
	OP_TYPE_REGISTER,
	/* Appears as =<number> as operand 2 */
	OP_TYPE_LITERAL,
	/* Label */
	OP_TYPE_SYMBOL,
	/* Appears as <number>, mainly as size */
	OP_TYPE_CONSTANT
} OperandType;

typedef struct Instruction {
	const char mnemonic[MAX_TOKEN_LEN];
	const MnemonicClass class;
	union {
		int opcode;
		int directive;
		int declaration;
	} info;
	/* Operand type of the fixed operand */
	const OperandType op_type;
	const int n_min_operands;
	const int n_max_operands;
	const bool requires_label;
	const int length;
} Instruction;

typedef struct Operand {
	const char mnemonic[MAX_TOKEN_LEN];
	const int infoval;
	const OperandType type;
} Operand;


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
	ASM_SRC_BLANK_LINE,
	ASM_SRC_INVALID_OPERAND,
	ASM_SRC_SYMBOL_REDEFINED
} AsmSrcErrors;

extern const char *ASM_SRC_ERROR_STR[];

typedef struct AsmSrcLine {
	char label[MAX_TOKEN_LEN];
	char mnemonic[MAX_TOKEN_LEN];
	char operand1[MAX_TOKEN_LEN];
	char operand2[MAX_TOKEN_LEN];
	AsmSrcErrors error;
} AsmSrcLine;

extern const Table instruction_table;
extern const Table operand_table;
int instruction_table_lookup_mnemonic(const void *search,
                                      const void *table_term);
int operand_table_lookup_mnemonic(const void *search, const void *table_term);


AsmSrcLine parse_line(char *line);
void validate_AsmSrcLine(AsmSrcLine *src_line);

static inline bool is_valid_constant(const char operand[static MAX_TOKEN_LEN]) {
	const char *ch = operand;
	while (isdigit(*ch)) ch++;
	if (*ch == '\0') {
		return true;
	}
	return false;
}

static inline int get_constant_value(const char operand[static MAX_TOKEN_LEN]) {
	return atoi(operand);
}

static inline bool is_literal(const char operand[static MAX_TOKEN_LEN]) {
	return operand[0] == '=';
}

static inline bool is_valid_literal(const char operand[static MAX_TOKEN_LEN]) {
	return is_literal(operand) && is_valid_constant(&operand[1]);
}

static inline int get_literal_value(const char operand[static MAX_TOKEN_LEN]) {
	return get_constant_value(&operand[1]);
}

#endif /* ifndef ASM_H */
