#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "lang.h"
#include "utility.h"
#include "status_handler.h"
#include "instruction_parser.h"


/*
 * Return the register number otherwise throw error message
 * @param line The source line
 * @param register operand name input
 * @return register or error messages
 */
reg get_register_critical(code_line line, char* name);

/*Extracts the operands from the line*/
bool operand_parser(code_line current_line, int i, char** dest, int* number_of_operands, char* c) {
	int j;
	/*Initial values*/
	*number_of_operands = 0;
	dest[0] = dest[1] = NULL;
	/*Skip white chars*/
	SKIP_NON_WHITE(current_line.content, i)
		/*Shouldn't hava a comma after the command*/
		if (current_line.content[i] == COMMA) {
			error_printer(current_line, "Unexpected comma after command.");
			return false; 
		}
	/*Extract the operands*/
	for (*number_of_operands = 0; current_line.content[i] != EOF && current_line.content[i] != NEW_LINE && current_line.content[i] != NEW_LINE_UBUNTU  && current_line.content[i];) {
		/*Illegal number of operands*/
		if (*number_of_operands == MAX_OPERANDS)  {
			error_printer(current_line, "Too many operands for operation (got >%d)", *number_of_operands);
			free(dest[0]);
			free(dest[1]);
			return false;
		}

		/*Memory Allocation*/
		dest[*number_of_operands] = safe_malloc(MAX_CHARS_SINGLE_LINE);
		/*Seprate using comma*/
		for (j = 0; current_line.content[i] && current_line.content[i] != NEW_LINE_UBUNTU && current_line.content[i] != '\t' && current_line.content[i] != ' ' && current_line.content[i] != NEW_LINE && current_line.content[i] != EOF &&
			current_line.content[i] != COMMA; i++, j++) {
			dest[*number_of_operands][j] = current_line.content[i];
		}
		dest[*number_of_operands][j] = END_OF_STRING;
		(*number_of_operands)++; 
		/*Skip white chars*/
		SKIP_NON_WHITE(current_line.content, i)
			/*Missing comma between opernads*/
			if (current_line.content[i] == NEW_LINE || current_line.content[i] == EOF || current_line.content[i] == NEW_LINE_UBUNTU||  !current_line.content[i]) break;
			else if (current_line.content[i] != COMMA) {
				error_printer(current_line, "Expecting COMMA between operands");
				free(dest[0]);
				if (*number_of_operands > 1) {
					free(dest[1]);
				}
				return false;
			}
		i++;
		/*Skip white chars*/
		SKIP_NON_WHITE(current_line.content, i)
			/*Missing comma after opernads*/
			if (current_line.content[i] == '\n' || current_line.content[i] == NEW_LINE_UBUNTU || current_line.content[i] == EOF || !current_line.content[i])
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

/*Generate the command from the parameters*/
code_word* get_processed_code(code_line current_line, opcode curr_opcode, funct curr_funct, code_type curr_type,  int op_count, char* operands[2]) {
	code_word* codeword;
	r_code_word* r_word;
	i_code_word* i_word;
	j_code_word* j_word;

	/*Validate operands by opcode - on failure exit*/
	if (!validate_operand_by_opcode(current_line, curr_opcode, curr_type, op_count)) {
		return NULL;
	}
	/* reate the code word by the data*/
	codeword = (code_word*)safe_malloc(sizeof(code_word));
	/*Work differently for each type of command*/
	switch (curr_type) {
	case R:
		/*Slightly different order of registers for different commands*/
		r_word = (r_code_word*)safe_malloc(sizeof(r_code_word));
		r_word->opcode = curr_opcode;
		r_word->funct = curr_funct;
		r_word->not_used = 0;
		if (curr_opcode >= ADD_OPCODE && curr_opcode <= NOR_OPCODE) {
			r_word->rs = get_register_critical(current_line, operands[0]);
			r_word->rt = get_register_critical(current_line, operands[1]);
			r_word->rd = get_register_critical(current_line, operands[2]);
		}
		else if (curr_opcode >= MOVE_OPCODE && curr_opcode <= MVLO_OPCODE) {
			r_word->rs = get_register_critical(current_line, operands[0]);
			r_word->rt = 0;
			r_word->rd = get_register_critical(current_line,operands[1]);
		}
		(codeword->code_word_type).r_code = r_word;
		(codeword->type) = R;
		break;
	case I:
		/*Slightly different order of registers for different commands*/
		i_word = (i_code_word*)safe_malloc(sizeof(i_code_word));
		i_word->opcode = curr_opcode;
		if (curr_opcode >= ADDI_OPCODE && curr_opcode <= NORI_OPCODE) {
			i_word->rs = get_register_critical(current_line, operands[0]);
			i_word->rt = get_register_critical(current_line, operands[2]);
			if (atoi(operands[1]) != 0 && strlen(operands[1]) < MAX_VALID_NUMBER_LENGTH)
				i_word->immed = atoi(operands[1]);
			else
				error_printer(current_line, "Error: operand isn't valid number");
		}
		else if (curr_opcode >= BNE_OPCODE && curr_opcode <= BGT_OPCODE) {
			i_word->rs = get_register_critical(current_line, operands[0]);
			i_word->rt = get_register_critical(current_line, operands[1]);
			i_word->immed = 0;/*delta between inst address to label address*/
		}
		else if (curr_opcode >= LB_OPCODE && curr_opcode <= SH_OPCODE) {
			i_word->rs = get_register_critical(current_line, operands[0]);
			i_word->rt = get_register_critical(current_line, operands[2]);
			if (atoi(operands[1]) != 0 && strlen(operands[1]) < MAX_VALID_NUMBER_LENGTH)
				i_word->immed = atoi(operands[1]);
			else
				error_printer(current_line, "Error: operand isn't valid number");
		}
		(codeword->code_word_type).i_code = i_word;
		(codeword->type) = I;
		break;
	case J:
		/*Slightly different order of registers for different commands*/
		j_word = (j_code_word*)safe_malloc(sizeof(j_code_word));
		j_word->opcode = curr_opcode;
		if (curr_opcode == JMP_OPCODE) {
			if (is_register(operands[0])) {
				j_word->address = get_register(operands[0]);
				j_word->reg = 1;
			}
			else {
				j_word->reg = 0;
				j_word->address = 0;/*delta between inst address to label address*/
			}
		}
		else if (curr_opcode == LA_OPCODE) {
			j_word->reg = 0;
			j_word->address = 0;/*delta between inst address to label address*/
		}
		else if (curr_opcode == CALL_OPCODE) {
			j_word->reg = 0;
			j_word->address = 0;/*delta between inst address to label address*/
		}
		else if (curr_opcode == STOP_OPCODE) {
			j_word->reg = 0;
			j_word->address = 0;
		}
		(codeword->code_word_type).j_code = j_word;
		(codeword->type) = J;
		break;
	default:
		break;
	}
	return codeword;
}

/*Check if this is char containg a register*/
bool is_register(char* name) {
	return get_register(name) != UNDEFINED_REG;
}

/*
* Get the register from the char
*/
reg get_register(char* name) {
	/*1 digit register number*/
	if (name[0] == '$' && isdigit(name[1]) && (name[2] == '\0')) {
		int digit = name[1] - '0'; /* convert digit ascii char to actual single digit */
		if (digit >= 0 && digit <= 31) return digit;
	}
	/*2 digits register number*/
	if (name[0] == '$' && isdigit(name[1]) && (isdigit(name[2]) && name[3] == '\0')) {
		int digit1 = name[1] - '0';
		int digit2 = name[2] - '0';
		int digit = digit1 * 10 + digit2;/* convert digit ascii char to actual single digit */
		if (digit >= 0 && digit <= 31) return digit;
	}
	return UNDEFINED_REG; /* No match */
}
/*Get the register from the char*/
reg get_register_critical(code_line line, char* name) {
	int res = get_register(name);
	if(res == UNDEFINED_REG)
		error_printer(line, "Error: register input is not valid");
	return res;
}

/*
 * Check that the number of operands is legal
 */
bool validate_operand_by_opcode(code_line line, opcode curr_opcode, code_type curr_type, int op_count) {
	if (curr_opcode >= ADD_OPCODE && curr_opcode <= NOR_OPCODE) {
		/* 3 operands required */
		if (op_count != 3) {
			error_printer(line, "Operation requires 3 operands (got %d)", op_count);
			return false;
		}
	}
	else if (curr_opcode >= MOVE_OPCODE && curr_opcode <= MVLO_OPCODE) {
		/* 2 operand required */
		if (op_count != 2) {
			error_printer(line, "Operation requires 2 operand (got %d)", op_count);
			return false;
		}
	}
	else if (curr_opcode >= ADDI_OPCODE && curr_opcode <= SH_OPCODE) {
		/* 3 operand required */
		if (op_count != 3) {
			error_printer(line, "Operation requires 3 operand (got %d)", op_count);
			return false;
		}
	}
	else if (curr_opcode >= JMP_OPCODE && curr_opcode <= CALL_OPCODE) {
		/* 1 operand required */
		if (op_count != 1) {
			if (op_count < 1) error_printer(line, "Operation requires 1 operand (got %d)", op_count);
			return false;
		}
	}
	else if (curr_opcode == STOP_OPCODE) {
		/* 0 operands exactly */
		if (op_count !=  0) {
			error_printer(line, "Operation requires no operands (got %d)", op_count);
			return false;
		}
	}
	return true;
}


/*
 * FInd the paramters of a command
*/
void get_opcode_funct_type(char* command_str, opcode* opcode_out, funct* funct_out, code_type* type_out) {
	struct command_bank* command_bank_element;
	*opcode_out = UNDEFINED_OPCODE;
	*funct_out = UNDEFINED_FUNCT;
	*type_out = UNDEFINED_TYPE;
	/*Start iterate*/
	for (command_bank_element = commands_collection; command_bank_element->command_str != NULL; command_bank_element++) {
		if (strcmp(command_bank_element->command_str, command_str) == 0) {
			*opcode_out = command_bank_element->_opcode;
			*funct_out = command_bank_element->_funct;
			*type_out = command_bank_element->_type;
			return;
		}
	}
	/*is collection doesn't exist*/
	if (instructions_collection == NULL) return;
}