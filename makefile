# Basic compilation macros

COMPILER = gcc # GCC Compiler

ALL_FLAGS = -ansi -Wall   -pedantic #Flags

GLOBAL_DEPS = global.h # Dependencies for everything

Dependencies = assembler.o lang.o first_pass.o second_pass.o instruction_parser.o symbol_table.o utility.o file_generator.o status_handler.o unit_tests.o

## Executable
assembler: $(Dependencies) $(GLOBAL_DEPS)
	$(COMPILER) -g $(Dependencies) $(ALL_FLAGS) -o $@

## Main:
assembler.o: assembler.c $(GLOBAL_DEPS)
	$(COMPILER) -c assembler.c $(ALL_FLAGS) -o $@

## Code helper functions:
lang.o: lang.c lang.h $(GLOBAL_DEPS)
	$(COMPILER) -c lang.c $(ALL_FLAGS) -o $@
	
## First Pass:
first_pass.o: first_pass.c first_pass.h $(GLOBAL_DEPS)
	$(COMPILER) -c first_pass.c $(ALL_FLAGS) -o $@
	
## Second Pass:
second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(COMPILER) -c second_pass.c $(ALL_FLAGS) -o $@
	
## Instructions helper functions:
instruction_parser.o: instruction_parser.c instruction_parser.h $(GLOBAL_DEPS)
	$(COMPILER) -c instruction_parser.c $(ALL_FLAGS) -o $@

## Symbol table:
symbol_table.o: symbol_table.c symbol_table.h $(GLOBAL_DEPS)
	$(COMPILER) -c symbol_table.c $(ALL_FLAGS) -o $@

## utilities functions:
utility.o: utility.c instruction_parser.h $(GLOBAL_DEPS)
	$(COMPILER) -c utility.c $(ALL_FLAGS) -o $@

## file generator Files:
file_generator.o: file_generator.c file_generator.h $(GLOBAL_DEPS)
	$(COMPILER) -c file_generator.c $(ALL_FLAGS) -o $@

## status functions:
status_handler.o: status_handler.c status_handler.h $(GLOBAL_DEPS)
	$(COMPILER) -c status_handler.c $(ALL_FLAGS) -o $@
	
## Unit tests functions:
unit_tests.o: unit_tests.c unit_tests.h $(GLOBAL_DEPS)
	$(COMPILER) -c unit_tests.c $(ALL_FLAGS) -o $@
	
# Clean Target (remove leftovers)
clean:
	rm -rf *.o