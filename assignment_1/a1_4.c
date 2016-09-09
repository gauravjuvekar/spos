#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "table.h"

#define DEBUG 0

typedef struct Entry {
	char symbol[32];
	char value[32];
} Entry;

typedef enum MenuChoice {
	MENU_EXIT = 0,
	MENU_LOOKUP = 1,
	MENU_UPDATE_OR_INSERT = 2,
} MenuChoice;

int compare_entry_symbol(const void *a, const void *b) {
	return strcmp(((Entry*)a)->symbol, ((Entry*)b)->symbol);
}

int compare_entry_value(const void *a, const void *b) {
	return strcmp(((Entry*)a)->value, ((Entry*)b)->value);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		exit(1);
	}
	FILE *input = fopen(argv[1], "r");
	if (input == NULL) {
		fprintf(stderr, "Unable to open file %s\n", argv[1]);
		exit(1);
	}

	Table table;
	table_init(&table, sizeof(Entry));

	Entry entry;
	while (fscanf(input, "%s%s", entry.symbol, entry.value) != EOF) {
		table_update_or_insert(&table, &entry, compare_entry_symbol);
#if DEBUG
		printf("Insert (%s, %s)\n", entry.symbol, entry.value);
#endif
	}
	fclose(input);

	int choice;
	while (1) {
		printf("0 Exit\n");
		printf("1 Lookup from symbol\n");
		printf("2 Update or insert value\n");
		scanf("%d", &choice);

		int index;
		Entry *table_entry;
		FILE *output;
		switch ((MenuChoice)choice) {
			case MENU_EXIT:
				output = fopen(argv[1], "w");
				for (index = 0; index < (int)table.n_entries; ++index) {
					table_entry = table_get(&table, index);
					fprintf(output, "%-*s%*s\n",
					        40, table_entry->symbol,
					        80, table_entry->value);
				}
				fclose(output);
				return 0;
				break;
			case MENU_LOOKUP:
				scanf("%s", entry.symbol);
				index = table_index(&table, &entry, compare_entry_symbol);
				if (index < 0) {
					printf("Not found\n");
				}
				else {
					printf("%s\n", ((Entry*)table_get(&table, index))->value);
				}
				break;
			case MENU_UPDATE_OR_INSERT:
				scanf("%s%s", entry.symbol, entry.value);
				table_update_or_insert(&table, &entry,
				                       compare_entry_symbol);
				break;
			default:
				printf("No such option\n");
				break;
		}
	}
	return 0;
}
