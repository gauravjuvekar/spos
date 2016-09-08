#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "table.h"
#include "asm.h"
#include "asm1.h"


#define DEBUG 0

#if !DEBUG
#define NDEBUG 1
#endif

#include <assert.h>

bool is_validate_operand(OperandType type,
                         const char operand[static MAX_TOKEN_LEN]) {
	if ((type == OP_TYPE_SYMBOL   && is_literal(operand)) ||
	    (type == OP_TYPE_CONSTANT && !is_valid_constant(operand)) ||
	    (type == OP_TYPE_LITERAL  && !is_valid_literal(operand))) {
		return false;
	}
	else if (type == OP_TYPE_REGISTER || type == OP_TYPE_CONDITION) {
		Operand *op;
		op = table_find(&operand_table, operand, operand_table_lookup_mnemonic);
		if (op == NULL || op->type != type) {
			return false;
		}
	}
	return true;
}

void validate_operands(AsmSrcLine *src_line) {
	if (src_line->error != ASM_SRC_OK) {
		return;
	}
	Instruction *instruction;
	instruction = table_find(&instruction_table, src_line->mnemonic,
				    		 instruction_table_lookup_mnemonic);

	/* We have already checked if the instruction is valid */
	assert(instruction != NULL);
	if (instruction->n_max_operands > 0)  {
		bool valid = is_validate_operand(instruction->op_type,
		                                 src_line->operand1);
		if (!((instruction->n_min_operands > 0 && valid) ||
		      (instruction->n_min_operands == 0 &&
		       (src_line->operand1[0] == '\0' || valid)))){
			src_line->error = ASM_SRC_INVALID_OPERAND;
			return;
		}
	}
	if (instruction->op_type == OP_TYPE_REGISTER ||
	    instruction->op_type == OP_TYPE_CONDITION) {
		if (!(is_validate_operand(OP_TYPE_LITERAL, src_line->operand2) ||
		      is_validate_operand(OP_TYPE_SYMBOL, src_line->operand2))) {
			src_line->error = ASM_SRC_INVALID_OPERAND;
			return;
		}
	}
}


void handle_error(AsmSrcLine *asm_src_line) {
	printf("%s|%s|%s|%s:error:%s\n",
		   asm_src_line->label,
		   asm_src_line->mnemonic,
		   asm_src_line->operand1,
		   asm_src_line->operand2,
		   ASM_SRC_ERROR_STR[asm_src_line->error]);
}

int symbol_table_lookup_spec(const void *spec, const void *entry) {
	return strcmp((char *)spec, ((Symbol *)entry)->spec);
}

int symbol_table_lookup_symbol(const void *symbol, const void *entry) {
	return strcmp(((Symbol  *)symbol)->spec, ((Symbol *)entry)->spec);
}

int literal_table_lookup_literal(const void *literal, const void *entry) {
	return strcmp((char *)literal, ((Literal *)entry)->spec);
}

int main(__attribute__((unused))int argc, char *argv[]) {
	char *line = NULL;
	size_t linelen = 0;
	ssize_t read;

	assert(argc == 2);
	FILE *intermediate_code_file = fopen(argv[1], "wb");
	assert(intermediate_code_file != NULL);

	AsmSrcLine asm_src_line;
	Table symbol_table;
	table_init(&symbol_table, sizeof(Symbol));

	Table literal_table;
	table_init(&literal_table, sizeof(Literal));

	Address location_counter = 0;

	while ((read = getline(&line, &linelen, stdin)) != 1) {
		if (line[read - 1] == '\n') {
			line[read - 1] = '\0';
		}
		asm_src_line = parse_line(line);
		validate_AsmSrcLine(&asm_src_line);
		validate_operands(&asm_src_line);
		if (asm_src_line.error != ASM_SRC_OK) {
			handle_error(&asm_src_line);
			continue;
		}

		Instruction *instruction = table_find(&instruction_table,
		                                      asm_src_line.mnemonic,
											  operand_table_lookup_mnemonic);
		if (instruction->n_max_operands < 2) {
			strcpy(asm_src_line.operand2, asm_src_line.operand1);
		}
		/* Previous validations ensure this */
		assert(instruction != NULL);
		IntermediateCode intermediate_code;
		size_t pool_start = 0;

		switch (instruction->class) {
			case MNEMONIC_CLASS_IS:
				intermediate_code.address = location_counter;
				intermediate_code.class = MNEMONIC_CLASS_IS;
				intermediate_code.opcode = instruction->info.opcode;
				if (instruction->op_type == OP_TYPE_REGISTER ||
					instruction->op_type == OP_TYPE_CONDITION) {

					Operand *operand = table_find(
						&operand_table,
						asm_src_line.operand1,
						operand_table_lookup_mnemonic);
					assert(operand != NULL);
					intermediate_code.op1_val = operand->infoval;
					strcpy(intermediate_code.op2_unresolved,
					       asm_src_line.operand2);
					if (is_literal(asm_src_line.operand2)) {
						Literal literal;
						strcpy(literal.spec, asm_src_line.operand2);
						literal.val = get_literal_value(literal.spec);
						literal.address = ADDRESS_UNRESOLVED;
						/* TODO take care of pools */
						table_insert(&literal_table, &literal);
					}
					else {
						Symbol symbol;
						strcpy(symbol.spec, asm_src_line.operand2);
						symbol.address = ADDRESS_UNRESOLVED;
						table_insert_or_ignore(&symbol_table, &symbol,
						                       symbol_table_lookup_symbol);
					}
				}
				else if (instruction->op_type == OP_TYPE_SYMBOL) {
					intermediate_code.op1_val = 0;
					strcpy(intermediate_code.op2_unresolved,
					       asm_src_line.operand2);
					Symbol symbol;
					strcpy(symbol.spec, asm_src_line.operand2);
					symbol.address = ADDRESS_UNRESOLVED;
					table_insert_or_ignore(&symbol_table, &symbol,
										   symbol_table_lookup_symbol);
				}

				location_counter += instruction->length;
				fwrite(&intermediate_code, sizeof(intermediate_code), 1,
				       intermediate_code_file);
				break;
			case MNEMONIC_CLASS_AD:
				if (!strcmp(instruction->mnemonic, "START") ||
				    !strcmp(instruction->mnemonic, "ORIGIN")) {
					location_counter = get_constant_value(
						asm_src_line.operand1);
				}
				else if (!strcmp(instruction->mnemonic, "END")) {
					location_counter = -1;
					/* TODO Implicit LTORG */
				}
				else if (!strcmp(instruction->mnemonic, "EQU")) {
					/* TODO */
				}
				else if (!strcmp(instruction->mnemonic, "LTORG")) {
					/* TODO Create jump statement */

					/* for (; */
						 /* pool_start < literal_table.n_entries; */
						 /* pool_start++, location_counter += 1) { */
						/* Literal *literal = table_get(&literal_table, */
													 /* pool_start); */
						/* intermediate_code.class=MNEMONIC_CLASS_AD; */
						/* intermediate_code.opcode = */


					/* } */
				}
				break;
			case MNEMONIC_CLASS_DS:
				if (!strcmp(instruction->mnemonic, "DS")) {
					Symbol symbol;
					symbol.address = location_counter;
					strcpy(symbol.spec, asm_src_line.label);

					intermediate_code.class = MNEMONIC_CLASS_DS;
					intermediate_code.op1_val = get_constant_value(
						asm_src_line.operand1);
					intermediate_code.opcode = instruction->info.declaration;
					Symbol *existing= table_find(&symbol_table, symbol.spec,
					                             symbol_table_lookup_spec);
					if (existing == NULL) {
						table_insert(&symbol_table, &symbol);
					}
					else {
						if (existing->address != ADDRESS_UNRESOLVED) {
							asm_src_line.error = ASM_SRC_SYMBOL_REDEFINED;
							handle_error(&asm_src_line);
						}
						else {
							*existing = symbol;
						}
					}
					location_counter += intermediate_code.op1_val;
				}
				else {
					/* DC */
					Symbol symbol;
					symbol.address = location_counter;
					strcpy(symbol.spec, asm_src_line.label);
					intermediate_code.class = MNEMONIC_CLASS_DS;

					intermediate_code.op1_val = get_literal_value(
						asm_src_line.operand1);
					intermediate_code.opcode = instruction->info.declaration;
					Symbol *existing= table_find(&symbol_table, symbol.spec,
					                             symbol_table_lookup_spec);
					if (existing == NULL) {
						table_insert(&symbol_table, &symbol);
					}
					else {
						if (existing->address != ADDRESS_UNRESOLVED) {
							asm_src_line.error = ASM_SRC_SYMBOL_REDEFINED;
							handle_error(&asm_src_line);
						}
						else {
							*existing = symbol;
						}
					}
					location_counter += 1;
				}
				fwrite(&intermediate_code, sizeof(intermediate_code), 1,
				       intermediate_code_file);
				break;
		}
	}
}
