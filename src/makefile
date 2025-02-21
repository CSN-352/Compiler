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
CFLAGS = -lfl

# # Initial cleaning
# clean:
# 	rm -rf $(C_FILE) $(EXECUTABLE)

# Default target: Build the executable
all: $(EXECUTABLE)

# Generate parser.tab.h and parser.tab.c from parser.y
$(YACC_C_FILE) $(YACC_H_FILE): $(YACC_FILE)
	$(YACC) $(YFLAGS) $(YACC_FILE)

# Generate lex.yy.c from lexer.l (depends on parser.tab.h)
$(LEX_C_FILE): $(LEX_FILE) $(YACC_H_FILE)
	$(LEX) $(LEX_FILE)

# Compile lex.yy.c and parser.tab.c separately, then link them
lex.yy.o: $(LEX_C_FILE)
	$(CC) -c $(LEX_C_FILE) $(CFLAGS)

parser.tab.o: $(YACC_C_FILE)
	$(CC) -c $(YACC_C_FILE) $(CFLAGS)

# Link object files into the final executable
$(EXECUTABLE): lex.yy.o parser.tab.o
	$(CC) -o $(EXECUTABLE) lex.yy.o parser.tab.o $(CFLAGS)

clean:
	rm -rf $(LEX_C_FILE) $(EXECUTABLE) $(YACC_C_FILE) $(YACC_H_FILE) $(YACC_D_FILE) *.o