#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "table.h"
#include "asm.h"
#include "asm1.h"
#include "asm2.h"


#ifndef DEBUG
#define DEBUG 0
#endif

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
	printf("Error: %s|%s|%s|%s: %s\n",
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

int literal_table_lookup_val(const void *literal, const void *entry) {
	return *((int *)literal) - ((Literal *)entry)->val ;
}

void emit_interim_code(const IntermediateCode *interim_code, FILE* file) {
	fwrite(interim_code, sizeof(*interim_code), 1, file);
#if DEBUG
	printf("IC >> : %d) %d %d |%s|%d\n",
	       interim_code->address,
	       interim_code->opcode,
	       interim_code->op1_val,
		   IC_OP2_STR[interim_code->op2_type],
		   interim_code->op2.constant);
#endif
}

int main(__attribute__((unused))int argc, char *argv[]) {
	char *line = NULL;
	size_t linelen = 0;
	ssize_t read;

	assert(argc == 2);
	FILE *interim_code_file = fopen(argv[1], "wb");
	assert(interim_code_file != NULL);

	AsmSrcLine asm_src_line;
	Table symbol_table;
	table_init(&symbol_table, sizeof(Symbol));

	Table literal_table;
	table_init(&literal_table, sizeof(Literal));

	Address location_counter = 0;
	bool end = false;
	bool start = false;
	size_t pool_start = 0;

	while (!end && (read = getline(&line, &linelen, stdin)) != -1) {
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
		else {
#if DEBUG
		printf("In << : %s|%s|%s|%s\n",
			   asm_src_line.label,
			   asm_src_line.mnemonic,
			   asm_src_line.operand1,
			   asm_src_line.operand2);
#endif
		}

		Instruction *instruction = table_find(&instruction_table,
		                                      asm_src_line.mnemonic,
											  operand_table_lookup_mnemonic);
		if (instruction->n_max_operands < 2) {
			strcpy(asm_src_line.operand2, asm_src_line.operand1);
		}
		/* Previous validations ensure this */
		assert(instruction != NULL);
		IntermediateCode interim_code;

		if (asm_src_line.label[0] != '\0') {
			Symbol symbol = {
				.spec="",
				.address=location_counter,
				.is_alias=false
			};
			strcpy(symbol.spec, asm_src_line.label);

			int existing_index = table_index(
				&symbol_table, symbol.spec, symbol_table_lookup_spec);
			if (existing_index == -1) {
				existing_index = table_insert(&symbol_table, &symbol);
			}
			else {
				Symbol *existing = table_get(&symbol_table, existing_index);
				if (existing->address != ADDRESS_UNRESOLVED) {
					asm_src_line.error = ASM_SRC_SYMBOL_REDEFINED;
					handle_error(&asm_src_line);
				}
				else {
					*existing = symbol;
				}
			}
		}

		switch (instruction->class) {
			case MNEMONIC_CLASS_IS:
				interim_code.address = location_counter;
				interim_code.class = MNEMONIC_CLASS_IS;
				interim_code.opcode = instruction->info.opcode;

				if (instruction->op_type == OP_TYPE_REGISTER ||
					instruction->op_type == OP_TYPE_CONDITION) {

					Operand *operand = table_find(
						&operand_table,
						asm_src_line.operand1,
						operand_table_lookup_mnemonic);
					assert(operand != NULL);
					interim_code.op1_val = operand->infoval;
					if (is_literal(asm_src_line.operand2)) {
						Literal literal = {
							.val=get_literal_value(asm_src_line.operand2),
							.address=ADDRESS_UNRESOLVED
						};
						int literal_index = table_index_from(
							&literal_table, &(literal.val),
							literal_table_lookup_val, pool_start);
						if (literal_index == -1) {
							literal_index = table_insert(
								&literal_table, &literal);
						}
						interim_code.op2_type = IC_OP2_TYPE_LITERAL_INDEX;
						interim_code.op2.literal_index = literal_index;
					}
					else {
						/* symbol */
						Symbol symbol = {
							.spec="",
							.address=ADDRESS_UNRESOLVED,
							.is_alias=false
						};
						strcpy(symbol.spec, asm_src_line.operand2);

						int symbol_index = table_insert_or_ignore(
							&symbol_table, &symbol, symbol_table_lookup_symbol);
						interim_code.op2_type = IC_OP2_TYPE_SYMBOL_INDEX;
						interim_code.op2.symbol_index = symbol_index;
					}
				}
				else if (instruction->op_type == OP_TYPE_SYMBOL) {
					interim_code.op1_val = 0;
					Symbol symbol = {
						.spec="",
						.address=ADDRESS_UNRESOLVED,
						.is_alias=false
					};
					strcpy(symbol.spec, asm_src_line.operand2);

					int symbol_index = table_insert_or_ignore(
						&symbol_table, &symbol, symbol_table_lookup_symbol);
					interim_code.op2_type = IC_OP2_TYPE_SYMBOL_INDEX;
					interim_code.op2.symbol_index = symbol_index;
				}

				location_counter += instruction->length;
				emit_interim_code(&interim_code, interim_code_file);
				break;
			case MNEMONIC_CLASS_AD:
				if (!strcmp(instruction->mnemonic, "START") ||
				    !strcmp(instruction->mnemonic, "ORIGIN")) {
					if (!strcmp(instruction->mnemonic, "START")) {
						start = true;
					}
					location_counter = get_constant_value(
						asm_src_line.operand1);
				}
				else if (!strcmp(instruction->mnemonic, "EQU")) {
					Symbol symbol = {
						.spec="",
						.address=ADDRESS_UNRESOLVED,
						.is_alias=false
					};
					strcpy(symbol.spec, asm_src_line.label);

					int existing_index = table_index(
						&symbol_table, symbol.spec, symbol_table_lookup_spec);
					int target_index = table_index(
						&symbol_table, asm_src_line.operand2,
						symbol_table_lookup_spec);
					/* Already inserted and resolved as a label */
					assert(existing_index != -1);
					assert(((Symbol *)table_get(
						&symbol_table, existing_index))->address !=
						ADDRESS_UNRESOLVED);

					if (target_index == -1) {
						Symbol symbol = {
							.spec="",
							.address=ADDRESS_UNRESOLVED,
							.is_alias=false
						};
						strcpy(symbol.spec, asm_src_line.operand2);
						target_index = table_insert(&symbol_table, &symbol);
					}
					Symbol *existing = (
						(Symbol *)table_get(&symbol_table, existing_index));
					existing->alias_index = target_index;
					existing->is_alias = true;
				}
				else if (!strcmp(instruction->mnemonic, "LTORG") ||
				         !strcmp(instruction->mnemonic, "END")) {
					if (!strcmp(instruction->mnemonic, "LTORG")) {
						/* Should LTORG create a branch on its own? */

						/* interim_code.address = location_counter; */
						/* instruction = table_find( */
							/* &instruction_table, "BC", */
							/* instruction_table_lookup_mnemonic); */
						/* interim_code.opcode = instruction->info.opcode; */
						/* interim_code.class = MNEMONIC_CLASS_IS; */
						/* interim_code.op1_val = ((Operand *)table_find( */
							/* &operand_table, "ANY", */
							/* operand_table_lookup_mnemonic))->infoval; */
						/* interim_code.op2_type = IC_OP2_TYPE_RESOLVED_ADDRESS; */
						/* interim_code.op2.resolved_address = ( */
							/* location_counter + 1 + */
							/* (literal_table.n_entries - pool_start)); */
						/* emit_interim_code(&interim_code, interim_code_file); */
					}
					interim_code.opcode = 0;
					interim_code.op1_val = 0;
					interim_code.class = MNEMONIC_CLASS_DS;
					interim_code.op1_val = 0;
					interim_code.op2_type = IC_OP2_TYPE_CONSTANT;

					location_counter += instruction->length;

					for (;
					     pool_start < literal_table.n_entries;
						 pool_start++) {
						interim_code.address = location_counter;
						Literal *literal = table_get(
							&literal_table, pool_start);
						literal->address = interim_code.address;
						interim_code.op2.constant = literal->val;
						emit_interim_code(&interim_code, interim_code_file);
						location_counter += 1;
					}
					if (!strcmp(instruction->mnemonic, "END")) {
						location_counter = -1;
						end = true;
					}
				}
				break;
			case MNEMONIC_CLASS_DS:
				if (!strcmp(instruction->mnemonic, "DS")) {
					interim_code.opcode = instruction->info.declaration;
					interim_code.class = MNEMONIC_CLASS_DS;
					interim_code.op2_type = IC_OP2_TYPE_CONSTANT;
					interim_code.op2.constant = get_constant_value(
						asm_src_line.operand1);
					interim_code.op1_val = interim_code.op2.constant;

					location_counter += interim_code.op2.constant;
				}
				else {
					/* DC */
					interim_code.class = MNEMONIC_CLASS_DS;
					interim_code.op2.constant = get_literal_value(
						asm_src_line.operand1);
					interim_code.opcode = instruction->info.declaration;
					interim_code.op1_val = 0;
					interim_code.op2_type = IC_OP2_TYPE_CONSTANT;
					location_counter += 1;
				}
				emit_interim_code(&interim_code, interim_code_file);
				break;
		}
	}
	fclose(interim_code_file);
	interim_code_file = fopen(argv[1], "rb");
#if DEBUG
	int literal_index = 0;
	printf("--------------------Literal table\n");
	for (; literal_index < (int)literal_table.n_entries; literal_index++) {
		Literal *literal = table_get(&literal_table, literal_index);
		printf("[%d] %d %d\n", literal_index, literal->val, literal->address);
	}

	int symbol_index = 0;
	printf("--------------------Symbol table\n");
	for (; symbol_index < (int)symbol_table.n_entries; symbol_index++) {
		Symbol *symbol = table_get(&symbol_table, symbol_index);
		printf("[%d] %s %d %d\n",
		       symbol_index, symbol->spec, symbol->address, symbol->is_alias);
	}
	printf("--------------------ASM OUTPUT\n");
#endif
	pass_2(&symbol_table, &literal_table, interim_code_file, stdout);
}
