#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "table.h"

void table_init(Table *table, size_t entry_len) {
	table->entry_len = entry_len;
	table->n_entries = 0;
	table->malloced_len = entry_len * 1;
	table->entries = malloc(table->malloced_len);
	if (table->entries == NULL) {
		perror("malloc");
	}
}

void table_deinit(Table *table) {
	free(table->entries);
	table->n_entries = 0;
	table->malloced_len = 0;
}

void table_insert(Table *table, const void *entry) {
	if (table->malloced_len < (table->entry_len * (table->n_entries + 1))) {
		table->entries = realloc(table->entries, table->malloced_len * 2);
		if (table->entries == NULL) {
			perror("malloc");
		}
		else {
			table->malloced_len *= 2;
		}
	}
	memcpy(table->entries + (table->entry_len * table->n_entries),
		   entry,
		   table->entry_len);
	table->n_entries += 1;
}

int table_index(const Table *table, const void *search,
				table_compare_entry compare_function) {
	size_t i = 0;
	for (i = 0; i < table->n_entries; ++i) {
		if (compare_function(search,
							 table->entries + (i * table->entry_len)) == 0) {
			return i;
		}
	}
	return -1;
}

void *table_get(const Table *table, size_t index) {
	return table->entries + (table->entry_len * index);
}

void *table_find(const Table *table, const void *search,
				 table_compare_entry compare_function) {
	int index = table_index(table, search, compare_function);
	if (index < 0) {
		return NULL;
	}
	else {
		return table_get(table, index);
	}
}

void table_remove(Table *table, size_t index) {
	memcpy(table->entries + (table->entry_len * index),
	       table->entries + (table->entry_len * table->n_entries),
		   table->entry_len);
	table->n_entries -= 1;
}

void table_update_or_insert(Table *table, const void *entry,
							table_compare_entry compare_function) {
	int index = table_index(table, entry, compare_function);
	if (index < 0) {
		table_insert(table, entry);
	}
	else {
		memcpy(table->entries + (table->entry_len * index),
		       entry,
			   table->entry_len);
	}
}
