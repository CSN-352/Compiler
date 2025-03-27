# Define variables
BUILD_DIR = build
BIN_DIR = build/bin
SRC_DIR = ../src
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
SYMBOL_TABLE_O_FILE = $(BUILD_DIR)/symbol_table.o
AST_O_FILE = $(BUILD_DIR)/ast.o
EXPRESSION_O_FILE = $(BUILD_DIR)/expression.o
# Compiler and flags
LEX = flex
YACC = bison
YFLAGS = -d -v
DEBUG ?= 0
CC = gcc
CXX = g++ -std=c++17
CFLAGS = -lfl -I$(SRC_DIR)
ifeq ($(NAME),aaditya)
    CFLAGS = -ll -I$(SRC_DIR)
endif
ifeq ($(DEBUG),1)
    CFLAGS += -DDEBUG
endif

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

# Compile symbol_table.cpp into symbol_table.o
$(SYMBOL_TABLE_O_FILE): $(SRC_DIR)/symbol_table.cpp $(SRC_DIR)/symbol_table.h | $(BUILD_DIR)
	$(CXX) -c -I$(BUILD_DIR) $(SRC_DIR)/symbol_table.cpp -o $(SYMBOL_TABLE_O_FILE) $(CFLAGS)

# Compile ast.cpp into ast.o
$(AST_O_FILE): $(SRC_DIR)/ast.cpp $(SRC_DIR)/ast.h | $(BUILD_DIR)
	$(CXX) -c -I$(BUILD_DIR) $(SRC_DIR)/ast.cpp -o $(AST_O_FILE) $(CFLAGS)

# Compile expression.cpp into expression.o
$(EXPRESSION_O_FILE): $(SRC_DIR)/expression.cpp $(SRC_DIR)/expression.h | $(BUILD_DIR)
	$(CXX) -c -I$(BUILD_DIR) $(SRC_DIR)/expression.cpp -o $(EXPRESSION_O_FILE) $(CFLAGS)

# Compile lex.yy.c into lex.yy.o
$(LEX_O_FILE): $(LEX_C_FILE)
	$(CXX) -c $(LEX_C_FILE) -o $(LEX_O_FILE) $(CFLAGS)

# Compile parser.tab.c into parser.tab.o
$(YACC_O_FILE): $(YACC_C_FILE)
	$(CXX) -c $(YACC_C_FILE) -o $(YACC_O_FILE)  $(CFLAGS) 

# Link object files into the final executable
$(EXECUTABLE): $(LEX_O_FILE) $(YACC_O_FILE) $(SYMBOL_TABLE_O_FILE) $(AST_O_FILE) $(EXPRESSION_O_FILE)| $(BIN_DIR)
	$(CXX) -o $(EXECUTABLE) $(LEX_O_FILE) $(YACC_O_FILE) $(SYMBOL_TABLE_O_FILE) $(AST_O_FILE) $(EXPRESSION_O_FILE) $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR)
