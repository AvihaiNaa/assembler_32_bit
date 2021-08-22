#ifndef _ERRORS_HANDLER_H
#define _ERRORS_HANDLER_H

#include "global.h"

/*
* stderr  - error type
*/
#define ERR_OUTPUT_FILE stderr

/*
* Used as a flag to stop a program before creating output files
*/
extern  bool is_error_status;
/**
 * Prints a detailed error message, using by status handler.
 * @param current code line
 * @param error message
 * @return result function status
 */
int error_printer(code_line line, char* message, ...);
/**
 * Prints a detailed error message.
 * @param status enumerator
 * @return nothing
 */
void error_handler(status);

#endif




