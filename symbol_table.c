#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "symbol_table.h"
#include "utility.h"
#include "status_handler.h"

/*
 * Returns a table of all the symbols of the type
 */
table_items* find_according_types(_symbol_table this_table, char* key, int symbol_count, ...) {
	int i;
	/*Malloc*/
	_symbol_attribute* all_symbol_kinds = safe_malloc((symbol_count) * sizeof(int));
	va_list arglist;
	va_start(arglist, symbol_count);
	/*Add values according to the instructions*/
	for (i = 0; i < symbol_count; i++) {
		all_symbol_kinds[i] = va_arg(arglist, _symbol_attribute);
	}
	va_end(arglist);
	/*When table is empty*/
	if (this_table == NULL) {
		free(all_symbol_kinds);
		return NULL;
	}
	do {
		/*Free memory when needed*/
		for (i = 0; i < symbol_count; i++) {
			if (all_symbol_kinds[i] == this_table->atrribute && strcmp(key, this_table->key) == 0) {
				free(all_symbol_kinds);
				return this_table;
			}
		}
	} while ((this_table = this_table->next) != NULL);
	free(all_symbol_kinds);
	/* Not found element */
	return NULL;
}

/*
 * Adds an item to the symbols table
 */
void add_var_to_value_field(_symbol_table my_table, long to_add, _symbol_attribute atrribute) {
	_symbol_table current_item;
	current_item = my_table;
	while (current_item != NULL) {
		if (current_item->atrribute == atrribute)
			current_item->value += to_add;
		current_item = current_item->next;
	}
}


/*Returns all of the table items of the specified type*/
_symbol_table get_table_by_symbol_type(_symbol_table this_table, _symbol_attribute attr) {
	_symbol_table my_table = NULL;
	do {
		if (this_table->atrribute == attr) {
			add_new_item_to_symbol_table(&my_table, this_table->key, this_table->value, this_table->atrribute);
		}
	} while ((this_table = this_table->next) != NULL);
	return my_table;
}

/*Free memory of the table*/
void make_symbol_table_free(_symbol_table this_table) {
	_symbol_table previous, current = this_table;
	while (current != NULL) {
		previous = current;
		current = current->next;
		free(previous->key);
		free(previous);
	}
}

/*
 * Adds a key to the table
 */
void add_new_item_to_symbol_table(_symbol_table* this_table, char* new_key, long new_val, _symbol_attribute attr) {
	_symbol_table _previous_item, _current_item, my_new_item;
	char* tempory_str;
	my_new_item = (_symbol_table)safe_malloc(sizeof(table_items));
	tempory_str = (char*)safe_malloc(strlen(new_key) + 1);
	strcpy(tempory_str, new_key);
	my_new_item->key = tempory_str;
	my_new_item->value = new_val;
	my_new_item->atrribute = attr;
	if ((*this_table) == NULL || (*this_table)->value > new_val) {
		my_new_item->next = (*this_table);
		(*this_table) = my_new_item;
		return;
	}
	/* Insert new element */
	_current_item = (*this_table)->next;
	_previous_item = *this_table;
	while (_current_item != NULL && _current_item->value < new_val) {
		_previous_item = _current_item;
		_current_item = _current_item->next;
	}
	my_new_item->next = _current_item;
	_previous_item->next = my_new_item;
}
