#ifndef TABLE_H
#define TABLE_H 1

#include <stddef.h>

typedef struct Table {
	void *entries;
	size_t entry_len;
	size_t n_entries;
	size_t malloced_len;
} Table;

typedef int (table_compare_entry)(const void *search, const void *table_term);

void table_init(Table *table, size_t entry_len);
void table_deinit(Table *table);
int table_insert(Table *table, const void *entry);
int table_update_or_insert(Table *table, const void *entry,
                           table_compare_entry compare_function);
int table_index_from(const Table *table, const void *search,
				     table_compare_entry compare_function,
				     size_t index);
int table_index(const Table *table, const void *search,
                table_compare_entry compare_function);
void *table_find_from(const Table *table, const void *search,
				      table_compare_entry compare_function,
				      size_t index);
void *table_find(const Table *table, const void *search,
				 table_compare_entry compare_function);
void *table_get(const Table *table, size_t index);
void table_remove(Table *table, size_t index);
int table_insert_or_ignore(Table *table, const void *entry,
                           table_compare_entry compare_function);

#endif /* ifndef TABLE_H */
