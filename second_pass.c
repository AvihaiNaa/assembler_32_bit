/* All functions that are related to the first pass section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "utility.h"
#include "second_pass.h"
#include "status_handler.h"
#include "instruction_parser.h"
#include "lang.h"

/*
 * Processes a single line in the second pass
 */
status process_single_line_by_second_pass(env** global_env)
{
    long address_of_label, current_line_address, address_diff;
    /*This are the indicies following the line*/
    int i = 0, symbol_start, symbol_end, command_start, command_end, current_opcode, number_of_operands;
    char* operands[3]; /* 3 strings, each for operand */
    char operation[8]; /* stores the string of the current code instruction */ 
    instruction instruction;
    char symbol[MAX_CHARS_SINGLE_LINE], * entry_symbol, * j_symbol, * i_symbol;
    entry_symbol = (char*)malloc(MAX_CHARS_SINGLE_LINE * sizeof(char));
    j_symbol = (char*)malloc(MAX_CHARS_SINGLE_LINE * sizeof(char));
    i_symbol = (char*)malloc(MAX_CHARS_SINGLE_LINE * sizeof(char));
    /*If this line is a comment or is empty (as done in first pass) skip the line*/
    {SKIP_NON_WHITE((*global_env)->current_line.content, i) /* Skip non-white char */
        if (!(*global_env)->current_line.content[i] || (*global_env)->current_line.content[i] == NEW_LINE || (*global_env)->current_line.content[i] == NEW_LINE_UBUNTU || (*global_env)->current_line.content[i] == EOF || (*global_env)->current_line.content[i] == ';')
            return SUCCESS;

    /*Use these 3 if's and 1 skip, in order to skip the label in the begining of the line if it exists and save it*/
    if (looking_for_labels((*global_env)->current_line, symbol)) {
        return ERR;
    }
    /* if illegal name */
    if (symbol[0] && !is_pass_name_validation(symbol)) {
        error_printer((*global_env)->current_line, "Illegal label name: %s", symbol);
        return ERR;
    }
    /* try using strtok instead... */
    if (symbol[0] != END_OF_STRING) {
        for (; (*global_env)->current_line.content[i] != ':'; i++); /* if symbol detected, start analyzing from it's deceleration end */
        i++;
    }
    SKIP_NON_WHITE((*global_env)->current_line.content, i) /* Move to next not-white char */

        if ((*global_env)->current_line.content[i] == NEW_LINE || (*global_env)->current_line.content[i] == NEW_LINE_UBUNTU) return SUCCESS; /* Label-only line - skip */

    /* Get the instruction from the data file */
    instruction = looking_for_instruction((*global_env)->current_line, &i);

    /* If this line contains an instruction */
    if (instruction != UNDEFINED_INSTRUCTION)
    {
        /* If this is a .entry */
        if (instruction == ENTRY_INSTRUCTION)
        {
            /*First of all if we found a symbol in the begining we print a message*/
            if (is_pass_name_validation(symbol))
            {
                error_printer((*global_env)->current_line, "A symbol was defined before a .entry instruction");
            }           
            /*i now points to the end of the first char of the white chars before the arguments of the command
            we find the start and end of the argument using the first non-white char*/
            SKIP_NON_WHITE((*global_env)->current_line.content, i);
            symbol_start = i;
            SKIP_WHITE((*global_env)->current_line.content, i);
            symbol_end = i;
            /*Scan the actual symbol after .entry and put it inside entry_symbol*/ 
            extract_sub_string((*global_env)->current_line.content, entry_symbol, symbol_start, symbol_end);
            /* If the symbol is not legal */
            if (!is_pass_name_validation(entry_symbol))
            {
                /* Print an error message and return error */
                error_printer((*global_env)->current_line, "Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
                return ERR;
            }
            /* Skip white chars - if there are some extra chars after the .entry and return err*/
            SKIP_NON_WHITE((*global_env)->current_line.content, i);
            if (!(!(*global_env)->current_line.content[i] || (*global_env)->current_line.content[i] == NEW_LINE || (*global_env)->current_line.content[i] == NEW_LINE_UBUNTU || (*global_env)->current_line.content[i] == EOF || (*global_env)->current_line.content[i] == ';') )
            {
                error_printer((*global_env)->current_line, "There are some extra chars after the symbol of the .entry");
                return ERR;
            }
            /*Find the address of the label*/ 
            address_of_label = find_address_of_label(global_env, entry_symbol);
            /*If the symbol is not in the symbols table */
            if (address_of_label == -1)
            {
                error_printer((*global_env)->current_line, "The entry symbol is not in the symbols table");
                return ERR;
            }
            /*If the destination symbol is an external symbol */
            if (address_of_label == 0)
            {
                error_printer((*global_env)->current_line, "The entry symbol is already defined as external in the symbols table");
                return ERR;
            }
            /*Add the entry symbol to the symbol table*/
            add_new_item_to_symbol_table((&(*global_env)->symbol_table), entry_symbol, address_of_label, ENTRY_SYMBOL);
        }
        else
        {

            /* This is an instruction that is not .entry - dealt with in first pass */
            return SUCCESS;
        }
    }
    else
    {
        /* It is not an instruction so it must be a command */
        /*If it is not the first j word - need to increase ic*/
        if ((*global_env)->is_first_j_word)
        {
            (*global_env)->is_first_j_word = false;
        }
        else
        {
            (*global_env)->local_counter++;
            ((*global_env)->temp_second_ic) += 4;
        }
        switch (find_code_word(global_env, &((*global_env)->local_counter)))
        {
        case J:
            /* If this is a J type command */
            /*If the command is not "stop" (using opcode)*/
            current_opcode = find_opcode(global_env, (&(*global_env)->local_counter));
            if (current_opcode != STOP_OPCODE)
            {
                /*write the address of the J command to the binary file*/
                /*If the address is not a register*/
                if (!((*global_env)->code_memory_image[(*global_env)->local_counter]->word.code->code_word_type.j_code->reg))
                {
                    /*Extract the symbol after the J command*/
                    /*i now points to the start of the command - skip the command: skip non-white and than skip white chars*/
                    SKIP_WHITE((*global_env)->current_line.content, i);
                    SKIP_NON_WHITE((*global_env)->current_line.content, i)
                        /*i now points to the end of the first char of the white chars before the arguments of the command
                        we find the start and end of the argument using the first non-white char*/
                        SKIP_NON_WHITE((*global_env)->current_line.content, i);
                    symbol_start = i;
                    SKIP_WHITE((*global_env)->current_line.content, i);
                    symbol_end = i;
                    /*Scan the actual symbol after the command and put it inside j_symbol*/ 
                    extract_sub_string((*global_env)->current_line.content, j_symbol, symbol_start, symbol_end);
                    /*Compare the symbol to the symbols table and find the address*/ 
                    address_of_label = find_address_of_label(global_env, j_symbol);
                    /*Put the address in the right place in the data struct*/
                    ((*global_env)->code_memory_image[(*global_env)->local_counter]->word.code->code_word_type.j_code->address) = address_of_label;
                    /*If this is an external symbol the address is 0*/
                    if (address_of_label == 0)
                    {
                        /*add this symbol to a list of instructions that use external symbols*/
                        add_new_item_to_symbol_table((&(*global_env)->symbol_table), j_symbol, ((*global_env)->temp_second_ic), EXTERNAL_REFERENCE);
                    }
                    return SUCCESS;
                }
            }
            break;
        case I:
            /* If this is an I type command */
            /*If the command is "beq" / "bne" / "blt" / "bgt" (using opcode)*/
            current_opcode = find_opcode(global_env, (&(*global_env)->local_counter));
            if ((current_opcode == BEQ_OPCODE) || (current_opcode == BNE_OPCODE) || (current_opcode == BLT_OPCODE) || (current_opcode == BGT_OPCODE))
            {
                /*Extract the symbol after the I command and the arguments*/
                /*i now points to the start of the command - skip it with 1 pass of skip white and non-white, save it for the next function*/
                SKIP_NON_WHITE((*global_env)->current_line.content, i);
                command_start = i;
                SKIP_WHITE((*global_env)->current_line.content, i);
                command_end = i;
                extract_sub_string((*global_env)->dummy_line, operation, command_start, command_end);
                /*use this function to find the symbol - the last operand*/
                if (!operand_parser((*global_env)->current_line, i, operands, &number_of_operands, operation)) {
                    return ERR;
                }
                /*Extract the symbol itself*/
                strcpy(i_symbol, operands[number_of_operands - 1]);
                /*Compare the symbol to the symbols table and find the address*/ 
                address_of_label = find_address_of_label(global_env, i_symbol);
                /*If the destination symbol is not in the symbols table */
                if (address_of_label == -1)
                {
                    error_printer((*global_env)->current_line, "The destination symbol is not in the symbols table");
                    return ERR;
                }
                /*If the destination symbol is an external symbol */
                if (address_of_label == 0)
                {
                    error_printer((*global_env)->current_line, "The destination symbol is external");
                    return ERR;
                }
                /*Find the address of the current line*/
                current_line_address = ((*global_env)->temp_second_ic) ;
                /*Find the difference*/
                address_diff = address_of_label - current_line_address;
                /*Store the result*/
                ((*global_env)->code_memory_image[(*global_env)->local_counter]->word.code->code_word_type.i_code->immed) = address_diff;
            }
            break;

        default:
            break;
        }
    }
    if (commands_collection == NULL || instructions_collection == NULL) return ERR;
    return SUCCESS;
    }
}