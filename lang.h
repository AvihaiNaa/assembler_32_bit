
#ifndef _LANG_H
#define _LANG_H
#include "symbol_table.h"
#include "global.h"



/*
 * Extracts the operands from the line
 * @param line The source line
 * @param The destination
 * @param The number of operands
 * @param char c
 * @return Whether succeeded
 */
bool operand_parser(code_line , int , char** , int* , char* );

/*
 * Generate the command from the parameters
 * @param line The source line
 * @param opcode
 * @param funct
 * @param type of code
 * @param operands
 * @return the commad (code word)
 */
code_word* get_processed_code(code_line , opcode , funct , code_type , int , char* operands[2]);
/*
 * Check if this is char containg a register
 * @param line The source line
 * @param register operand name input
 * @return register or error messages
 */
bool is_register(char* name);
/*
 * Check that the number of operands is legal
 * @param line The source line
 * @param opcode
 * @param type of code
 * @param the number of operands
 * @return whether legal
 */
bool validate_operand_by_opcode(code_line , opcode , code_type , int );
/*
 * *Get the register from the char
 * @param the string of potential register
 * @return register identifier
 */
reg get_register(char* name);
/*
 * Represents a single command bank element of asm program
 */
struct command_bank {
	char* command_str;
	opcode _opcode;
	funct _funct;
	code_type _type;
};
/*
 * A commands bank for opcode, funct and type
 */
static struct command_bank commands_collection[] = {
		{"add",  ADD_OPCODE,		ADD_FUNCT,			ADD_TYPE},
		{"sub",  SUB_OPCODE,		SUB_FUNCT,			SUB_TYPE},
		{"and",  AND_OPCODE,		AND_FUNCT,			AND_TYPE},
		{"or",   OR_OPCODE,			OR_FUNCT,			OR_TYPE},
		{"nor",  NOR_OPCODE,		NOR_FUNCT,			NOR_TYPE},
		{"move", MOVE_OPCODE,		MOVE_FUNCT,			MOVE_TYPE},
		{"mvhi", MVHI_OPCODE,		MVHI_FUNCT,			MVHI_TYPE},
		{"mvlo", MVLO_OPCODE,		MVLO_FUNCT,			MVLO_TYPE},
		{"addi", ADDI_OPCODE,		UNDEFINED_FUNCT,	ADDI_TYPE},
		{"subi", SUBI_OPCODE,		UNDEFINED_FUNCT,	SUBI_TYPE},
		{"andi", ANDI_OPCODE,		UNDEFINED_FUNCT,	ANDI_TYPE},
		{"ori",  ORI_OPCODE,		UNDEFINED_FUNCT,	ORI_TYPE},
		{"nori", NORI_OPCODE,		UNDEFINED_FUNCT,	NORI_TYPE},
		{"bne",  BNE_OPCODE,		UNDEFINED_FUNCT,	BNE_TYPE},
		{"beq",  BEQ_OPCODE,		UNDEFINED_FUNCT,	BEQ_TYPE},
		{"blt",  BLT_OPCODE,		UNDEFINED_FUNCT,	BLT_TYPE},
		{"bgt",  BGT_OPCODE,		UNDEFINED_FUNCT,	BGT_TYPE},
		{"lb",   LB_OPCODE,			UNDEFINED_FUNCT,	LB_TYPE},
		{"sb",   SB_OPCODE,			UNDEFINED_FUNCT,	SB_TYPE},
		{"lw",   LW_OPCODE,			UNDEFINED_FUNCT,	LW_TYPE},
		{"sw",   SW_OPCODE,			UNDEFINED_FUNCT,	SW_TYPE},
		{"lh",   LH_OPCODE,			UNDEFINED_FUNCT,	LH_TYPE},
		{"sh",   SH_OPCODE,			UNDEFINED_FUNCT,	SH_TYPE},
		{"jmp",  JMP_OPCODE,		UNDEFINED_FUNCT,	JMP_TYPE},
		{"la",   LA_OPCODE,			UNDEFINED_FUNCT,	LA_TYPE},
		{"call", CALL_OPCODE,		UNDEFINED_FUNCT,	CALL_TYPE},
		{"stop", STOP_OPCODE,		UNDEFINED_FUNCT,	STOP_TYPE},
		{NULL,   UNDEFINED_OPCODE,  UNDEFINED_FUNCT,	UNDEFINED_TYPE}
};




#endif