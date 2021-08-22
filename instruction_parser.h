#ifndef _INSTRUCTION_PARSER_H
#define _INSTRUCTION_PARSER_H

#include "global.h"

/*
 * Returns the first instruction detected from the index in the string.
 * @param line The source string.
 * @param index The index to start looking from.
 * @return instruction_type indicates the detected instruction.
 */
instruction looking_for_instruction(code_line line, int* index);

/*
 * Processes a .asciz instruction from index of source line.
 * @param line The source line
 * @param the index
 * @param data_img The current data image
 * @param dc The current data counter
 * @return Whether succeeded
 */
status decrypt_asciz_instruction(code_line line, int index, data_word** data_img, long* dc, long* dc_address);

/*
 * Processes a .data instruction from index of source line.
 * @param line The source line
 * @param the index
 * @param data_img The data image
 * @param dc The current data counter
 * @return Whether succeeded
 */
status decrypt_data_instruction(code_line line, int index, data_word** data_img, long* dc, long* dc_address, instruction inst_type);

/*
 * Returns true if the instrucion is asciz or data and false otherwise
 * @param instruction type
 * @return is asciz or data and false otherwise
 */
bool is_asciz_or_data(instruction type);
/*
 * Returns true if the instrucion is asciz and false otherwise
 * @param instruction type
 * @return is asciz and false otherwise
 */
bool is_asciz_instruction(instruction type);
/*
 * Returns true if the instrucion is data and false otherwise
 * @param instruction type
 * @return is  data and false otherwise
 */
bool is_data_instruction(instruction type);


/*Represents instruction element*/
struct instruction_element {
	/*The name of instruction*/
	char* name;
	/*The instruction value*/
	instruction value;
};


/*All the instruction collection*/
static struct instruction_element
instructions_collection[] = {
{"asciz",	 ASCIZ_INSTRUCTION},
{"dw",		 DW_INSTRUCTION},
{"db",		 DB_INSTRUCTION},
{"dh",		 DH_INSTRUCTION},
{"entry",    ENTRY_INSTRUCTION},
{"extern",   EXTERN_INSTRUCTION},
{NULL,	     UNDEFINED_INSTRUCTION}
};

#endif