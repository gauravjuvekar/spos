#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "table.h"
#include "asm.h"
#include "asm1.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#if !DEBUG
#define NDEBUG 1
#endif

#include <assert.h>

void emit_assembled_code(IntermediateCode *interim_code, FILE *output_file) {
	if (interim_code->op2_type == IC_OP2_TYPE_CONSTANT &&
	    interim_code->op1_val != 0) {
		const Address end_address =
			interim_code->address + interim_code->op1_val;
		for (; interim_code->address < end_address; interim_code->address++) {
			fprintf(output_file, "%03d)\n", interim_code->address);
		}
	}
	else  {
		assert(interim_code->op2_type == IC_OP2_TYPE_CONSTANT ||
		       interim_code->op2_type == IC_OP2_TYPE_RESOLVED_ADDRESS);
		fprintf(output_file,
				"%03d) +%02d %d %03d\n",
				interim_code->address,
				interim_code->opcode,
				interim_code->op1_val,
				interim_code->op2.resolved_address);
	}
}

bool pass_2(
	const Table *symbol_table,
	const Table *literal_table,
	FILE *interm_code_file,
	FILE *output_file) {

	bool fatal_error = false;

	IntermediateCode interim_code;
	while (!fatal_error &&
	       (fread(&interim_code,
	              sizeof(interim_code), 1,
	              interm_code_file) == 1)) {
		Literal *literal;
		Symbol *symbol;
		switch (interim_code.op2_type) {
			case IC_OP2_TYPE_CONSTANT:
				emit_assembled_code(&interim_code, output_file);
				break;
			case IC_OP2_TYPE_SYMBOL_INDEX:
				while (true) {
					symbol = table_get(symbol_table,
					                   interim_code.op2.symbol_index);
					assert(symbol != NULL);
					if (symbol->is_alias) {
						interim_code.op2.symbol_index = symbol->alias_index;
						continue;
					}
					else {
						if (symbol->address == ADDRESS_UNRESOLVED) {
							fatal_error = true;
							printf("Error: Undefined symbol %s\n",
							       symbol->spec);
						}
						else {
							interim_code.op2.resolved_address = symbol->address;
							interim_code.op2_type =
								IC_OP2_TYPE_RESOLVED_ADDRESS;
						}
						break;
					}
				}
				if (interim_code.op2_type == IC_OP2_TYPE_RESOLVED_ADDRESS) {
					emit_assembled_code(&interim_code, output_file);
				}
				break;
			case IC_OP2_TYPE_LITERAL_INDEX:
				literal = table_get(literal_table,
				                    interim_code.op2.literal_index);
				assert(literal->address != ADDRESS_UNRESOLVED);
				interim_code.op2.resolved_address = literal->address;
				interim_code.op2_type = IC_OP2_TYPE_RESOLVED_ADDRESS;
				emit_assembled_code(&interim_code, output_file);
				break;
			case IC_OP2_TYPE_RESOLVED_ADDRESS:
				emit_assembled_code(&interim_code, output_file);
				break;
		}
	}
	return !fatal_error;
}
