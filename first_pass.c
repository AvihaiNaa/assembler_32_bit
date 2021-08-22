/* All functions that are related to the first pass section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "utility.h"
#include "first_pass.h"
#include "lang.h"
#include "instruction_parser.h"
#include "status_handler.h"

/*Read and interpret the end of the line in order to find the parameters inside it
* @params the paramters to be extracted from the line
* @return the status of the program
*/
bool code_builder(code_line line, int i, long* ic, long* ic_address, long* dc_address, long* dc, asm_word** code_memory_image);
/* Free the memory of the operands
*  @params the operands
*  @params and their amount
*  @return void
*/
void free_operands(char** operands, int* number_of_operands);

/*
*Read and interpret a single line in the first pass
*/
status process_single_line_by_first_pass(env** global_env) {
	int line_index, j;
	char symbol[MAX_CHARS_SINGLE_LINE];
	instruction instruction_type;
	code_line curr_line = (*global_env)->current_line;
	line_index = 0;
	
	/*Skip to the first non-white char*/
	SKIP_NON_WHITE(curr_line.content, line_index)
		if (!curr_line.content[line_index] || curr_line.content[line_index] == NEW_LINE || curr_line.content[line_index] == NEW_LINE_UBUNTU || curr_line.content[line_index] == EOF || curr_line.content[line_index] == ';')
			return SUCCESS; /*Empty or Comment line*/

	/*Check for labels in the begining of the line and find it's type*/
	if (looking_for_labels(curr_line, symbol)) {
		return ERR;
	}

	/*The name is illegal*/
	if (symbol[0] && !is_pass_name_validation(symbol)) {
		error_printer(curr_line, "Illegal label name: %s", symbol);
		return ERR;
	}

	/*If the symbol is already in the symbnol table*/
	if (is_symbol_defined(symbol[0])) {
		line_index = symbol_analyzer(curr_line, line_index);
	}

	/*Skip to the end of the symbol*/
	SKIP_NON_WHITE(curr_line.content, line_index) 
	
	/*There was only a symbol in the line*/
	if (curr_line.content[line_index] == NEW_LINE || curr_line.content[line_index] == NEW_LINE_UBUNTU) return SUCCESS;

	/*If the symbol is already defined as data / external / code and the line is not empty*/
	if (find_according_types((*global_env)->symbol_table, symbol, 3, EXTERNAL_SYMBOL, DATA_SYMBOL, CODE_SYMBOL)) {
		error_printer(curr_line, "Symbol %s is already defined.", symbol);
		return ERR;
	}

	/*Check for an instruction in and find it's type*/
	instruction_type = looking_for_instruction(curr_line, &line_index);

	/* Syntax error found */
	if (instruction_type == ERROR_INSTRUCTION) { 
		return ERR;
	}

	/*Skip to the end of the the instruction*/
	SKIP_NON_WHITE(curr_line.content, line_index)

	/*If the instruction is legal*/
	if (instruction_type != UNDEFINED_INSTRUCTION) {
	
		/*If .asciz or or .dh, .dw, .db - Add it into the symbol table*/
		if (is_symbol_defined(symbol[0]) && is_asciz_or_data(instruction_type))
			/*Add DC with the symbol to the table as data */
			add_new_item_to_symbol_table((&(*global_env)->symbol_table), symbol, ((*global_env)->dc_address), DATA_SYMBOL);
		
		/*If .asciz or .dh, .dw, .db - Add to image and incearse the data counter*/
		if (is_asciz_instruction(instruction_type) )
			return decrypt_asciz_instruction(curr_line, line_index, (*global_env)->data_memory_image, (&(*global_env)->DC), (&(*global_env)->dc_address));
		else if (is_data_instruction(instruction_type) )
			return decrypt_data_instruction(curr_line, line_index, (*global_env)->data_memory_image, (&(*global_env)->DC), (&(*global_env)->dc_address), instruction_type);

		/*If .extern - Add to symbol table */
		else if (instruction_type == EXTERN_INSTRUCTION) {
			SKIP_NON_WHITE(curr_line.content, line_index)
				/*Exreact the symbol*/
				for (j = 0; curr_line.content[line_index] && curr_line.content[line_index]  != NEW_LINE_UBUNTU && curr_line.content[line_index] != NEW_LINE && curr_line.content[line_index] != '\t' && curr_line.content[line_index] != ' ' && curr_line.content[line_index] != EOF; line_index++, j++) {
					symbol[j] = curr_line.content[line_index];
				}
			/*The name is illegal*/
			symbol[j] = 0;
			if (!is_pass_name_validation(symbol)) {
				error_printer(curr_line, "Invalid external label name: %s", symbol);
				return SUCCESS;
			}
			/*Add it to the symbols table*/
			add_new_item_to_symbol_table((&(*global_env)->symbol_table), symbol, 0, EXTERNAL_SYMBOL);
		}
		/*Entry instruction cannot come with a label*/
		else if (instruction_type == ENTRY_INSTRUCTION && is_symbol_defined(symbol[0])) {
			error_printer(curr_line, "Error: cant add label to an entry instruction");
			return ERR;
		}
			
	}
	else {
		/*If the symbol is already defined - Add it to table */
		if (is_symbol_defined(symbol[0]))
			add_new_item_to_symbol_table((&(*global_env)->symbol_table), symbol, ((*global_env)->ic_address), CODE_SYMBOL);
		/*Analyze and build the code from the remains of the line*/
		return code_builder(curr_line, line_index, (&(*global_env)->IC), (&(*global_env)->ic_address), (&(*global_env)->dc_address), (&(*global_env)->DC), (*global_env)->code_memory_image);
	}
	return SUCCESS;
}

/*
*Read and interpret the end of the line in order to find the parameters inside it
*/
bool code_builder(code_line line, int i, long* ic, long* ic_address, long* dc_address, long* dc, asm_word** code_memory_image) {
	char operation[SIZE_OF_INSTRUCTION];
	char* operands[MAX_OPERANDS]; /*for operand*/
	opcode curr_opcode; 
	funct curr_funct;
	code_type curr_type;
	code_word* codeword; 
	long snapshot_of_ic;
	int j, number_of_operands;
	asm_word* word_to_write;
	/*Skip to the begininig - after the white chars*/
	SKIP_NON_WHITE(line.content, i)
	/*Extract the operation name*/
	for (j = 0; line.content[i] && line.content[i] != '\t' && line.content[i] != NEW_LINE_UBUNTU  && line.content[i] != ' ' && line.content[i] != NEW_LINE && line.content[i] != EOF && j < 6; i++, j++) {
		operation[j] = line.content[i];
	}
	operation[j] = END_OF_STRING; /*End of string*/
	/*Find the opcode for the operation name*/
	get_opcode_funct_type(operation, &curr_opcode, &curr_funct, &curr_type);
	
	/*Check if the opcode is legal*/
	if (curr_opcode == UNDEFINED_OPCODE) {
		error_printer(line, "Unrecognized instruction: %s.", operation);
		return false; /* an error occurred */
	}

	/*Deinterleave and parse operands.*/
	if (!operand_parser(line, i, operands, &number_of_operands, operation)) {
		return false;
	}
	
	/*Find the paramters after the command name*/
	if ((codeword = get_processed_code(line, curr_opcode, curr_funct, curr_type, number_of_operands, operands)) == NULL) {
		/*free memory*/ 
		free_operands(&(operands[0]), &number_of_operands);
		return false;
	}

	snapshot_of_ic = *ic;
	word_to_write = (asm_word*)safe_malloc(sizeof(asm_word));

	(word_to_write->word).code = codeword;
	(word_to_write->type) = CODE;

	code_memory_image[(*ic) - IC_DEFAULT_VAL] = word_to_write;

	/*free memory*/ 
	free_operands(&(operands[0]), &number_of_operands);

	/*increase ic to point the next cell*/
	(*ic)+=1;
	(*ic_address) += 4;

	code_memory_image[snapshot_of_ic - IC_DEFAULT_VAL]->length = (*ic) - snapshot_of_ic;

	return true;
}


/*
* Free the memory of the operands
*/
void free_operands(char** operands,int* number_of_operands) {
	if (is_error_status)
		return;
	if ((*number_of_operands)--) { /* If there's 1 operand at least */
		free(operands[0]);
		if ((*number_of_operands) > 1) { /* If there are 2 operands */
			free(operands[1]);
			if ((*number_of_operands) > 2) { /* If there are 3 operands */
				free(operands[2]);
			}
		}
	}
	/*is collection doesn't exist*/
	if (commands_collection == NULL || instructions_collection == NULL) return;
}
