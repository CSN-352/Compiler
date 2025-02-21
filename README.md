# Syntax Analyzer / Parser

## Project Overview
This project is a **Syntax Analyzer / Parser** designed to validate and analyze the syntax of programs written in a **custom programming language**. The language syntax is **inspired by C**, but also includes additional features such as **class-based object-oriented programming, function overloading, and new control structures**. The parser is implemented using **C++** and utilizes **LALR(1) parsing techniques**.

- **Source Language:** Custom Language (C-like with extended features)  
- **Implementation Language:** C++  
- **Parsing Method:** LALR(1)  

---

## Features

### Basic Features
The syntax analyzer supports the following basic features:

- **Arithmetic and Logical Operators**
- **Control Flow Statements:**
  - `if-else`
  - `for` loop
  - `while` loop
  - `do-while` loop
  - `switch` cases
- **Data Structures:**
  - Arrays
  - Pointers
  - Structures
- **Standard I/O:**
  - `printf` and `scanf`
- **Functionality:**
  - Function calls with arguments
  - `goto`, `break`, and `continue`
  - `static` keyword

---

### Advanced Features
The language includes the following advanced features:

- **Recursive Function Calls**
- **Class and Object Support**
- **Dynamic Memory Allocation**
- **Function Pointers**
- **Command Line Input**
- **Access Modifiers:** `public`, `private`, and `protected`
- **Typedef**
- **References**
- **Enum and Union**
- **File Manipulation**
- **Until Loop**
- **Multi-level Pointers**
- **Multi-dimensional Arrays**
- **Function call with variable arguments**

---

## Class Structure and Object Declaration
The language supports class-based programming with a **block-based access control system**. Instead of defining access modifiers before individual variables or functions, they are grouped in specific blocks.

### Example:
```cpp
class Basic_Class {
    private {
        int a;
        int b;
    }
};

class Basic_Class obj;
```

### Key Features:
- Classes contain **private, public, and protected blocks** to define access control.
- Multiple such blocks can exist in a single class.
- **Function definitions are replaced with function pointers**, ensuring a structured and controlled execution model.
- Objects of a class or struct **must** be declared using a similar syntax.

This approach enforces strict encapsulation while providing flexibility in function handling.

---

## Errors Detected in Syntax Analysis Phase
The syntax analyzer detects the following errors:

- **Missing or Mismatched Brackets:** `{}`, `()`, `[]`
- **Incorrectly Structured `if-else` or Loop Statements**
- **Invalid Function Definitions**
- **Undefined or Incorrectly Used Identifiers**
- **Misplaced Return Statements**
- **Duplicate Function/Variable Declarations**
- **Incorrect Class Definitions**
- **Conflicting Access Modifiers**
- **Incorrect Loop Syntax**

The analyzer provides meaningful **error messages with line numbers** to aid debugging.

---

## Setup Instructions
Follow the steps below to set up and run the syntax analyzer:

### 1. Install Dependencies
#### For macOS (using Homebrew):
```bash
brew install flex bison
```
#### For Linux (Ubuntu/Debian):
```bash
sudo apt install flex bison build-essential
```
#### For Windows (using MinGW or WSL):
Ensure that `flex`, `bison`, and `g++` are installed.

### 2. Navigate to the Source Directory
```bash
cd /path/to/your/project/src
```

### 3. Build the Project
```bash
make
```

---

## Test Instructions
To run tests on the syntax analyzer, follow these steps:

### 1. Navigate to the Test Directory
```bash
cd /path/to/your/project/test
```

### 2. Make the Run Script Executable
```bash
chmod +x run.sh
```

### 3. Execute the Run Script
```bash
./run.sh
```

---

## Authors
- **Aaditya Aren**
- **Divyansh Jain**
- **Siya Arora**
- **Sukhman Singh**

