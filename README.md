# RPAL Interpreter
This repository contains a lexical analyser and parser for the RPAL language, built for CS3513: Programming Languages.

### Problem Statement

The project requirement was to implement a lexical analyzer and a parser for the
RPAL language. The output of the parser should be the Abstract Syntax Tree (AST) for the
given input program. Then an algorithm must be implemented to convert the Abstract Syntax
Tree (AST) into Standardize Tree (ST) and the CSE machine should be implemented. The
program should be able to read an input file that contains an RPAL program. The output of
the program should match the output of “rpal.exe“ for the relevant program.

### Program Execution Instructions

The following sequence of commands can be used in the root of the project directory
to compile the program and execute rpal programs:
```
> make
> ./rpal20 file_name
```
To generate the Abstract Syntax Tree:
```
> ./rpal20 -ast file_name
```
To generate the Standardized Tree:
```
> ./rpal20 -st file_name
```

### Structure of the Project

This project was coded entirely in C++. It consists of mainly 5 files. They are,
1. main.cpp
2. parser.h
3. tree.h
4. token.h
5. environment.h
   
More information about the purpose of each file and the function prototypes along
with their uses can be found [here](/docs/Group%2046%20-%20PL%20Project%20Report.pdf).
