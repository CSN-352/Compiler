# Define variables
BUILD_DIR = build
BIN_DIR = build/bin
LEX_FILE = lexer.l
LEX_C_FILE = $(BUILD_DIR)/lex.yy.c
YACC_FILE = parser.y
YACC_BASE = $(BUILD_DIR)/parser
YACC_H_FILE = $(YACC_BASE).tab.h
YACC_C_FILE = $(YACC_BASE).tab.c
EXECUTABLE = $(BIN_DIR)/parser
YACC_D_FILE = $(YACC_BASE).output

LEX_O_FILE = $(BUILD_DIR)/lex.yy.o
YACC_O_FILE = $(BUILD_DIR)/parser.tab.o

# Compiler and flags
LEX = lex
YACC = bison
YFLAGS = -d -v
CC = gcc
CFLAGS = -ll

all: $(EXECUTABLE)

# Ensure build and bin directories exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Generate parser.tab.c and parser.tab.h (force output in build/)
$(YACC_C_FILE) $(YACC_H_FILE): $(YACC_FILE) | $(BUILD_DIR)
	$(YACC) $(YFLAGS) -o $(YACC_BASE).tab.c $(YACC_FILE)

# Generate lex.yy.c (force output in build/)
$(LEX_C_FILE): $(LEX_FILE) $(YACC_H_FILE) | $(BUILD_DIR)
	$(LEX) -o $(LEX_C_FILE) $(LEX_FILE)

# Compile lex.yy.c into lex.yy.o
$(LEX_O_FILE): $(LEX_C_FILE)
	$(CC) -c $(LEX_C_FILE) -o $(LEX_O_FILE) $(CFLAGS)

# Compile parser.tab.c into parser.tab.o
$(YACC_O_FILE): $(YACC_C_FILE)
	$(CC) -c $(YACC_C_FILE) -o $(YACC_O_FILE) $(CFLAGS)

# Link object files into the final executable
$(EXECUTABLE): $(LEX_O_FILE) $(YACC_O_FILE) | $(BIN_DIR)
	$(CC) -o $(EXECUTABLE) $(LEX_O_FILE) $(YACC_O_FILE) $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR)