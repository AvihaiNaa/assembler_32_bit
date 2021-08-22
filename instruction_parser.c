#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility.h"
#include "global.h"
#include "instruction_parser.h"
#include "status_handler.h"

/*
 * Extracts the operands from the line (after data there may be infinite amount)
 * @param line The source line
 * @param The line number
 * @param the destination
 * @param The number of operands
 * @return Whether succeeded
 */
bool parse_data_operands(code_line current_line, int i, char** dest, int* number_of_operands);

/*Add values from the instruction to the data image
* @params the line
* @params the operands
* @params the number of operands
* @params the data image
* @params the value of dc
* @params the addess of dc
* @params the instruction
* @return the status
*/
status add_to_data_image(code_line line, char* operands[], int number_of_operands, data_word** data_img, long* dc, long* dc_address, instruction inst_type);


instruction looking_for_instruction(code_line line, int* index) {
	char temp[MAX_CHARS_SINGLE_LINE];
	int j;
	instruction result;

	/*Skip white chars*/
	SKIP_NON_WHITE(line.content, *index) 
		if (line.content[*index] != '.') return UNDEFINED_INSTRUCTION;

	/*Extract the instruction*/
	for (j = 0; line.content[*index] && line.content[*index] != '\t' && line.content[*index] != ' '; (*index)++, j++) {
		temp[j] = line.content[*index];
	}
	temp[j] = END_OF_STRING; 
	/*Find the instruction name and check if it is legal*/
	if ((result = get_instruction_by_name(temp + 1)) != UNDEFINED_INSTRUCTION) return result;
	error_printer(line, "Invalid instruction name: %s", temp);
	return ERROR_INSTRUCTION; 
}

status decrypt_asciz_instruction(code_line curr_line, int char_idx, data_word** data_img, long* dc, long* dc_address) {
	data_word* dataword;
	asciz_data* asciz;
	char asciz_tempory[MAX_CHARS_SINGLE_LINE];
	char* quote_state = strrchr(curr_line.content, '"');
	/*Skip white chars*/
	SKIP_NON_WHITE(curr_line.content, char_idx)
	 if (&curr_line.content[char_idx] == quote_state) {
		 error_printer(curr_line, "asciz quote need to be close");
		return ERR;
	}
	else {
			int i;
			/*Skip the "quotes" - and if it doesn't exist return an error*/
			if (curr_line.content[char_idx] != '"')
			{
				error_printer(curr_line, "No quotes after the .asciz");
				return ERR;
			}
			else
			{
				/*Skip the quotes*/
				char_idx++;
			}
			/*Copy the string*/
			for (i = 0; curr_line.content[char_idx] && curr_line.content[char_idx] != NEW_LINE && curr_line.content[char_idx] != NEW_LINE_UBUNTU &&
				curr_line.content[char_idx] != EOF && curr_line.content[char_idx] != '"'; char_idx++, i++) {
				asciz_tempory[i] = curr_line.content[char_idx];
			}
			/*Check fot "quotes" after the end - and if it doesn't exist return an error*/
			if (curr_line.content[char_idx] != '"')
			{
				error_printer(curr_line, "No quotes after the .asciz term");
				return ERR;
			}
			/*Add the string terminator */
			asciz_tempory[i] = END_OF_STRING;
			for (i = 0; asciz_tempory[i] && i<strlen(asciz_tempory); i++) {
				dataword = (data_word*)safe_malloc(sizeof(data_word));
				asciz = (asciz_data*)safe_malloc(sizeof(asciz_data));
				asciz->data = asciz_tempory[i];
				(dataword->data_word_type).asciz_data = asciz;
				(dataword->type) = ASCIZ_INSTRUCTION;
				data_img[*dc] = dataword;
				(*dc)++;
				(*dc_address)++;
			}
			/*Malloc*/
			dataword = (data_word*)safe_malloc(sizeof(data_word));
			asciz = (asciz_data*)safe_malloc(sizeof(asciz_data));
			/*Put string terminator*/
			asciz->data = END_OF_STRING;
			/*Actually store the string and it's type*/
			(dataword->data_word_type).asciz_data = asciz;
			(dataword->type) = ASCIZ_INSTRUCTION;
			data_img[*dc] = dataword;
			/*Increament*/
			(*dc)++;
			(*dc_address)++;
		}
	return SUCCESS;
}

status decrypt_data_instruction(code_line curr_line, int char_idx, data_word** data_img, long* dc, long* dc_address,  instruction inst_type) {
	char* operands[50];
	int number_of_operands;
	if (!parse_data_operands(curr_line, char_idx, operands, &number_of_operands)) {
		return ERR;
	}
	return add_to_data_image(curr_line, operands, number_of_operands, data_img, dc,  dc_address, inst_type);
}

/*
* Add values from the instruction to the data image
*/
status add_to_data_image(code_line line, char* operands[], int number_of_operands, data_word** data_img, long* dc, long* dc_address, instruction inst_type) {
	data_word* dataword;
	db_data_word* db;
	dh_data_word* dh;
	dw_data_word* dw;
	int temp_number = 0;
	int j;
	/*For different types of instruction there is a different storage method*/
	switch (inst_type) {
	/*.db*/
	case DB_INSTRUCTION:
		for (j = 0; j < number_of_operands; j++) {
			/*Create the code word using the data:*/
			dataword = (data_word*)safe_malloc(sizeof(data_word));
			db = (db_data_word*)safe_malloc(sizeof(db_data_word));
			/*sort of strcpy but with dc increment*/
			if (atoi(operands[j]) != 0 && strlen(operands[j]) <= MAX_DB_LENGTH)
				temp_number = atoi(operands[j]);
			else
				error_printer(line, "Error: operand isn't valid number");
			db->data = temp_number;
			(dataword->type) = DB_INSTRUCTION;
			(dataword->data_word_type).db_data = db;
			data_img[*dc] = dataword;
			(*dc)++;
			(*dc_address)++;			
		}
		break;
	/*.dw*/
	case DW_INSTRUCTION:
		for (j = 0; j < number_of_operands; j++) {
			/*Create the code word by the data:*/
			dataword = (data_word*)safe_malloc(sizeof(data_word));
			dw = (dw_data_word*)safe_malloc(sizeof(dw_data_word));
			/*sort of strcpy but with dc increment*/
			if (atoi(operands[j]) != 0 && strlen(operands[j]) <= MAX_DW_LENGTH)
				temp_number = atoi(operands[j]);
			else
				error_printer(line, "Error: operand isn't valid number");
			dw->data = temp_number;
			(dataword->data_word_type).dw_data = dw;
			(dataword->type) = DW_INSTRUCTION;
			data_img[*dc] = dataword;
			(*dc)++;
			(*dc_address)+=4;
		}
		break;
	/*.dh*/
	case DH_INSTRUCTION:
		for (j = 0; j < number_of_operands; j++) {
			/*Create the code word by the data: */
			dataword = (data_word*)safe_malloc(sizeof(data_word));
			dh = (dh_data_word*)safe_malloc(sizeof(dh_data_word));
			/*sort of strcpy but with dc increment*/
			if (atoi(operands[j]) != 0 && strlen(operands[j]) <= MAX_DH_LENGTH)
				temp_number = atoi(operands[j]);
			else
				error_printer(line, "Error: operand isn't valid number");
			dh->data = temp_number;			
			(dataword->data_word_type).dh_data = dh;
			(dataword->type) = DH_INSTRUCTION;
			data_img[*dc] = dataword;
			(*dc)++;
			(*dc_address) += 2;
		}
		break;
	default:
		break;
	}
	return SUCCESS;
}

/*
 * Extracts the operands from the line (after data there may be infinite amount)
 */
bool parse_data_operands(code_line current_line, int i, char** dest, int* number_of_operands) {
	int j;
	*number_of_operands = 0;
	dest[0] = dest[1] = NULL;
	/*Skip white chars*/
	SKIP_NON_WHITE(current_line.content, i)
		/*Unexpeted comma after command name*/
		if (current_line.content[i] == COMMA) {
			error_printer(current_line, "Unexpected comma after command.");
			return false;
		}

	/*Add operands until the end of the line, seprating using comma*/
	for (*number_of_operands = 0; current_line.content[i] != EOF && current_line.content[i] != NEW_LINE_UBUNTU && current_line.content[i] != NEW_LINE && current_line.content[i];) {
		dest[*number_of_operands] = safe_malloc(MAX_CHARS_SINGLE_LINE);
		for (j = 0; current_line.content[i] && current_line.content[i] != '\t' && current_line.content[i] != NEW_LINE_UBUNTU && current_line.content[i] != ' ' && current_line.content[i] != NEW_LINE && current_line.content[i] != EOF &&
			current_line.content[i] != COMMA; i++, j++) {
			dest[*number_of_operands][j] = current_line.content[i];
		}
		dest[*number_of_operands][j] = END_OF_STRING;
		(*number_of_operands)++; 
		/*Skip white chars*/
		SKIP_NON_WHITE(current_line.content, i)
			/*Missing comma*/
			if (current_line.content[i] == NEW_LINE || current_line.content[i] == NEW_LINE_UBUNTU || current_line.content[i] == EOF || !current_line.content[i]) break;
			else if (current_line.content[i] != COMMA) {
				error_printer(current_line, "Expecting COMMA between operands");
				free(dest[0]);
				/*Free memory*/
				if (*number_of_operands > 1) {
					free(dest[1]);
				}
				return false;
			}
		i++;
		/*Skip white chars*/
		SKIP_NON_WHITE(current_line.content, i)
			/*Comma without operand*/
			if (current_line.content[i] == NEW_LINE || current_line.content[i] == NEW_LINE_UBUNTU || current_line.content[i] == EOF || !current_line.content[i])
				error_printer(current_line, "Missing operand after comma.");
			/*Too many commas*/
			else if (current_line.content[i] == COMMA) error_printer(current_line, "Multiple consecutive commas.");
			else continue; 
		{ 
			/*Free memory*/
			free(dest[0]);
			if (*number_of_operands > 1) {
				free(dest[1]);
			}
			return false;
		}
	}
	return true;
}


/*Returns the instruction with the name in the input*/
instruction get_instruction_by_name(char* name) {
	struct instruction_element* curr_item;
	for (curr_item = instructions_collection; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return UNDEFINED_INSTRUCTION;
}

/*Returns true if the instrucion is asciz or data and false otherwise*/
bool is_asciz_or_data(instruction type) {
	return (type == DH_INSTRUCTION || type == DB_INSTRUCTION || type == DW_INSTRUCTION || type == ASCIZ_INSTRUCTION);
}
bool is_data_instruction(instruction type) {
	return (type == DH_INSTRUCTION || type == DB_INSTRUCTION || type == DW_INSTRUCTION);
}

/*Returns true if the instrucion is asciz and false otherwise*/
bool is_asciz_instruction(instruction type) {
return (type == ASCIZ_INSTRUCTION);
}