#ifndef _UTILITY_H
#define _UTILITY_H

#include "global.h"
#include <ctype.h>

/*
 * Concatenates both string to a new allocated memory
 * @param str0 The first string
 * @param str1 The second string
 * @return a pointer to the new, allocated string
 */
char* string_concat(char* str0, char* str1);

/*
 * Allocates memory in the required size.
 * @param size The size to allocate in bytes
 * @return a generic pointer to the allocated memory if succeeded, otherwise error
 */
void* safe_malloc(long size);
/*
 * Allocates memory in the required size.
 * @param size The size to allocate in bytes
 * @return a generic pointer to the allocated memory if succeeded, otherwise error
 */
status safe_open(FILE** file_ptr, char const* _FileName, char const* _Mode);

/*
 * Get instruction code by string name.
 * @param name of instruction
 * @return the instruction enumerator code type
 */
instruction get_instruction_by_name(char* name);
/*
 * Make code image data structure as memory free
 * @param code image 
 * @return nothing
 */
void free_code_image(asm_word * *code_image, long fic);
/*
 * Repeats if an error occurred during the symbol analysis attempt .Insert the icon into the second buffer.
 * @param current code line
 * @param symbol that collected
 * @return is label
 */
bool looking_for_labels(code_line line, char* symbol_dest);
/*
 * Check if the label is in the correct length + is alphanumeric
 * @param string of label
 * @return is correct label
 */
bool is_pass_name_validation(char* name);
/*
 * Check if string is alphanumeric
 * @param some string
 * @return is string alphanumeric
 */
bool is_string_alphanumeric(char* string);
/*
 * Is the symbol an empty string
 * @param some string
 * @return is symbol defined
 */
bool is_symbol_defined(char start);
/*
 * Finds the index in a line where the symbol end
 * @param current code line
 * @paramthe start index of symbol
 * @return the index termination of symbol/label
 */
int symbol_analyzer(code_line line, int i);
/*
 * Allocates memory for a stringe
 * @param current code line
 * @param index
 * @param symbol start index of the string
 * @return is successed
 */
int memory_allocator(code_line line, int i, char* symbol_dest);
/*
 * FInd the paramters of a command
 * @param command string
 * @param opcode
 * @param funct
 * @param the type of the command
 */
void get_opcode_funct_type(char* command_str, opcode * opcode_out, funct * funct_out, code_type * type_out);

/*
 * Extracs a sub string from a source string, using stard and end indicies
 * @param source string var
 * @param target string var
 * @param the start of string index
 * @param the end of string index
 */
void extract_sub_string(char* source, char* target, int start_index, int end_index);
/*
 * Finds the type of the code word, for the current instruction counter
 * @param global env of program
 * @param instruction counter of program
 * @return code type(R, J, I).
 */
code_type find_code_word(env * *global_env, long* ic);
/*
 * Find the opcode in the current line
 * @param global env of program
 * @param instruction counter of program
 * @return the opcode
 */
int find_opcode(env * *global_env, long*);
/*
 * Find the location of the label
 * @param global env of program
 * @param string label
 * @return the address of label
 */
long find_address_of_label(env * *global_env, char* label);

/** moves the index to the next place in string where the char isn't white */
#define SKIP_NON_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;


/** moves the index to the next place in string where the char is white */
#define SKIP_WHITE(string, index) \
        for (;string[(index)] &&  (!isspace(string[(index)])); (++(index)))\
        ;

#endif

