/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2020
*************************************************************
* File name: parser.h
* Compiler: MS Visual Studio 2019
* Author: Ryan Arreola
* Course: CST 8152 – Compilers, Lab Section: 021
* Assignment: A3
* Date: December 12, 2020
* Professor: Paulo Sousa
* Purpose: This file is the main header for Parser.c
* Function list: void formatString(char* ch);optionalStatements(),program(),statements(),statement(),statementsP(), assignment_statement(), assignment_expression(), selection_statement(),iteration_statement(),pre_condition(),input_statement(), variable_list(), variable_listP(), variable_identifier(), output_statement(), output_list(), arithmetic_expression(), unary_arithmetic_expression(), additive_arithmetic_expression(), additive_arithmetic_expressionP(), multiplicative_arithmetic_expression(), multiplicative_arithmetic_expressionP(), primary_arithmetic_expression(), string_expression(), string_expressionP(), primary_string_expression(), conditional_expression(), logical_OR_expression(), logical_OR_expressionP(), logical_AND_expression(), logical_AND_expressionP(), relational_expression(), primary_a_relational_expressionP(), primary_a_relational_expression(), primary_s_relational_expression(), primary_s_relational_expressionP(), processParser(), matchToken(), syncErrorHandler(), printError()
*************************************************************/

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"
#include "buffer.h"

/* Global vars */
static Token lookahead;
int syntaxErrorNumber;

extern Buffer* stringLiteralTable;
extern int line;
extern Token processToken();
extern char* keywordTable[];

/* Constants */
enum Keywords{ NO_ATTR = -1, DO, ELSE, FALSE, IF, INPUT, OUTPUT, PROGRAM, THEN, TRUE, WHILE };

/* Function definitions */
void processParser(void);
void matchToken(int, int);
void syncErrorHandler(int);
void printError();

// TODO_04 All FunctionsForNonTerminals
void formatString(char* ch);
void optionalStatements(void);
void program(void);
void statements(void);
void statement(void);
void statementsP(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void pre_condition(void);
void input_statement(void);
void variable_list(void);
void variable_listP(void);
void variable_identifier(void);
void output_statement(void);
void output_list(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expressionP(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expressionP(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expressionP(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_OR_expression(void);
void logical_OR_expressionP(void);
void logical_AND_expression(void);
void logical_AND_expressionP(void);
void relational_expression(void);
void primary_a_relational_expressionP(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);
void primary_s_relational_expressionP(void);