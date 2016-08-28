#ifndef TABLE_H
#define TABLE_H 1

#include <stddef.h>

typedef struct Table {
	void *entries;
	size_t entry_len;
	size_t n_entries;
	size_t malloced_len;
} Table;

typedef int (table_compare_entry)(const void *a, const void *b);

void table_init(Table *table, size_t entry_len);
void table_deinit(Table *table);
void table_insert(Table *table, void *entry);
int table_index(const Table *table, const void *search,
                table_compare_entry compare_function);
void *table_get(const Table *table, size_t index);
void table_remove(Table *table, size_t index);

#endif /* ifndef TABLE_H */
