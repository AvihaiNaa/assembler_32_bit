#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "utility.h"
#include "file_generator.h"
#include "status_handler.h"


/* This function generates the file from the symbol table
*  @params the data struct
*  @return the status of the program
*/
status write_table_to_file(_symbol_table tab, char* file_name_no_extension, char* file_extension);

/* This function generates the object file
*  @params the data struct
*  @return the status of the program
*/
status generate_object_file(env** global_env);
/*This function seprates the code image and the data image into 4 fields as requested
* @params the data to seprate
* @params first_field
*/
void separate_fields(long* source, long* first_field, long* second_field, long* third_field, long* forth_field);

/*This is the function that writes the program output to a file
* @params the file pointer
* @params the first field of hexa
* @params the second field of hexa
* @params the third field of hexa
* @params the forth field of hexa
* @params the cnt
* @params the address counter offset
* @params the index
* @params the dcf
* @return nothing
*/
void write_to_file(FILE** file_ptr, long first_field, long second_field, long third_field, long forth_field, int cnt, int address_counter, int i, int dcf);

/*
* This function generate the output files that are required
*/
status generate_output_files(env** global_env) {
	status ob_status, ext_status, ent_status;
	/*Get the externals and the entries*/
	_symbol_table externals = get_table_by_symbol_type((*global_env)->symbol_table, EXTERNAL_REFERENCE);
	_symbol_table entries = get_table_by_symbol_type((*global_env)->symbol_table, ENTRY_SYMBOL);
		
	/*Generate object file*/
	ob_status = generate_object_file(global_env);

	/*Check program status*/
	if (ob_status != SUCCESS)
		return ob_status;

	/*Generate externals file*/
	ext_status = write_table_to_file(externals, (*global_env)->current_line.file_name_without_ext, ".ext");
	
	/*Check program status*/
	if (ext_status != SUCCESS)
		return ext_status;

	/*Generate entries file*/
	ent_status =	write_table_to_file(entries, (*global_env)->current_line.file_name_without_ext, ".ent");

	/*Check program status*/
	if (ent_status != SUCCESS)
		return ent_status;

	/*If we got here*/
	return SUCCESS;
}

/*
* This function seprates the code image and the data image into 4 fields as requested
*/
void separate_fields(long* source, long* first_field, long* second_field, long* third_field, long* forth_field) {
	/*Seprating into fields*/
	*first_field = *source & 0xFF000000;
	*first_field = *first_field >> TWO_THIRDS_WORD;
	*first_field &= 0x000000FF;
	*second_field = *source & 0x00FF0000;
	*second_field = *second_field >> HALF_WORD;
	*second_field &= 0x000000FF;
	*third_field = *source & 0x0000FF00;
	*third_field = *third_field >> BYTE;
	*third_field &= 0x000000FF;
	*forth_field = *source & 0x000000FF;
}
/*
* This function generates the object file
*/
status generate_object_file(env** global_env) {
	/*Needed variables*/
	int i;
	r_code_word* r_word;
	i_code_word* i_word;
	j_code_word* j_word;
	db_data_word* db;
	dh_data_word* dh;
	dw_data_word* dw;
	asciz_data* asciz;
	int cnt = 0;
	unsigned int ab;
	unsigned int cd;
	unsigned int ef;
	unsigned int gh;
	long result;
	long first_field, second_field, third_field, forth_field;
	long draft_result;
	int char_counter = 4;
	int draft_char_counter = 4;
	int address_counter;
	/*File name*/
	char* ob_file_name = string_concat((*global_env)->current_line.file_name_without_ext, ".ob");
	
	FILE* file_ptr = NULL;
	/*Open file*/
	status program_status = safe_open(&file_ptr, ob_file_name, WRITE_ACCESS);
	if (program_status != SUCCESS)
		return program_status;
	
	/*Print the header to the file*/
	fprintf(file_ptr, "     %ld %ld", (*global_env)->icf - IC_DEFAULT_VAL, (*global_env)->dc_address);

	/*Starting with commands*/
	/*Starting from begining - IC DEFAULT VALUE as icf*/
	for (i = 0; i < (*global_env)->IC - IC_DEFAULT_VAL; i++) {
		/*This is used for all of the command types*/
		result = 0;
		if ((*global_env)->code_memory_image[i]->length > 0) {
			/*Calculate the values to write for each type of command (R/I/J)*/
			switch ((*global_env)->code_memory_image[i]->word.code->type) {
			case R:
				r_word = (*global_env)->code_memory_image[i]->word.code->code_word_type.r_code;
				result = (r_word->opcode << 26) | (r_word->rs << 21) |
					(r_word->rt << HALF_WORD) | (r_word->rd << 11) |
					(r_word->funct << 6) |
					(r_word->not_used);
				break;
			case I:
				i_word = (*global_env)->code_memory_image[i]->word.code->code_word_type.i_code;
				result = (i_word->opcode << 26) | (i_word->rs << 21) |
					(i_word->rt << HALF_WORD)|
					(i_word->immed);
				break;
			case J:
				j_word = (*global_env)->code_memory_image[i]->word.code->code_word_type.j_code;
				result = (j_word->opcode << 26) | (j_word->reg << 25) |
					(j_word->address);
				break;
			default:
				break;
			}
			/*Separating bits*/
			gh = result & 0xff;
			ef = (result >> BYTE) & 0xff;
			cd = (result >> HALF_WORD) & 0xff;
			ab = (result >> TWO_THIRDS_WORD) & 0xff;
			result = (gh << TWO_THIRDS_WORD) |
				(ef << HALF_WORD) |
				(cd << BYTE) |
				(ab);
		}
		/*Seprating into fields*/
		separate_fields(&result, &first_field, &second_field, &third_field, &forth_field);
		/*Print the values calculated above*/
		/*For the last line there is a different case*/
		write_to_file(&file_ptr, first_field, second_field, third_field, forth_field, i, 0, i, (*global_env)->IC - IC_DEFAULT_VAL - 1);
		/*Reinitiate IC*/
		cnt = (*global_env)->IC - IC_DEFAULT_VAL;
	}

	/*Now dealing with instructions*/
	result = 0;
	draft_result = 0;
	address_counter = 0;
	/*Starting from begining - DC DEFAULT VALUE as icf*/
	for (i = 0; i < (*global_env)->dcf; i++) {
			switch ((*global_env)->data_memory_image[i]->type) {
				/*Calculate the values to write for each type of instruction*/
			case ASCIZ_INSTRUCTION:
				asciz = (*global_env)->data_memory_image[i]->data_word_type.asciz_data;
				char_counter--;
				result = result | (asciz->data << (WORD - ((4 - char_counter) * BYTE)));
				break;
			case DB_INSTRUCTION:
				db = (*global_env)->data_memory_image[i]->data_word_type.db_data;
				 gh = db->data & 0xff;
				char_counter--;
				result = result | (gh << (WORD - ((4 - char_counter) * BYTE)));
				break;
			case DH_INSTRUCTION:
				dh = (*global_env)->data_memory_image[i]->data_word_type.dh_data;
				 gh = dh->data & 0xff;
				 ef = (dh->data >> BYTE) & 0xff;
				if (char_counter == 1) {
					result = result | (gh);
					draft_result = (ef << TWO_THIRDS_WORD) |draft_result;
					draft_char_counter = 3;
					char_counter = 0;
				}
				else if (char_counter == 2) {
					result = (gh << BYTE) |
						(ef) | result;
					draft_result = 0;
					draft_char_counter = 4;
					char_counter = 0;
				}
				else if (char_counter == 3) {
					result = (gh << HALF_WORD) |
						(gh << BYTE) | result;
					draft_result = 0;
					draft_char_counter = 4;
					char_counter = 1;
				}
				else if (char_counter == 4) {
					result = (dh->data);
					char_counter = 2;
				}
				break;
			case DW_INSTRUCTION:
				dw = (*global_env)->data_memory_image[i]->data_word_type.dw_data;
				 gh = dw->data & 0xff;
				 ef = (dw->data >> BYTE) & 0xff;
				 cd = (dw->data >> HALF_WORD) & 0xff;
			     ab = (dw->data >> TWO_THIRDS_WORD) & 0xff;
				if (char_counter == 1) {
					result = result | (gh);
					draft_result = (ef << TWO_THIRDS_WORD) |
						(cd << HALF_WORD) |
						(ab << BYTE) | draft_result;
					draft_char_counter = 1;
				}
				else if (char_counter == 2) {
					result = (gh << BYTE) |
						(ef) | result;
					draft_result = (cd << TWO_THIRDS_WORD) |
						(ab << HALF_WORD) | draft_result;
					draft_char_counter = 2;
				}
				else if (char_counter == 3) {
					result = (gh << HALF_WORD) |
						(ef << BYTE) |
						(cd) | result;
					draft_result = 
						(ab << TWO_THIRDS_WORD) | draft_result;
					draft_char_counter = 3;
				}
				else if (char_counter == 4)
					result = (dw->data);
				char_counter = 0;
				break;
			default:
				break;
			}
			if (char_counter == 0 || ((*global_env)->dcf == (i+1)) ) {
				/*Seprating into fields*/
				separate_fields(&result, &first_field, &second_field, &third_field, &forth_field);
				/*Print the values calculated above*/
				/*For the last line there is a different case*/
				write_to_file(&file_ptr, first_field, second_field, third_field, forth_field, cnt, address_counter, i, (*global_env)->dcf - 1);
				address_counter += 1;
				if (((*global_env)->dcf == (i + 1)) && draft_char_counter != 4) {
					char_counter = draft_char_counter;
					result = draft_result;
					/*Seprating into fields*/
					separate_fields(&result, &first_field, &second_field, &third_field, &forth_field);
					/*For the last line there is a different case*/
					write_to_file(&file_ptr, first_field, second_field, third_field, forth_field, cnt, address_counter, i, (*global_env)->dcf - 1);
				}
				else {
					char_counter = draft_char_counter;
					draft_char_counter = 4;
					result = draft_result;
					draft_result = 0;
				}


			}
	}
	/*Close the file*/
	fclose(file_ptr);
	/*If we got here*/
	return SUCCESS;
}


/*
* This function generates the file from the symbol table
*/
status write_table_to_file(_symbol_table tab, char* file_name_no_extension, char* file_extension) {
	/*Concatenate the filename with the extension*/
	char* file_name_with_extension = string_concat(file_name_no_extension, file_extension);
	FILE* file_ptr = NULL;
	/*Open file*/
	status program_status = safe_open(&file_ptr, file_name_with_extension, WRITE_ACCESS);
	/*Check program status*/
	if (program_status != SUCCESS)
		return program_status;
	/*The table is empty*/
	if (tab == NULL) return SUCCESS;
	/*Print to files*/
	fprintf(file_ptr, "%s %.4ld", tab->key, tab->value);
	while ((tab = tab->next) != NULL) {
		fprintf(file_ptr, "\n%s %.4ld", tab->key, tab->value);
	}
	/*Close the file*/
	fclose(file_ptr);
	/*If we got here*/
	return SUCCESS;
}


/*
* This is the function that writes the program output to a file
*/
void write_to_file(FILE** file_ptr, long first_field, long second_field, long third_field, long forth_field, int cnt, int address_counter, int i, int dcf) {
	if (i == dcf)
	{
		/*Print only the non-white fields*/
		/*Don't print any line content after the line number*/
		if ((first_field == 0) && (second_field == 0) && (third_field == 0) && (forth_field == 0))
		{
			fprintf(*file_ptr, "\n%.4ld", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL));
		}
		else
		{
			if ((second_field == 0) && (third_field == 0) && (forth_field == 0))
			{
				fprintf(*file_ptr, "\n%.4ld %.2lX", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL), first_field);
			}
			else
			{
				if ((third_field == 0) && (forth_field == 0))
				{
					fprintf(*file_ptr, "\n%.4ld %.2lX %.2lX", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL), first_field, second_field);
				}
				else
				{
					if (forth_field == 0)
					{
						fprintf(*file_ptr, "\n%.4ld %.2lX %.2lX %.2lX", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL), first_field, second_field, third_field);
					}
					else
					{
						/*Print regulary*/ 
						fprintf(*file_ptr, "\n%.4ld %.2lX %.2lX %.2lX %.2lX", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL), first_field, second_field, third_field, forth_field);
					}
				}
			}
		}

	}
	/*Regular line*/
	else {
		fprintf(*file_ptr, "\n%.4ld %.2lX %.2lX %.2lX %.2lX", (long)(((cnt + address_counter) * 4) + IC_DEFAULT_VAL), first_field, second_field, third_field, forth_field);
	}
}

