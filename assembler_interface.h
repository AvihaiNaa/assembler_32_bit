#ifndef _ASSEMBLER_H
#define _ASSEMBLER_H
#include "global.h"

/*
* Unit test flag
*/
#define UNIT_TESTS false


/**
 * Processes a single assembly source file, and returns the result status.
 * @param filename The filename, without it is extension
 * @return Whether succeeded
 */
 status start_processing_file(char* filename);

#endif
