/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2020
*************************************************************
* File name: parser.h
* Compiler: MS Visual Studio 2019
* Author: Ryan Arreola
* Course: CST 8152 – Compilers, Lab Section: 021
* Assignment: A3
* Date: Sep 01 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: This file is the main header for Parser.h
* Function list: 
*************************************************************/

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"
#include "buffer.h"

/* Global vars */
// TODO_01: lookahead
// TODO_02: syntaxErrorNumber
extern Buffer* stringLiteralTable;
extern int line;
extern Token processToken();
extern char* keywordTable[];

/* Constants */
#define	NO_ATTR (-1)
// TODO_03: Create ALL constants for all keywords
// Example: #define KEYWORD1 CONSTANTVALUE1
//...

/* Function definitions */
void processParser(void);
void matchToken(int, int);
void syncErrorHandler(int);
void printError();

// TODO_04 All FunctionsForNonTerminals
void program(void);
//...
