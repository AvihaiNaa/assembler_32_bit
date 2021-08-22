#ifndef _TABLE_H
#define _TABLE_H

/** Enumerator of symbol type */
typedef enum _symbol_attribute {
	CODE_SYMBOL,
	DATA_SYMBOL,
	EXTERNAL_SYMBOL,
	EXTERNAL_REFERENCE,
	ENTRY_SYMBOL
} _symbol_attribute;


typedef struct item* _symbol_table;

/** A single table elm */
typedef struct item {
	/** Address of the symbol */
	long value;
	/** Key - name */
	char* key;
	/** Next item in table */
	_symbol_table next;
	/** Symbol type */
	_symbol_attribute atrribute;
} table_items;



/*
 * Adds an entry to the symbols table
 * @param the symbol table
 * @param the key to add
 * @Return the type of the symbol
 */
void add_var_to_value_field(_symbol_table tab, long to_add, _symbol_attribute atrr);
/*
 * Returns all of the table items of the specified type
 * @param the symbol table
 * @param type of symbol table(attribute)
 * @Return symbol table
 */
_symbol_table get_table_by_symbol_type(_symbol_table tab, _symbol_attribute tyatrrpe);
/*
 * Free memory of the table
 * @param the symbol table
 * @Return nothing
 */
void make_symbol_table_free(_symbol_table tab);
/*
 * Returns a table of all the symbols of the type
 * @param the symbol table
 * @param the key
 * @param the amount of symbols
 * @return the extracted sub-table
 */
table_items* find_according_types(_symbol_table tab, char* key, int symbol_count, ...);
/*
 * Adds a key to the table
 * @param the symbol table
 * @param the key to add
 * @param the value to add
 * @param the type of the symbol
 */
void add_new_item_to_symbol_table(_symbol_table* tab, char* key, long value, _symbol_attribute type);



#endif