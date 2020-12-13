/*************************************************************
* COMPILERS COURSE - Algonquin College
* Code version: Fall, 2020
*************************************************************
* File name: parser.c
* Compiler: MS Visual Studio 2019
* Author: Ryan Arreola
* Course: CST 8152 – Compilers, Lab Section: 021
* Assignment: A3
* Date: December 12, 2020
* Professor: Paulo Sousa
* Purpose: Implementation of a recursive descent predictive parser
* Function list: Function list: void formatString(char* ch);optionalStatements(),program(),statements(),statement(),statementsP(), assignment_statement(), assignment_expression(), selection_statement(),iteration_statement(),pre_condition(),input_statement(), variable_list(), variable_listP(), variable_identifier(), output_statement(), output_list(), arithmetic_expression(), unary_arithmetic_expression(), additive_arithmetic_expression(), additive_arithmetic_expressionP(), multiplicative_arithmetic_expression(), multiplicative_arithmetic_expressionP(), primary_arithmetic_expression(), string_expression(), string_expressionP(), primary_string_expression(), conditional_expression(), logical_OR_expression(), logical_OR_expressionP(), logical_AND_expression(), logical_AND_expressionP(), relational_expression(), primary_a_relational_expressionP(), primary_a_relational_expression(), primary_s_relational_expression(), primary_s_relational_expressionP(), processParser(), matchToken(), syncErrorHandler(), printError()
*************************************************************/

#include "parser.h"


/*************************************************************
 * Function name: processParser
 * Purpose: Processess the parser
 * Author: Ryan Arreola
 * Version: 1.0
 * Called functions: processToken(), matchToken(), program(), printf()
 * Parameters: void
 * Return value: n/a
 ************************************************************/
void processParser(void) {
	lookahead = processToken();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");
}

/*************************************************************
 * Function name: matchToken
 * Purpose: Matches the code of the token base don what should be there in the sequence
 * Author: Ryan Arreola
 * Version: 1.0 - November 30, 2020
 * Called functions: syncErrorHandler, processToken, printError
 * Parameters: int tokenCode, int tokenAttribute
 * Return value: void
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

	if (lookahead.code == ERR_T) {
		printError();
		lookahead = processToken();
		syntaxErrorNumber++;
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
	//Implement all the cases to show the error message. The error messages correspond to the token code of the lookahead.
	switch (t.code) {
	case ERR_T:
		printf("%s\n", t.attribute.err_lex);
		break;
	case SEOF_T:
		printf("SEOF_T\t\t%d\t\n", ++t.attribute.get_int);
		break;
	case AVID_T:
	case SVID_T:
		printf("%s\n", t.attribute.vid_lex);
		break;
	case FPL_T: 
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T: 
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:
		printf("%s\n", bufferGetString(stringLiteralTable, t.attribute.str_offset));
		break;
	case KW_T:
		printf("%s\n", keywordTable[t.attribute.get_int]);
		break;
	case ART_OP_T:
	case REL_OP_T:
	case LOG_OP_T:
		printf("%d\n", t.attribute.get_int);
		break;
	case ASS_OP_T:
	case SCC_OP_T:
	case LPR_T: 
	case RPR_T:
	case LBR_T:
	case RBR_T: 
	case COM_T:
	case EOS_T:
		printf("NA\n");
		break;
	default:
		printf("\nPLATY: Scanner error: invalid token code: %d\n", t.code);
	}
}


/*************************************************************
 * 
 * BNF: <program> -> PROGRAM { <opt_statements> }
 * FIRST(<program>)= {KW_T (PROGRAM)}.
 ************************************************************/
void program(void) {
	matchToken(KW_T, PROGRAM);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");
}


/*************************************************************
 * Optional Statements
 * BNF: <opt_statements> ->  <statements> | ϵ
 * FIRST(<opt_statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT), ϵ }
 ************************************************************/
void optionalStatements(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		statements();
		break;
	case KW_T:
		if (lookahead.attribute.get_int == IF
			|| lookahead.attribute.get_int == WHILE
			|| lookahead.attribute.get_int == INPUT
			|| lookahead.attribute.get_int == OUTPUT) {
			statements();
			break;
		}
	default:
		printf("%s\n", "PLATY: Opt_statements parsed");
	}
}

void statements(void) {
	statement();
	statementsP();
}

/*************************************************************
 * Statement
 * BNF: <statement> ->  <assignment statement> | <selection statement> | <iteration statement> 
| <input statement> | <output statement>
 * FIRST (<statement>) = { AVID_T, SVIT_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT) }
 ************************************************************/
void statement(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		assignment_statement();
		break;
	case KW_T:
		switch (lookahead.attribute.get_int) {
		case IF: 
			selection_statement(); 
			break;
		case WHILE: 
			iteration_statement(); 
			break;
		case INPUT: 
			input_statement(); 
			break;
		case OUTPUT: 
			output_statement(); 
			break;
		}
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * Program Statements'
 * BNF: <statementsPrime> -> <statement><statementsPrime> | ϵ
 * FIRST (<statements’>) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT), KW_T(OUTPUT), ϵ }
 ************************************************************/
void statementsP(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T:
		statement();
		statementsP();
		break;
	case KW_T:
		if (lookahead.attribute.get_int != PROGRAM
			&& lookahead.attribute.get_int != ELSE
			&& lookahead.attribute.get_int != DO
			&& lookahead.attribute.get_int != THEN
			&& lookahead.attribute.get_int != TRUE
			&& lookahead.attribute.get_int != FALSE){
			statement();
			statementsP();
			break;
		}
	default:
		break;
	}
}

/*************************************************************
 * Assignment Statement
 * BNF: <assignment statement> -> <assignment expression>
 * FIRST(<assignment statement>) = {AVID_T, SVID_T}
 ************************************************************/
void assignment_statement(void) {
	assignment_expression();
	matchToken(EOS_T, NO_ATTR);
	formatString("PLATY: Assignment statement parsed");
}

/*************************************************************
 * Assignment Expression
 * BNF: <assignment expression> ->  AVID = <arithmetic expression> | SVID = <string expression>
 * FIRST(<assignment expression>) = {AVID_T, SVID_T}
 ************************************************************/
void assignment_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
		matchToken(AVID_T, NO_ATTR);
		matchToken(ASS_OP_T, EQ);
		arithmetic_expression();
		formatString("PLATY: Assignment expression (arithmetic) parsed");
		break;
	case SVID_T:
		matchToken(SVID_T, NO_ATTR);
		matchToken(ASS_OP_T, EQ);
		string_expression();
		formatString("PLATY: Assignment expression (string) parsed");
		break;
	default:
		printError();
		break;
	}
}

/*************************************************************
 * Selection Statement
 * BNF: <selection statement> ->  IF <pre-condition> (<conditional expression>)
 		THEN { <opt_statements> }
   		ELSE { <opt_statements> };

 * FIRST(<selection statement>) = { KW_T(IF) }
 ************************************************************/
void selection_statement(void) {
	matchToken(KW_T, IF);
	pre_condition();
	matchToken(LPR_T, NO_ATTR);
	conditional_expression();
	matchToken(RPR_T, NO_ATTR);
	matchToken(KW_T, THEN);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(KW_T, ELSE);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	formatString("PLATY: Selection statement parsed");
}

/*************************************************************
 * Iteration Statement
 * BNF: <iteration statement> -> WHILE <pre-condition> (<conditional expression>)
      DO { <statements>};
 * FIRST(<iteration statement>) = { KW_T(WHILE) }
 ************************************************************/
void iteration_statement(void) {
	matchToken(KW_T, WHILE);
	pre_condition();
	matchToken(LPR_T, NO_ATTR);
	conditional_expression();
	matchToken(RPR_T, NO_ATTR);
	matchToken(KW_T, DO);
	matchToken(LBR_T, NO_ATTR);
	statements();
	matchToken(RBR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	formatString("PLATY: Iteration statement parsed");
}

/*************************************************************
 * Pre-condition
 * BNF: <pre-condition> -> TRUE | FALSE
 * FIRST(<pre-condition>) = { KW_T(TRUE), KW_T(FALSE) }
 ************************************************************/
void pre_condition(void) {
	switch (lookahead.code) {
	case KW_T:
		switch (lookahead.attribute.get_int) {
		case TRUE: 
			matchToken(KW_T, TRUE);
			break;
		case FALSE: 
			matchToken(KW_T, FALSE);
			break;
		default: 
			printError(); 
			break;
		}
		break;
	default: 
		printError(); 
		break;
	}
}

/*************************************************************
 * Input Statement
 * BNF: <input statement>  INPUT (<variable list>);
 * FIRST (<input statements>) = { KW_T(INPUT) }
 ************************************************************/
void input_statement(void) {
	matchToken(KW_T, INPUT);
	matchToken(LPR_T, NO_ATTR);
	variable_list();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	formatString("PLATY: Input statement parsed");
}

/*************************************************************
 * Variable list
 * BNF: <variable list>  <variable identifier> <variable list’>
 * FIRST(<variable list>) = { AVID_T, SVID_T }
 ************************************************************/
void variable_list(void) {
	variable_identifier();
	variable_listP();
	formatString("PLATY: Variable list parsed");
}

/*************************************************************
 * Variable List Prime
 * BNF: <variable list’>  ,<variable identifier> <variable list’> | ϵ
 * FIRST(<variable list’>) = { COM_T, ϵ }
 ************************************************************/
void variable_listP(void) {
	switch (lookahead.code) {
	case COM_T: 
		matchToken(COM_T, NO_ATTR);
		variable_identifier(); 
		variable_listP();
		break;
	}
}

/*************************************************************
 * Vriable Identifier
 * BNF: <variable identifier>  AVID | SVID
 * FIRST(<variable identifier>) = { AVID_T, SVID_T }
 ************************************************************/
void variable_identifier(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: 
		matchToken(lookahead.code, NO_ATTR);
		break;
	default: printError(); 
		break;
	}
}

/*************************************************************
 * Output statement
 * BNF: <output statement>  OUTPUT (<output statementPrime>);
 * FIRST(<output_list>) = { KW_T(INPUT) }
 ************************************************************/
void output_statement(void) {
	matchToken(KW_T, OUTPUT);
	matchToken(LPR_T, NO_ATTR);
	output_list();
	matchToken(RPR_T, NO_ATTR);
	matchToken(EOS_T, NO_ATTR);
	formatString("PLATY: Output statement parsed");
}

/*************************************************************
 * Output list
 * FIRST(<output_list>) = { KW_T(INPUT) }
 ************************************************************/
void output_list(void) {
	switch (lookahead.code) {
	case AVID_T:
	case SVID_T: 
		variable_list();
		break;
	case STR_T: 
		matchToken(STR_T, NO_ATTR);
		formatString("PLATY: Output list (string literal) parsed");
		break;
	default:
		formatString("PLATY: Output list (empty) parsed");
		break;
	}
}


/*************************************************************
 * Arithmetic Expression
 * BNF: <arithmetic expression>   <unary arithmetic expression> | <additive arithmetic expression>	
 * FIRST(<arithmetic expression>) = { -, +, AVID_T, FPL_T, INL_T, ( }
 ************************************************************/
void arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case ADD:
		case SUB:
			unary_arithmetic_expression();
			break;
		default: 
			printError();
			break;
		}
		break;
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T: 
		additive_arithmetic_expression();
		break;
	default: 
		printError();
		break;
	}
	formatString("PLATY: Arithmetic expression parsed");
}


/*************************************************************
 * Unary Arithmetic Expression
 * BNF: <unary arithmetic expression>  - <primary arithmetic expression> 
 				| + <primary arithmetic expression>

 * FIRST(<unary arithmetic expression>) = { -, + }
 ************************************************************/
void unary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case ADD:
			matchToken(ART_OP_T, ADD);
			primary_arithmetic_expression();
			break;
		case SUB:
			matchToken(ART_OP_T, SUB);
			primary_arithmetic_expression();
			break;
		default: 
			printError(); 
			break;
		}
	}
	formatString("PLATY: Unary arithmetic expression parsed");
}


/*************************************************************
 * Additive Arithmetic Expression
 * BNF: <additive arithmetic expression>  <multiplicative arithmetic expression><additive arithmetic expression’>
 * FIRST(<additive arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
 ************************************************************/
void additive_arithmetic_expression(void) {
	multiplicative_arithmetic_expression();
	additive_arithmetic_expressionP();
}


/*************************************************************
 * Additive Arithmetic Expression
 * BNF: <additive arithmetic expression>  <multiplicative arithmetic expression><additive arithmetic expression’>
 * FIRST(<additive arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
 ************************************************************/
void additive_arithmetic_expressionP(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case ADD:
		case SUB:
			matchToken(ART_OP_T, lookahead.attribute.arr_op);
			multiplicative_arithmetic_expression();
			additive_arithmetic_expressionP();
			formatString("PLATY: Additive arithmetic expression parsed");
			break;
		default:
			break;
		}
	}
}


/*************************************************************
 * Multiplicative Arithmetic Expression
 * BNF: <additive arithmetic expression’>  + <multiplicative arithmetic expression><additive arithmetic expression’> | - <multiplicative arithmetic expression><additive arithmetic expression’> | ϵ
 * FIRST(<additive arithmetic expression’>) = { +, -, ϵ }
 ************************************************************/
void multiplicative_arithmetic_expression(void) {
	primary_arithmetic_expression();
	multiplicative_arithmetic_expressionP();
}


/*************************************************************
 * Multiplicative Arithmetic Expression Prime
 * BNF: <multiplicative arithmetic expression’>  * <primary arithmetic expression><multiplicative arithmetic>
 * expression’> | / <primary arithmetic expression><multiplicative arithmetic expression’> | ϵ
 ************************************************************/
void multiplicative_arithmetic_expressionP(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		switch (lookahead.attribute.arr_op) {
		case MUL:
		case DIV:
			matchToken(ART_OP_T, lookahead.attribute.arr_op);
			primary_arithmetic_expression();
			multiplicative_arithmetic_expressionP();
			formatString("PLATY: Multiplicative arithmetic expression parsed");
			break;
		default:
			break;
		}
	}
}


/*************************************************************
 * Primary Arithmetic Expression
 * BNF: <primary arithmetic expression>  AVID_T | FPL_T | INL_T
 					| (<arithmetic expression>)

 * FIRST(<primary arithmetic expression>) = { AVID_T, FPL_T, INL_T, ( }
 ************************************************************/
void primary_arithmetic_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		matchToken(lookahead.code, NO_ATTR);
		break;
	case LPR_T:
		matchToken(LPR_T, NO_ATTR);
		arithmetic_expression();
		matchToken(RPR_T, NO_ATTR);
		break;
	default: 
		printError(); 
		break;
	}
	formatString("PLATY: Primary arithmetic expression parsed");
}


/*************************************************************
 * String Expression
 * BNF: <string expression>  <primary string expression> | <primary string expression’>
 * FIRST(<string expression>) = {SVID_T, STR_T}
 * 
 ************************************************************/
void string_expression(void) {
	primary_string_expression();
	string_expressionP();
	formatString("PLATY: String expression parsed");
}


/*************************************************************
 * String Expression Prime
 * BNF: <string expression’> $$ <primary string expression><string expression’> | ϵ
 * FIRST (<string expression’>) = { $$, ϵ }
 ************************************************************/
void string_expressionP(void) {
	switch (lookahead.code) {
	case SCC_OP_T:
		matchToken(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		string_expressionP();
		break;
	default:
		break;
	}
}


/*************************************************************
 * Primary String Expression
 * BNF: <primary string expression>    SVID_T  |  STR_T
 * FIRST(<primary string expression>) = { SVID_T, STR_T }
 ************************************************************/
void primary_string_expression(void) {
	switch (lookahead.code) {
	case SVID_T:
	case STR_T: 
		matchToken(lookahead.code, NO_ATTR);
		break;
	default:
		printError(); 
		break;
	}
	formatString("PLATY: Primary string expression parsed");
}


/*************************************************************
 * Conditional Expression
 * BNF: <conditional expression>   <logical OR  expression>
 * FIRST(<conditional expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
 ************************************************************/
void conditional_expression(void) {
	logical_OR_expression();
	formatString("PLATY: Conditional expression parsed");
}


/*************************************************************
 * Logical OR Expression
 * BNF: <logical OR expression>  <logical AND expression><logical OR expression’>
 * FIRST(<logical OR expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T}
 ************************************************************/
void logical_OR_expression(void) {
	logical_AND_expression();
	logical_OR_expressionP();
}


/*************************************************************
 * Logical OR Expression Prime
 * BNF: <logical OR expression’>  _OR_ <logical AND expression><logical OR expression’> | ϵ
 * FIRST(<logical OR expression’>) = { _OR_, ϵ }
 ************************************************************/
void logical_OR_expressionP(void) {
	switch (lookahead.code) {
	case LOG_OP_T:
		switch (lookahead.attribute.log_op) {
		case OR:
			matchToken(LOG_OP_T, OR);
			logical_AND_expression();
			logical_OR_expressionP();
			formatString("PLATY: Logical OR expression parsed");
			break;
		}
		break;
	}
}


/*************************************************************
 * Logical AND Expression
 * BNF: <logical AND expression>  <relational expression><logical AND expression’>
 * FIRST (<logical AND expression>) = {AVID_T, FPL_T, INL_T, SVID_T, STR_T}
 ************************************************************/
void logical_AND_expression(void) {
	relational_expression();
	logical_AND_expressionP();
}


/*************************************************************
 * Logical AND Expression Prime
 * BNF: <logical AND expresison’>  _AND_ <relational expression><logical AND expression’> | ϵ
 * FIRST(<logical AND expression’>) = { _AND_, ϵ }
 ************************************************************/
void logical_AND_expressionP(void) {
	switch (lookahead.code) {
	case LOG_OP_T:
		switch (lookahead.attribute.log_op) {
		case AND:
			matchToken(LOG_OP_T, AND);
			relational_expression();
			logical_AND_expressionP();
			formatString("PLATY: Logical AND expression parsed");
			break;
		}
		break;
	}
}


/*************************************************************
 * Relational Expression
 * BNF: <relational expression> -> <primary a_relational expression> <primary a_relational expression’>
 | <primary s_relational expression><primary s_relational expression’>
 * FIRST(<relational expression>) = { AVID_T, FPL_T, INL_T, SVID_T, STR_T }
 ************************************************************/
void relational_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T:
		primary_a_relational_expression();
		primary_a_relational_expressionP();
		break;
	case SVID_T:
	case STR_T:
		primary_s_relational_expression();
		primary_s_relational_expressionP();
		break;
	default: printError();
		break;
	}
	formatString("PLATY: Relational expression parsed");
}


/*************************************************************
 * Primary a relational expression
 * BNF: <primary a_relational expression>   AVID_T  | FPL_T  | INL_T
 * FIRST(<primary a_relational expression>) = { AVID_T, FPL_T, INL_T}
 ************************************************************/
void primary_a_relational_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
	case FPL_T:
	case INL_T: 
		matchToken(lookahead.code, NO_ATTR);
		break;
	default:
		printError();
		break;
	}
	formatString("PLATY: Primary a_relational expression parsed");
}

/*************************************************************
 * Primary a relational expression prime
 * BNF: <primary a_relational expression’> ->
== <primary a_relational expression>
| <> <primary a_relational expression>
| > <primary a_relational expression>
| < <primary a_relational expression>
 * FIRST(<primary a_relational expression’>) = { ==, <>, >, < }
 ************************************************************/
void primary_a_relational_expressionP(void) {
	switch (lookahead.code)
	{
	case REL_OP_T:
		switch (lookahead.attribute.rel_op)
		{
		case EQ:
		case NE:
		case GT:
		case LT:
			matchToken(REL_OP_T, lookahead.attribute.rel_op);
			primary_a_relational_expression();
			break;
		default:
			printError();
			break;
		}
		break;
	default:
		printError(); break;
	}
}

/*************************************************************
 * Primary s realtional expression
 * BNF: <primary s_relational expression>  <primary string expression>
 * FIRST(<primary s_relational expression >) = { SVID_T, STR_T }
 ************************************************************/
void primary_s_relational_expression(void) {
	primary_string_expression();
	formatString("PLATY: Primary s_relational expression parsed");
}


/*************************************************************
 * Primary s relational expression prime
 * BNF: <primary a_relational expression’> ->
== <primary a_relational expression>
| <> <primary a_relational expression>
| > <primary a_relational expression>
| < <primary a_relational expression>
 * FIRST(<primary a_relational expression’>) = { ==, <>, >, < }
 ************************************************************/
void primary_s_relational_expressionP(void) {
	switch (lookahead.code) {
	case REL_OP_T:
		switch (lookahead.attribute.rel_op)
		{
		case EQ:
		case NE:
		case GT:
		case LT:
			matchToken(REL_OP_T, lookahead.attribute.rel_op);
			primary_s_relational_expression();
			break;
		default:
			printError();
			break;
		}
		break;
	default:
		printError(); break;
	}
}

/*
* Purpose: Formats outputs to have a line break at the end.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: printf()
* Parameters: char* ch pointer to characters
* Return value: n/a
*/
void formatString(char* ch) {
	printf("%s\n", ch);
}