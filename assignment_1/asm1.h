#ifndef ASM1_H
#define ASM1_H 1

#include "asm.h"

typedef int Address;

#define ADDRESS_UNRESOLVED ((Address)(-1))

typedef struct Symbol {
	char spec[MAX_TOKEN_LEN];
	union {
		Address address;
		int alias_index;
	};
	bool is_alias;
} Symbol;

typedef struct Literal {
	Address address;
	int val;
} Literal;

typedef enum IntermediateCodeOp2Type {
	IC_OP2_TYPE_LITERAL_INDEX,
	IC_OP2_TYPE_SYMBOL_INDEX,
	IC_OP2_TYPE_CONSTANT,
	IC_OP2_TYPE_RESOLVED_ADDRESS,
} IntermediateCodeOp2Type;

static const char *IC_OP2_STR[] = {
	[IC_OP2_TYPE_LITERAL_INDEX]="literal",
	[IC_OP2_TYPE_SYMBOL_INDEX]="symbol",
	[IC_OP2_TYPE_CONSTANT]="constant",
	[IC_OP2_TYPE_RESOLVED_ADDRESS]="resolved address"
};

typedef struct IntermediateCode {
	Address address;
	MnemonicClass class;
	int opcode;
	int op1_val;
	IntermediateCodeOp2Type op2_type;
	union {
		int literal_index;
		int symbol_index;
		int constant;
		Address resolved_address;
	} op2;
} IntermediateCode;

int literal_table_lookup_val(const void *literal, const void *entry);
int symbol_table_lookup_symbol(const void *symbol, const void *entry);

#endif /* ifndef ASM1_H */
