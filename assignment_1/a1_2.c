#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "table.h"

#define DEBUG 0

typedef struct Entry {
	char name[32];
	char phno[32];
} Entry;

typedef enum MenuChoice {
	MENU_EXIT = 0,
	MENU_LOOKUP = 1,
	MENU_RLOOKUP = 2,
	MENU_UPDATE = 3,
} MenuChoice;

int compare_entry_name(const void *a, const void *b) {
	return strcmp(((Entry*)a)->name, ((Entry*)b)->name);
}

int compare_entry_phno(const void *a, const void *b) {
	return strcmp(((Entry*)a)->phno, ((Entry*)b)->phno);
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
	while (fscanf(input, "%s%s", entry.name, entry.phno) != EOF) {
		table_insert(&table, &entry);
#if DEBUG
		printf("Insert (%s, %s)\n", entry.name, entry.phno);
#endif
	}
	fclose(input);

	int choice;
	while (1) {
		printf("0 Exit\n");
		printf("1 Lookup from name\n");
		printf("2 Reverse lookup from number\n");
		printf("3 Update number\n");
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
					        40, table_entry->name,
					        80, table_entry->phno);
				}
				fclose(output);
				return 0;
				break;
			case MENU_LOOKUP:
				scanf("%s", entry.name);
				index = table_index(&table, &entry, compare_entry_name);
				if (index < 0) {
					printf("Not found\n");
				}
				else {
					printf("%s\n", ((Entry*)table_get(&table, index))->phno);
				}
				break;
			case MENU_RLOOKUP:
				scanf("%s", entry.phno);
				index = table_index(&table, &entry, compare_entry_phno);
				if (index < 0) {
					printf("Not found\n");
				}
				else {
					printf("%s\n", ((Entry*)table_get(&table, index))->name);
				}
				break;
			case MENU_UPDATE:
				scanf("%s%s", entry.name, entry.phno);
				index = table_index(&table, &entry, compare_entry_name);
				if (index < 0) {
					printf("Not found, inserting\n");
					table_insert(&table, &entry);
				}
				else {
					*((Entry *)table_get(&table, index)) = entry;
				}
				break;
			default:
				printf("No such option\n");
				break;
		}
	}
	return 0;
}
