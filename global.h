/* Constants */

#ifndef _GLOBAL_H
#define _GLOBAL_H

/*Include the symbol table data structure*/
#include "symbol_table.h"

/* Declaration of enum */
typedef enum { false = 0, true = 1 } bool;

/** Maximum size of code image and data image */
#define MAX_CODE_IMAGE 1000

/** Maximum length of a single source line  */
#define MAX_CHARS_SINGLE_LINE 80

/** Initial IC value */
#define IC_DEFAULT_VAL 100

/** Max operands in single instruction */
#define MAX_OPERANDS 3

/*Max string length of instruction */
#define SIZE_OF_INSTRUCTION 8

/*Termination of label*/
#define LABEL_TERMINATOR ':'

/*Extension of file input*/
#define AS_EXTENSION ".as"

/*End of string*/
#define END_OF_STRING '\0'

/*Comma symbol*/
#define COMMA ','

/*New line in windows*/
#define NEW_LINE '\n'

/*Part of new line in ubuntu*/
#define NEW_LINE_UBUNTU '\r'

/*Number of bits in single byte*/
#define BYTE 8

/*Number of bits in half*/
#define HALF_WORD 16

/*Number of bits in two third word*/
#define TWO_THIRDS_WORD 24

/*Number of bits in single word*/
#define WORD 32

/*Max length of valid number(operand of command)*/
#define MAX_VALID_NUMBER_LENGTH 6 

/*Max length of db number*/
#define MAX_DB_LENGTH 3 

/*Max length of dh number*/
#define MAX_DH_LENGTH 5 

/*Max length of dw number*/
#define MAX_DW_LENGTH 10 

/* Debug flag */
#define _DEBUG false

/*Request for read only access, when open file*/
#define READ_ONLY_ACCESS "r"

/*Request for write access, when open file*/
#define WRITE_ACCESS "w"


/** A status type */
typedef enum status {
	ERR,
	SUCCESS,
	ERROR_FILE_READING
} status;

/**
 * Represents a single source line
 */
typedef struct code_line {
	/** Line number in file */
	long line_number;
	/** File name */
	char* file_name;
	/** Line content (source) */
	char* content;

	char* file_name_without_ext;
} code_line;

/*
 * Represents a type of command
*/
typedef enum code_type {
	UNDEFINDED_CODE = 0,
	R = 1,
	I = 2,
	J = 3
}code_type;

/*
 * Represents a type of data structure
*/
typedef enum ds_types {
	UNDEFINDED_DS = 0,
	CODE = 2
}ds_type;

/** Instruction type (.data, .entry, etc.) */
typedef enum instruction {
	NONE = 0,

	/** .db, .dw .dd instruction */
	DH_INSTRUCTION,
	DW_INSTRUCTION,
	DB_INSTRUCTION,

	/** .extern instruction */
	EXTERN_INSTRUCTION,
	/** .entry instruction */
	ENTRY_INSTRUCTION,
	/** .asciz instruction */
	ASCIZ_INSTRUCTION,
	/** Not found */
	UNDEFINED_INSTRUCTION,
	/** Parsing error */
	ERROR_INSTRUCTION
} instruction;



/** Commands type */
typedef enum actions_type {
	/* First Group */
	ADD_TYPE = R,
	SUB_TYPE = R,
	AND_TYPE = R,
	OR_TYPE = R,
	NOR_TYPE = R,
	MOVE_TYPE = R,
	MVHI_TYPE = R,
	MVLO_TYPE = R,
	/* END First Group */

	/*Others*/
	ADDI_TYPE = I,
	SUBI_TYPE = I,
	ANDI_TYPE = I,
	ORI_TYPE = I,
	NORI_TYPE = I,
	BNE_TYPE = I,
	BEQ_TYPE = I,
	BLT_TYPE = I,
	BGT_TYPE = I,
	LB_TYPE = I,
	SB_TYPE = I,
	LW_TYPE = I,
	SW_TYPE = I,
	LH_TYPE = I,
	SH_TYPE = I,


	JMP_TYPE = J,
	LA_TYPE = J,
	CALL_TYPE = J,
	STOP_TYPE = J,

	/** Failed Or Error */
	UNDEFINED_TYPE = 0

} action_type;

/* Commands opcode */
typedef enum opcodes {
	/* First Group */
	ADD_OPCODE = 0,
	SUB_OPCODE = 0,
	AND_OPCODE = 0,
	OR_OPCODE = 0,
	NOR_OPCODE = 0,
	/* END First Group */

	/* Second Group */
	MOVE_OPCODE = 1,
	MVHI_OPCODE = 1,
	MVLO_OPCODE = 1,
	/* END Second Group */

	/*Others*/
	ADDI_OPCODE = 10,
	SUBI_OPCODE = 11,
	ANDI_OPCODE = 12,
	ORI_OPCODE = 13,
	NORI_OPCODE = 14,
	BNE_OPCODE = 15,
	BEQ_OPCODE = 16,
	BLT_OPCODE = 17,
	BGT_OPCODE = 18,
	LB_OPCODE = 19,
	SB_OPCODE = 20,
	LW_OPCODE = 21,
	SW_OPCODE = 22,
	LH_OPCODE = 23,
	SH_OPCODE = 24,
	JMP_OPCODE = 30,
	LA_OPCODE = 31,
	CALL_OPCODE = 32,
	STOP_OPCODE = 63,

	/** Failed Or Error */
	UNDEFINED_OPCODE = -1

} opcode;


/** Commands funct */
typedef enum funct {
	/* OPCODE 0 */
	ADD_FUNCT = 1,
	SUB_FUNCT = 2,
	AND_FUNCT = 3,
	OR_FUNCT = 4,
	NOR_FUNCT = 5,

	/* OPCODE 1 */
	MOVE_FUNCT = 1,
	MVHI_FUNCT = 2,
	MVLO_FUNCT = 3,

	/** Default (No need/Error) */
	UNDEFINED_FUNCT = 0
} funct;


/** Registers + undefined*/
typedef enum registers {
	reg_0 = 0,
	reg_1,
	reg_2,
	reg_3,
	reg_4,
	reg_5,
	reg_6,
	reg_7,
	reg_8,
	reg_9,
	reg_10,
	reg_11,
	reg_12,
	reg_13,
	reg_14,
	reg_15,
	reg_16,
	reg_17,
	reg_18,
	reg_19,
	reg_20,
	reg_21,
	reg_22,
	reg_23,
	reg_24,
	reg_25,
	reg_26,
	reg_27,
	reg_28,
	reg_29,
	reg_30,
	reg_31,
	UNDEFINED_REG = -1
} reg;



/** Represents a single code word */
typedef struct r_code_word {
	/* First byte: not_used+funct */
	unsigned int not_used : 6;
	unsigned int funct : 5;
	/* second byte: rd+rt+rs, source */
	unsigned int rd : 5;
	unsigned int rt : 5;
	unsigned int rs : 5;
	/* Third byte:  opcode */
	unsigned int opcode : 6;
} r_code_word;

/** Represents a single code word */
typedef struct i_code_word {
	/* First byte: not_used+funct */
	unsigned int immed : 16;
	/* second byte:rt+rs, source */
	unsigned int rt : 5;
	unsigned int rs : 5;
	/* Third byte:  opcode */
	unsigned int opcode : 6;
} i_code_word;



/** Represents a single code word */
typedef struct j_code_word {
	/* First byte: not_used+funct */
	unsigned int address :25;

	unsigned int reg : 1;

	/* Third byte:  opcode */
	unsigned int opcode : 6;
} j_code_word;

/** Represents asciz data word */
typedef struct asciz_data {
	char data;
} asciz_data;

/** Represents db quarter data word(1 bytes) */
typedef struct db_data_word {
	unsigned int data : 8;
} db_data_word;

/** Represents dw single data word(4 bytes) */
typedef struct dw_data_word {
	unsigned int data : 32;
} dw_data_word;

/** Represents dh half data word(2 bytes) */
typedef struct dh_data_word {
	unsigned int data : 16;
} dh_data_word;

/** Represents a single data word. */
typedef struct data_word {	
	instruction type;
	/* The data content */
	union data_word_type {
		db_data_word* db_data;
		dw_data_word* dw_data;
		dh_data_word* dh_data;
		asciz_data* asciz_data;
	} data_word_type;
} data_word;

/** Represents a single code word (R, J, I)*/
typedef struct code_word {
	code_type type;
	union code_word_type {
		r_code_word* r_code;
		i_code_word* i_code;
		j_code_word* j_code;
	} code_word_type;
} code_word;




/** Represents a general machine code word contents */
typedef struct asm_word {
	short length;
	ds_type type;
	/* The struct content is code word*/
	union word {
		code_word* code;
	} word;

} asm_word;




/**
 * Represents a env for single file
 */
typedef struct env {
	/* ic - instruction counter var. */
	long IC;
	/* dc - data counter var.*/
	long DC;
	/* ic */
	long icf;
	/* dc*/
	long dcf;
	/*tempaory var of ic*/
	long temp_second_ic;
	/*ic real  address(not counter)*/
	long ic_address;
	/*dc real  address(not counter)*/
	long dc_address;
	/*tempaory var for file generator*/
	long local_counter;
	/*Data memory image array*/
	data_word* data_memory_image[MAX_CODE_IMAGE];
	/*Instructions memory image array*/
	asm_word* code_memory_image[MAX_CODE_IMAGE];
	/*Dummy line of single line*/
	char dummy_line[MAX_CHARS_SINGLE_LINE + 2]; /* dummy string for storing single line */
	/*Represents a single current code line*/
	code_line current_line;
	/*Represents a symbol table*/
	_symbol_table symbol_table;
	/*File pointer to assembly file descriptor*/
	FILE* file_ptr;
	bool is_first_j_word;
} env;

#endif


