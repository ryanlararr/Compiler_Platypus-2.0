/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2020
*************************************************************
* File name: parser.c
* Compiler: MS Visual Studio 2019
* Author: Ryan Arreola
* Course: CST 8152 – Compilers, Lab Section: 021
* Assignment: A3
* Date: Sep 01 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: 
* Function list:
*************************************************************/

#include "parser.h"


/*************************************************************
 * Function name: processParser
 * Purpose:
 * Author: Ryan Arreola
 * Version: 1.0 -
 * Called functions:
 * Parameters:
 * Return value:
 * Algorithm:
 ************************************************************/
void processParser(void) {
	lookahead = processToken();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");
}

/*************************************************************
 * Function name: matchToken
 * Purpose:
 * Author: Ryan Arreola
 * Version: 1.0 - November 30, 2020
 * Called functions:
 * Parameters:
 * Return value:
 * Algorithm:
 ************************************************************/
void matchToken(int tokenCode, int tokenAttribute) {
	int matchFlag = 1;
	switch (lookahead.code) {
		//The attribute code is used only when the token code is one of the following codes
	case KW_T:
	case REL_OP_T:
	case ART_OP_T:
	case LOG_OP_T:
		//tokenAttribute should be the same value as the corresponding lookahead attribute as assigned by the keywordTable[]
		if (tokenAttribute != lookahead.attribute.get_int) {
			syncErrorHandler(tokenCode);
			return;
		}
		//In all other cases the token code is matched only check if the tokencode matches the lookahead code
	default:
		if (tokenCode != lookahead.code) {
			syncErrorHandler(tokenCode);
			return;
		}
	}
	if (matchFlag && lookahead.code == SEOF_T) {
		//If the match is successful and the lookahead is SEOF_T the function returns
		return;
	}

	if (matchFlag) {
		//If the match is successful and the lookahead is not SEOF_T, the function advances to the next input token by executing the statement:
		//lookahead = processToken()
		lookahead = processToken();
	}
	//If no matching is found, you must call the syncErrorHandler() function.
	else {
		syncErrorHandler(tokenCode);
		return;
	}


}

/*************************************************************
 * Function Name: syncErrorHandler
 * Purpose:
 * Author: Ryan Arreola
 * Version: 1.0 - November 30, 2020
 * Called functions: printError, processToken, exit
 * Parameters:
 * Return value: void
 * Algorithm:
	* Advances the input token until it finds a token code matching the one required by the parser. If the current lookahead code is a source end of file token, but the token required by the parser is not SEOF then this is an error that exits the program.
	* In the case that the lookahead token code is the same as the one required by the parser but not the SEOF then advance the lookahead. This is to prevent overrunning the input buffer.
 ************************************************************/
void syncErrorHandler(int syncTokenCode) {
	printError();
	syntaxErrorNumber++;

	while (lookahead.code != syncTokenCode) {
		lookahead = processToken();
		if (lookahead.code == SEOF_T) {
			if (syncTokenCode != SEOF_T) {
				exit(syntaxErrorNumber);
			}
			else {
				return;
			}
		}
	}
	if (lookahead.code != SEOF_T) {
		lookahead = processToken();
	}
}


/*************************************************************
 * Function name: printError
 * Purpose:
 * Author: Ryan Arreola
 * Version: 1.0 -
 * Called functions:
 * Parameters:
 * Return value:
 * Algorithm:
 ************************************************************/
void printError() {
	Token t = lookahead;
	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	// TODO_13
	//Implement all the casers to show the error message. The error messages correspond to the token code of the lookahead.
	switch (t.code) {
		
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}
}


/*************************************************************
 * Program statement
 * BNF: <program> -> PLATYPUS { <opt_statements> }
 * FIRST(<program>)= {KW_T (PLATYPUS)}.
 ************************************************************/
void program(void) {
	matchToken(KW_T, PROGRAM);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");
}

// TODO_14: All functions...

/*************************************************************
 * Function name: optionalStatements
 * Purpose:
 * Author: Ryan Arreola
 * Version: 1.0 -
 * Called functions:
 * Parameters:
 * Return value:
 * Algorithm:
 ************************************************************/
void optionalStatements(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		//TODO: non-terminal definition
		break;
	case KW_T:
		if (lookahead.attribute.get_int == IF
			|| lookahead.attribute.get_int > WHILE
			|| lookahead.attribute.get_int > INPUT
			|| lookahead.attribute.get_int > OUTPUT) {
			//TODO: non-terminal definition
			break;
		}
	default:
		printf("%s\n", "PLATY: Opt_statements parsed");
	}
}