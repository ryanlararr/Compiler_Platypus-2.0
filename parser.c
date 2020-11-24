#include "parser.h"

/*************************************************************
 * Process Parser
 ************************************************************/
void processParser(void) {
	if (debugMode)
		fprintf(stderr, "[00] Processing parser...\n");
	lookahead = processToken();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");
}

/*************************************************************
 * Match Token
 ************************************************************/
void matchToken(int tokenCode, int tokenAttribute) {
	int matchFlag = 1;
	switch (lookahead.code) {
	case KW_T:
	case REL_OP_T:
	case ART_OP_T:
	case LOG_OP_T:
		// TODO_05
	default:
		// TODO_06
	}
	if (matchFlag && lookahead.code == SEOF_T)
		// TODO_07
	if (matchFlag) {
		// TODO_08
	}
	else
		// TODO_09
}

/*************************************************************
 * Syncronize Error Handler
 ************************************************************/
void syncErrorHandler(int syncTokenCode) {
	// TODO_10
	while (lookahead.code != syncTokenCode) {
	// TODO_11
	}
	if (lookahead.code != SEOF_T)
		// TODO_12
}


/*************************************************************
 * Print Error
 ************************************************************/
void printError() {
	Token t = lookahead;
	if (debugMode)
		fprintf(stderr, "[39] PrintError\n");
	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch (t.code) {
		// TODO_13
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
	if (debugMode)
		fprintf(stderr, "[01] Program\n");
	matchToken(KW_T, PROGRAM);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");
}

// TODO_14: All functions...