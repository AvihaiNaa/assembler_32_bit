#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "utility.h"
#include "global.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_generator.h"
#include "assembler_interface.h"
#include "unit_tests.h"
#include "status_handler.h"


/* Ininitiate the data struct with initial values + malloc
*  @return the enviorment - the data struct
*/
env* init_env();

/* Read and interpret all of the lines in the first pass
*  @params the data struct
*  @return the status of the program
*/
status first_pass_file_reader(env**);

/* Read and interpret all of the lines in the second pass
*  @params the data struct
*  @return the status of the program
*/
status second_pass_file_reader(env**);

/* Free memory after the runtime
*  @params the data struct
*  @return the status of the program
*/
status free_random_access_memory(env** my_env);

/* Using for custom function for visual studio debugger
*  @return is successed
*/
bool _vs_debuging();

/**
 * Main - 32bit assembler
 */
int main(int argc, char* argv[]) {
	int i;
	/*Indicator of the program status*/
	bool succeeded = true;
	/*This is used for debugging - a single file*/
	if (_DEBUG)
		succeeded = _vs_debuging();
	/*This is used for runtie - multiple files*/
	else {
		for (i = 1; i < argc; ++i) {
			/* when failed, continue to next file */
			if (!succeeded) puts("");
			/*start processing. */
			succeeded = start_processing_file(argv[i]);
			/* Break when failed */
		}
	}
	
	/*Test the output, compare to expexted result*/
	if (UNIT_TESTS)
		unit_tests();
	
	return 0;
}

/*This function runs the assembler for a single file
* it calls the first + second pass and the cleanup function
* @params the data struct
* @return the status of the program
*/
 status start_processing_file(char* file_name_no_extension) {
	status program_status;
	
	char* file_name_with_extension;
	/*Initiate the enviorment*/
	env* new_env = init_env();

	/*Concat extensionless filename with .as extension*/
	file_name_with_extension = string_concat(file_name_no_extension, AS_EXTENSION);


	/*Open file*/
	program_status = safe_open(&(new_env->file_ptr), file_name_with_extension, READ_ONLY_ACCESS);
	if (program_status != SUCCESS) {
		error_handler(program_status);
		return ERR;
	}

	/*Line meta data*/
	new_env->current_line.file_name = file_name_with_extension;
	new_env->current_line.content = new_env->dummy_line;
	new_env->current_line.file_name_without_ext = file_name_no_extension;

	/*First pass - multiple lines*/
	program_status = first_pass_file_reader(&new_env);

	/*Set values after the end of the first pass*/
	new_env->icf = new_env->ic_address;
	new_env->dcf = new_env->DC;

	/*Check program status*/
	if (program_status != SUCCESS)
		error_handler(program_status);
	
	if (is_error_status)
		return ERR;

	/*Second pass - multiple lines*/
	program_status = second_pass_file_reader(&new_env);

	/*Check program status*/
	if (program_status != SUCCESS)
		error_handler(program_status);

	if (is_error_status)
		return ERR;

	/*Generate output files based on the data*/
	program_status = generate_output_files(&new_env);

	/*Check program status*/
	if (program_status != SUCCESS)
		error_handler(program_status);

	/*Free memory*/
	program_status = free_random_access_memory(&new_env);

	/*Check program status*/
	if (program_status != SUCCESS)
		error_handler(program_status);

	/*If we got here*/
	return SUCCESS;


}
 
env* init_env() {

	/*Malloc*/
	env* instance = (env*)malloc(sizeof(env));
	if (instance == NULL) {
		/* error handling */
		printf("[ERROR] Memory error\n");
		return NULL;
	}

	/*Initial values*/
	instance->IC = IC_DEFAULT_VAL;
	instance->DC = 0;
	instance->icf = 0;
	instance->dcf = 0;
	instance->temp_second_ic = IC_DEFAULT_VAL;
	instance->ic_address = IC_DEFAULT_VAL;
	instance->dc_address = 0;
	instance->local_counter = 0;
	instance->symbol_table = NULL;
	instance->is_first_j_word = true;

	return instance;
}


status first_pass_file_reader(env** my_env) {
	/*Indicator of the program status*/
	status first_pass_status = SUCCESS;
	/*Read each line - exit on error*/
	for ((*my_env)->current_line.line_number = 1;
		fgets((*my_env)->current_line.content, MAX_CHARS_SINGLE_LINE + 2, (*my_env)->file_ptr) != NULL;
		(*my_env)->current_line.line_number++) {
		
		/*Verify the length of the line and report errors*/
		if (strchr((*my_env)->current_line.content, NEW_LINE) == NULL && !feof((*my_env)->file_ptr)) {
			error_printer((*my_env)->current_line, "Line too long to process.");
			first_pass_status = ERR;
		}
		else {
			/*Single line analysis*/
			if (!process_single_line_by_first_pass(my_env)) {
				if (first_pass_status) {
					/*Value fix after one line*/
					(*my_env)->icf = -1;
					first_pass_status = ERR;
				}
			}
		}
	}
		return first_pass_status;
}

status second_pass_file_reader(env** my_env) {
	/*Indicator of the program status*/
	status second_pass_status = SUCCESS;

	add_var_to_value_field((*my_env)->symbol_table, (*my_env)->icf, DATA_SYMBOL);

	/*Rewind back to the bgining of the file*/
	rewind((*my_env)->file_ptr);

	/*Go through all of the lines*/
	for ((*my_env)->current_line.line_number = 1; !feof((*my_env)->file_ptr); (*my_env)->current_line.line_number++) {
		int i = 0;
		fgets((*my_env)->current_line.content, MAX_CHARS_SINGLE_LINE, (*my_env)->file_ptr); /* Grab line */
		SKIP_NON_WHITE((*my_env)->current_line.content, i)
				second_pass_status &= process_single_line_by_second_pass(my_env);		
	}
	/*The status was saved earlier*/
	return second_pass_status;
}

status free_random_access_memory(env** my_env) {
	/*Calls the free function for each part of the data struct*/
	free((*my_env)->current_line.file_name);
	make_symbol_table_free((*my_env)->symbol_table);
	free_code_image((*my_env)->code_memory_image, (*my_env)->IC);
	return SUCCESS;
}


bool _vs_debuging() {
	bool succeeded = true;
	char* path = "C:\\Projects\\openu_project\\openu_project\\openu_project_avihai\\test\\stable_test\\T0";
	succeeded = start_processing_file(path);
	if(succeeded)
		 return true;
	return false;
}