#define _POSIX_C_SOURCE 1
#include "asm.h"
#include "table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const Instruction instruction_list[] = {
	{.mnemonic="STOP",   .n_min_operands=0, .n_max_operands=0, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_NONE,      .info.opcode=0},
	{.mnemonic="ADD",    .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=1},
	{.mnemonic="SUB",    .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=2},
	{.mnemonic="MULT",   .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=3},
	{.mnemonic="MOVER",  .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=4},
	{.mnemonic="MOVEM",  .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=5},
	{.mnemonic="COMP",   .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=6},
	{.mnemonic="BC",     .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_CONDITION, .info.opcode=7},
	{.mnemonic="DIV",    .n_min_operands=2, .n_max_operands=2, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_REGISTER,  .info.opcode=8},
	{.mnemonic="READ",   .n_min_operands=1, .n_max_operands=1, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_SYMBOL,    .info.opcode=9},
	{.mnemonic="PRINT",  .n_min_operands=1, .n_max_operands=1, .requires_label=false, .length=1, .class=MNEMONIC_CLASS_IS, .op_type=OP_TYPE_SYMBOL,    .info.opcode=10},

	{.mnemonic="DC",     .n_min_operands=1, .n_max_operands=1, .requires_label=true,  .length=0, .class=MNEMONIC_CLASS_DS, .op_type=OP_TYPE_LITERAL,   .info.declaration=1},
	{.mnemonic="DS",     .n_min_operands=1, .n_max_operands=1, .requires_label=true,  .length=0, .class=MNEMONIC_CLASS_DS, .op_type=OP_TYPE_CONSTANT,  .info.declaration=2},

	{.mnemonic="START",  .n_min_operands=1, .n_max_operands=1, .requires_label=false, .length=0, .class=MNEMONIC_CLASS_AD, .op_type=OP_TYPE_CONSTANT,  .info.directive=1},
	{.mnemonic="END",    .n_min_operands=0, .n_max_operands=1, .requires_label=false, .length=0, .class=MNEMONIC_CLASS_AD, .op_type=OP_TYPE_SYMBOL,    .info.directive=2},
	{.mnemonic="ORIGIN", .n_min_operands=1, .n_max_operands=1, .requires_label=false, .length=0, .class=MNEMONIC_CLASS_AD, .op_type=OP_TYPE_SYMBOL,    .info.directive=3},
	{.mnemonic="EQU",    .n_min_operands=1, .n_max_operands=1, .requires_label=true,  .length=0, .class=MNEMONIC_CLASS_AD, .op_type=OP_TYPE_SYMBOL,    .info.directive=4},
	{.mnemonic="LTORG",  .n_min_operands=0, .n_max_operands=0, .requires_label=false, .length=0, .class=MNEMONIC_CLASS_AD, .op_type=OP_TYPE_NONE,      .info.directive=5},
};

const Table instruction_table = {
	.entries=(void *)&instruction_list[0],
	.entry_len=sizeof(instruction_list[0]),
	.n_entries=sizeof(instruction_list) / sizeof(instruction_list[0]),
	.malloced_len=sizeof(instruction_list)
};
int instruction_table_lookup_mnemonic(const void *search,
                                      const void *table_term) {
	return strcmp(search, ((Instruction *)table_term)->mnemonic);
}

const Operand operand_list[] = {
	{.mnemonic="AREG", .infoval=1, .type=OP_TYPE_REGISTER},
	{.mnemonic="BREG", .infoval=2, .type=OP_TYPE_REGISTER},
	{.mnemonic="CREG", .infoval=3, .type=OP_TYPE_REGISTER},
	{.mnemonic="DREG", .infoval=4, .type=OP_TYPE_REGISTER},
	{.mnemonic="LT",   .infoval=1, .type=OP_TYPE_CONDITION},
	{.mnemonic="LE",   .infoval=2, .type=OP_TYPE_CONDITION},
	{.mnemonic="EQ",   .infoval=3, .type=OP_TYPE_CONDITION},
	{.mnemonic="GT",   .infoval=4, .type=OP_TYPE_CONDITION},
	{.mnemonic="GE",   .infoval=5, .type=OP_TYPE_CONDITION},
	{.mnemonic="ANY",  .infoval=6, .type=OP_TYPE_CONDITION}
};
const Table operand_table = {
	.entries=(void *)&operand_list[0],
	.entry_len=sizeof(operand_list[0]),
	.n_entries=sizeof(operand_list) / sizeof(operand_list[0]),
	.malloced_len=sizeof(operand_list)
};
int operand_table_lookup_mnemonic(const void *search, const void *table_term) {
	return strcmp(search, ((Operand  *)table_term)->mnemonic);
}

static int get_tokens(char tokens[static MAX_TOKENS_PER_LINE][32],
					  char *line) {

	char *token;
	char *saveptr;
	int n_tokens;
	for (n_tokens = 0;
	     n_tokens < MAX_TOKENS_PER_LINE;
	     n_tokens++, line = NULL) {
		token = strtok_r(line, ", ", &saveptr);
		if (token == NULL) {
			break;
		}
		strcpy(tokens[n_tokens], token);
	}
	if (token != NULL) {
		token = strtok_r(NULL, ", ", &saveptr);
	}
	if (token != NULL) {
		return MAX_TOKENS_PER_LINE + 1;
	}
	else {
		return n_tokens;
	}
}


AsmSrcLine parse_line(char *line) {
	AsmSrcLine ret = {
		.label="",
		.mnemonic="",
		.operand1="",
		.operand2="",
		.error=ASM_SRC_OK
	};
	char tokens[MAX_TOKENS_PER_LINE][MAX_TOKEN_LEN] = {"", "", "", ""};

	const int n_tokens = get_tokens(tokens, line);

	if (n_tokens > MAX_TOKENS_PER_LINE) {
		ret.error = ASM_SRC_TRAILING_CHARACTERS;
		return ret;
	}
	if (n_tokens == 0) {
		ret.error = ASM_SRC_BLANK_LINE;
		return ret;
	}

	Instruction *instruction = NULL;
	int instruction_alignment = -1;

	if (n_tokens > 1) {
		instruction = table_find(&instruction_table, tokens[1],
								 instruction_table_lookup_mnemonic);
		if (instruction != NULL) {
			instruction_alignment = 1;
		}
	}
	if (instruction_alignment == -1) {
		instruction = table_find(&instruction_table, tokens[0],
								 instruction_table_lookup_mnemonic);
		if (instruction != NULL) {
			instruction_alignment = 0;
		}
	}

	if (instruction_alignment == -1) {
		instruction_alignment = 0;
	}
	if (instruction_alignment == 1) {
		strcpy(ret.label, tokens[0]);
	}
	strcpy(ret.mnemonic, tokens[instruction_alignment]);
	if (n_tokens > instruction_alignment + 1) {
		strcpy(ret.operand1, tokens[instruction_alignment + 1]);
	}
	if (n_tokens > instruction_alignment + 2) {
		strcpy(ret.operand2, tokens[instruction_alignment + 2]);
	}
	return ret;
}


void validate_AsmSrcLine(AsmSrcLine *src_line) {
	if (src_line->error != ASM_SRC_OK) {
		return;
	}

	/* Check if a label or operand is an instruction */
	if (table_find(&instruction_table, src_line->label,
				   instruction_table_lookup_mnemonic)) {
		src_line->error = ASM_SRC_RESERVED_KEYWORD_LABEL;
		return;
	}
	else if (table_find(&instruction_table, src_line->operand1,
					   instruction_table_lookup_mnemonic)) {
		src_line->error = ASM_SRC_RESERVED_KEYWORD_OPERAND_1;
		return;
	}
	else if (table_find(&instruction_table, src_line->operand2,
					   instruction_table_lookup_mnemonic)) {
		src_line->error = ASM_SRC_RESERVED_KEYWORD_OPERAND_2;
		return;
	}

	Instruction *instruction;
	instruction = table_find(&instruction_table, src_line->mnemonic,
	                         instruction_table_lookup_mnemonic);
	if (instruction == NULL) {
		src_line->error = ASM_SRC_INVALID_INSTRUCTION;
		return;
	}

	if (instruction->requires_label && src_line->label[0] == '\0') {
		src_line->error = ASM_SRC_MISSING_LABEL;
		return;
	}

	/* Check number of operands */
	if ((instruction->n_min_operands > 0 && src_line->operand1[0] == '\0') ||
	    (instruction->n_min_operands > 1 && src_line->operand2[0] == '\0')) {
		src_line->error = ASM_SRC_MISSING_OPERAND;
		return;
	}
	if ((instruction->n_max_operands < 1 && src_line->operand1[0] != '\0') ||
	    (instruction->n_max_operands < 2 && src_line->operand2[0] != '\0')) {
		src_line->error = ASM_SRC_EXTRA_OPERAND;
		return;
	}
}

const char *ASM_SRC_ERROR_STR[] = {
	[ASM_SRC_OK]="OK",
	[ASM_SRC_INVALID_INSTRUCTION]="invalid instruction",
	[ASM_SRC_MISSING_OPERAND]="missing operand",
	[ASM_SRC_EXTRA_OPERAND]="extra operand",
	[ASM_SRC_MISSING_LABEL]="missing label",
	[ASM_SRC_RESERVED_KEYWORD_LABEL]="label cannot be a reserved keyword",
	[ASM_SRC_RESERVED_KEYWORD_OPERAND_1]="operand 1 cannot be a reserved keyword",
	[ASM_SRC_RESERVED_KEYWORD_OPERAND_2]="operand 2 cannot be a reserved keyword",
	[ASM_SRC_TRAILING_CHARACTERS]="trailing characters",
	[ASM_SRC_BLANK_LINE]="blank line",
	[ASM_SRC_INVALID_OPERAND]="invalid operand",
	[ASM_SRC_SYMBOL_REDEFINED]="redefinition of symbol"
};
