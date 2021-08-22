#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility.h"
#include "global.h"
#include "status_handler.h"
#include "lang.h"

/**
 * Check if the label is legal or not
 * @param current code line
 * @param char potential label terminator
 * @return symbol that collected
 */
bool label_validation_checker(code_line line, char curr_char, char* symbol_dest);
/**
 * Check if a char is one of the saved words of the assembler
 * @param string input that represent as part of words bank
 * @return is belongs to words bank(command, instruction, etc.) 
 */
bool is_belongs_to_words_bank(char* name);



/*Concatenates both string to a new allocated memory*/
char* string_concat(char* str0, char* str1) {
	char* str = (char*)safe_malloc(strlen(str0) + strlen(str1) + 1);
	strcpy(str, str0);
	strcat(str, str1);
	return str;
}

/*Malloc with a message in case of an error*/
void * safe_malloc(long size) {
	void* ptr = malloc(size);
	if (ptr == NULL) {
		printf("Error: Memory allocation failed.");
		exit(1);
	}
	return ptr;
}

/*File open with checking of whether succfull or not*/
status safe_open(FILE** file_ptr, char const* _FileName, char const* _Mode) {
	*file_ptr = fopen(_FileName, _Mode);
	if ((*file_ptr) == NULL) {
		printf("Error In %s: ", _FileName);
		free((char*)_FileName);
		return ERROR_FILE_READING;
	}
	return SUCCESS;
}


/*Repeats if an error occurred during the symbol analysis attempt .Insert the icon into the second buffer.*/ 
bool looking_for_labels(code_line line, char* symbol_dest) {
	int i = 0;

	/* In any case skip white characters at the beginning */ 
	SKIP_NON_WHITE(line.content, i)
	
	/* Allocate  memory to the string */
	 i = memory_allocator(line, i, symbol_dest);

	return label_validation_checker(line, line.content[i], symbol_dest);
}

/*Is the symbol an empty string?*/
bool is_symbol_defined(char start) {
	return start != END_OF_STRING;
}

/*Allocates memory for a code_line variable*/
int memory_allocator(code_line line, int i, char* symbol_dest){
	int j = 0;
	/* Allocate memory to the string */
	for (; line.content[(i)] && line.content[(i)] != ':' && line.content[(i)] != EOF && i <= MAX_CHARS_SINGLE_LINE; i++, j++) {
		symbol_dest[j] = line.content[(i)];
	}
	symbol_dest[j] = END_OF_STRING; /* Close the string */
	return i;
}

/*Finds the index in a line where the symbol end*/
int symbol_analyzer(code_line line, int i) { 
	/* if symbol detected, start analyzing from it is deceleration end */
	for (; line.content[i] != ':'; i++);
	i++;
	return i;
}

/*Check if 2 chars are equal*/
bool char_to_char_compare(char c0, char c1) {
	return c0 == c1;
}

/*Check if the label is legal or not*/
bool label_validation_checker(code_line line, char curr_char , char* symbol_dest) {
	/* If it was an attempt to set a label, printing errors if necessary */
	if (char_to_char_compare(curr_char, LABEL_TERMINATOR)) {
		/*The label is 2 long*/
		if (!is_pass_name_validation(symbol_dest)) {
			error_printer(line,
				"Invalid label name - Label larger than 32 characters.");
			symbol_dest[0] = END_OF_STRING;
			return true; /* No valid symbol */
		}
		return false;
	}
	symbol_dest[0] = END_OF_STRING;
	return false; /* no errors */
}

/*Check if the label is in the correct length + is alphanumeric*/
bool is_pass_name_validation(char* name) {
	int name_len = strlen(name);
	bool _alpha = isalpha(name[0]);
	return name[0] && name_len <= 31 && _alpha && is_string_alphanumeric(name + 1) &&
		!is_belongs_to_words_bank(name);
}

/*Check if string is alphanumeric*/
bool is_string_alphanumeric(char* string) {
	int i;
	/*check  if it is non alphanumeric char*/
	for (i = 0; string[i]; i++) {
		if (!isalpha(string[i]) && !isdigit(string[i])) return false;
	}
	return true;
}

/*Check if a char is one of the saved words of the assembler*/
bool is_belongs_to_words_bank(char* name) {
	int _funct, _opcode, _type;
	/* check if register or command */
	get_opcode_funct_type(name, &_opcode, (funct*)&_funct, (code_type*)&_type);
	return (_opcode != UNDEFINED_OPCODE || get_register(name) != UNDEFINED_REG || get_instruction_by_name(name) != UNDEFINED_INSTRUCTION);
}

/*Extracs a sub string from a source string, using stard and end indicies*/
void extract_sub_string(char* source, char* target, int start_index, int end_index) {
	int i, j;
	for (i = start_index, j = 0; i < end_index; i++, j++)
	{
		*(target + j) = *(source + i);
	}
	target[j] = END_OF_STRING;
}

/*Finds the type of the code word, for the current ic*/
code_type find_code_word(env** global_env, long* ic)
{
	if (((*global_env)->code_memory_image[(*ic)]->word.code->type) == R)
	{
		return R;
	}
	if (((*global_env)->code_memory_image[(*ic)]->word.code->type) == I)
	{
		return I;
	}
	if (((*global_env)->code_memory_image[(*ic)]->word.code->type) == J)
	{
		return J;
	}
	return UNDEFINDED_CODE;
}

/*Finds the opcode of the code word, for the current ic*/
int find_opcode(env** global_env, long* ic)
{
	switch (find_code_word(global_env, (ic)))
	{
	case R:
		return ((*global_env)->code_memory_image[(*ic)]->word.code->code_word_type.r_code->opcode);
		break;
	case I:
		return ((*global_env)->code_memory_image[(*ic)]->word.code->code_word_type.i_code->opcode);
		break;
	case J:
		return ((*global_env)->code_memory_image[(*ic)]->word.code->code_word_type.j_code->opcode);
		break;
	default:
		break;
	}
	/*Error opcode*/
	return UNDEFINDED_CODE;
}

/*Gets a label name, and returns it's address in the symbol table*/
long find_address_of_label(env** global_env, char* label)
{
	_symbol_table temp_node = (*global_env)->symbol_table;
	while (temp_node != NULL)
	{
		/*When symbols are equal*/
		if (!strcmp(temp_node->key, label))
		{
			return temp_node->value;
		}
		temp_node = temp_node->next;
	}
	/*Not exist*/
	return -1;
}

/*Free space of the code image*/
void free_code_image(asm_word** code_image, long fic) {
	long i;
	for (i = 0; i < fic - IC_DEFAULT_VAL; i++) {
		asm_word* curr_word = code_image[i];
		if (curr_word != NULL) {
			/* free code word */
			if (curr_word->length > 0) {
				free(curr_word->word.code);
			}
			/* free curr_word  */
			free(curr_word);
			code_image[i] = NULL;
		}
	}
	/*is collection doesn't exist*/
	if (commands_collection == NULL) return;
}

