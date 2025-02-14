# Define variables
LEX_FILE = lexer.l
LEX_C_FILE = lex.yy.c
YACC_FILE = parser.y
YACC_H_FILE = parser.tab.h
YACC_C_FILE = parser.tab.c
EXECUTABLE = parser
YACC_D_FILE = parser.output

# Compiler and flags
LEX = lex
YACC = bison
YFLAGS = -d -v
CC = gcc
CFLAGS = -ll

# # Initial cleaning
# clean:
# 	rm -rf $(C_FILE) $(EXECUTABLE)

# Default target: Build the executable
all: $(EXECUTABLE)

# Generate parser.tab.h and parser.tab.c from parser.y
$(YACC_C_FILE) $(YACC_H_FILE): $(YACC_FILE)
	$(YACC) $(YFLAGS) $(YACC_FILE)

# Generate lex.yy.c from lexer.l 
$(LEX_C_FILE): $(LEX_FILE)
	$(LEX) $(LEX_FILE)

# Compile lex.yy.c into the final executable
$(EXECUTABLE): $(LEX_C_FILE) $(YACC_C_FILE)
	$(CC) -o $(EXECUTABLE) $(LEX_C_FILE) $(YACC_C_FILE) $(CFLAGS)

clean:
	rm -rf $(LEX_C_FILE) $(EXECUTABLE) $(YACC_C_FILE) $(YACC_H_FILE) $(YACC_D_FILE)