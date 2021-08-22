#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "global.h"
#include "status_handler.h"
#include "utility.h"

/*
* Init error status as false
*/
bool is_error_status = false;


/*Prints an erros message using the status indicator*/
void error_handler(status program_status) {
	/*compare strings by status code*/
	switch (program_status) {
	case SUCCESS:
		break;
	case ERROR_FILE_READING:
		printf("[ERROR] file is inaccessible for reading. skipping it.\n");
		break;
	case ERR:
		break;
	}
}

/*Prints an error message using the line paramters and a custom message*/
int error_printer(code_line line, char* message, ...) { 
	int result;
	va_list args;
	is_error_status = true;
	fprintf(ERR_OUTPUT_FILE, "Error In %s:%ld: ", line.file_name, line.line_number);

	/*get additional opernads*/
	va_start(args, message);
	result = vfprintf(ERR_OUTPUT_FILE, message, args);
	va_end(args);

	fprintf(ERR_OUTPUT_FILE, "\n");
	return result;

}

