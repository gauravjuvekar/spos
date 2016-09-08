#ifndef ASM1_H
#define ASM1_H 1

#include "asm.h"

typedef int Address;

#define ADDRESS_UNRESOLVED ((Address)(-1))
typedef struct Symbol {
	char spec[MAX_TOKEN_LEN];
	Address address;
} Symbol;

typedef struct Literal {
	char spec[MAX_TOKEN_LEN];
	Address address;
	int val;
} Literal;

typedef struct IntermediateCode {
	Address address;
	MnemonicClass class;
	int opcode;
	int op1_val;
	char op2_unresolved[MAX_TOKEN_LEN];
} IntermediateCode;

int literal_table_lookup_literal(const void *literal, const void *entry);
int symbol_table_lookup_symbol(const void *symbol, const void *entry);

#endif /* ifndef ASM1_H */
