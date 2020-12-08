/*******************************************************************
 * Filename: scanner.c
 * PURPOSE:
 *    Functions implementing a Lexical Analyzer (Scanner)
 *	Author: Ryan Arreola
 *	Course: Compilers CST8152
 *	Assignment: 2
 *	Date: November 13, 2020
 *	Professor: Paulo Sousa
 *	Purpose: Implementing front-end compilation of a lexical analyzer.
 * 
 *	Function list: aStateFuncAVID(), aStateFuncErr(), aStateFuncErrorRet(), aStateFuncFPL(), aStateFuncIL(), aStateFuncSL(), aStateFuncSVID(), getNextState(), initScanner(), isKeyword(), nextTokenClass(), processToken(), free(), bufferGetString(), bufferCreate(), 
 *******************************************************************
 */

 /* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
  * to suppress the warnings about using "unsafe" functions like fopen()
  * and standard sting library functions defined in string.h.
  * The define does not have any effect in Borland compiler projects.
  */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

  /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer stringLiteralTable;		/* String literal table */
int line;								/* current line number of the source code */
extern int errorNumber;					/* defined in platy_st.c - run-time error number */

static char debugMode = 0;				/* optional for debugging */

/* Local(file) global objects - variables */
static pBuffer lexemeBuffer;			/* pointer to temporary lexeme buffer */
static pBuffer sourceBuffer;			/* pointer to input source buffer */
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int nextTokenClass(char c);		/* character class function */
static int getNextState(int, char);		/* state machine function */
static int isKeyword(char* kw_lexeme);	/* keywords lookup functuion */

/* TODO_02: Follow the standard and adjust all function headers */

/*
* Purpose: Initializes scanner, sets global variables.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: bufferIsEmpty(), bufferRewind(), bufferClear()
* Parameters: pBuffer psc_buf
* Return value: int exit code
* Algorithm: 
	In the event that buffer is empty: Return a failure exit value. 
	In the event of a success:
		- Rewind the buffer to position 0, clear everything to default.
		- Set the global line variable to 1, and the global source buffer to the given buffer entity.
*/
int initScanner(pBuffer psc_buf) {
	if (bufferIsEmpty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	bufferRewind(psc_buf);
	bufferClear(stringLiteralTable);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS;/*0*/
/*   scerrnum = 0;  *//*no need - global ANSI C */
}


/*
* Purpose: Processes tokens through a hybrid of token-driven and transition-table driven implementation
* Author: Ryan Arreola
* Version: 1.0
* Called functions: bufferGetChar(), isspace(), bufferRetract(), bufferSetMarkPosition(), bufferGetCPosition(), bufferReset(), isalnum(), buffeSetEnd(), bufferAddChar()
* Parameters: void
* Return value: Token: The currently processed token.
* Algorithm: Check for whitespace and file terminators. After doing so, go through token driven scanning - if a match is not found then proceed to use transition table driven scanning. Once a token or error is recognizxed set the token to the appropriate code and attributes and return the token.
*/
Token processToken(void) {
	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;	/* input symbol */
	int state = 0;		/* initial state of the FSM */
	short lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	short lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	int lexLength;		/* token length */
	int i;				/* counter */
	unsigned char newc;	/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */
		c = bufferGetChar(sourceBuffer);

		//Account for whitespace - PLATYPUS skips whitespace.
		//If the space is a newline type then increment the global line counter
		if (isspace(c)) {
			if (c == '\n') {
				line++;
			}
			continue;
		}

		//Check if current symbol is a char representing SEOF
		//SEOF0 represents a regular EOF
		//CHARSEOF255 represents a error during the reading of chars and premature termination
		if (c == CHARSEOF0) {
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_0;
			return currentToken;
		}
		else if (c == CHARSEOF255){
			currentToken.code = SEOF_T;
			currentToken.attribute.seof = SEOF_EOF;
			return currentToken;
		}

		/*Token driven scanner*/
		switch (c) {
		
		/*Separators (not including quotes; used in TT)*/
		case '(':
			currentToken.code = LPR_T;
			return currentToken;
		case ')':
			currentToken.code = RPR_T;
			return currentToken;
		case '{':
			currentToken.code = LBR_T;
			return currentToken;
		case '}':
			currentToken.code = RBR_T;
			return currentToken;
		case ';':
			currentToken.code = EOS_T;
			return currentToken;
		case ',':
			currentToken.code = COM_T;
			return currentToken;

		/*Arithmetic operators - not including '='*/
		case '-':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = SUB;
			return currentToken;
		case '+':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = ADD;
			return currentToken;
		case '*':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = MUL;
			return currentToken;
		case '/':
			currentToken.code = ART_OP_T;
			currentToken.attribute.arr_op = DIV;
			return currentToken;

		/*Relational operators*/
		case '>':
			currentToken.code = REL_OP_T;
			currentToken.attribute.rel_op = GT;
			return currentToken;
		//Handling the <> case in PLATYPUS: Translates to != in other languages
		//If the symbol read after the current 'c' symbol is GT  then it is the NE operator
		case '<':
			currentToken.code = REL_OP_T;
			if (bufferGetChar(sourceBuffer) == '>') {
				currentToken.attribute.rel_op = NE;
				return currentToken;
			}
			bufferRetract(sourceBuffer);
			currentToken.attribute.rel_op = LT;
			return currentToken;
		case '_':
			//Mark the first character after '_' to check the next symbols
			bufferSetMarkPosition(sourceBuffer, bufferGetCPosition(sourceBuffer));
			c = bufferGetChar(sourceBuffer);

			//Check if equal to _NOT_ / _AND_ / _OR_
			if (c == 'N' && bufferGetChar(sourceBuffer) == 'O' && bufferGetChar(sourceBuffer) == 'T' && bufferGetChar(sourceBuffer) == '_'){
				currentToken.code = LOG_OP_T;
				currentToken.attribute.log_op = NOT;
				return currentToken;
			}
			else if (c == 'A' && bufferGetChar(sourceBuffer) == 'N' && bufferGetChar(sourceBuffer) == 'D' && bufferGetChar(sourceBuffer) == '_') {
				currentToken.code = LOG_OP_T;
				currentToken.attribute.log_op = AND;
				return currentToken;
			}
			else if (c == 'O' && bufferGetChar(sourceBuffer) == 'R' && bufferGetChar(sourceBuffer) == '_') {
				currentToken.code = LOG_OP_T;
				currentToken.attribute.log_op = OR;
				return currentToken;
			}
			else {
				bufferReset(sourceBuffer);
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = '_';
				return currentToken;
			}

		/*********Sequence based tokens********/

		/*Comment operator*/
		case '!':
			//Hold the character after the current '!' character in a temporary variable newc
			newc = c = bufferGetChar(sourceBuffer);

			//If the comment construct is not lexically correct then must return an error token. According to the frammar the only character after the initial '!' must be another '!'

			//If the subsequent character is a SEOF then return an error token
			//Attribute of the error token is the lexeme content itself stored in err_lex.
			if (newc == CHARSEOF255 || newc == CHARSEOF0) {
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = '!';
				currentToken.attribute.err_lex[1] = CHARSEOF0;
				bufferRetract(sourceBuffer);
				return currentToken;
			}
			//If the subsequent character is not SEOF or the appropriate '!' then add the current invalid symbol to the error lexeme after the '!' and before the EOF.
			//If the invalid charcter is a space then do not set it in the error lexeme.
			else if (newc != '!') {
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = '!';
				if (newc != '\n') {
					currentToken.attribute.err_lex[1] = c;
				}
				currentToken.attribute.err_lex[2] = CHARSEOF0;
			}
			//In the case that the above is not true, and that the subsequent character is a '!', scan through the buffer ignoring whitespace until the end of the line - handling SEOF errors. 
			while (newc != '\n'){
				newc = bufferGetChar(sourceBuffer);
				if (newc == CHARSEOF255 || newc == CHARSEOF0) {
					//Conforming with requirements to pass testfile a214com.pls and Platypus lexical grammar.
					//Grammar specifications IMPLICITLY state that EOF should not but in a comment string nor should it be able to be used as a regular linebreak.
					currentToken.code = ERR_T;
					currentToken.attribute.err_lex[0] = '!';
					currentToken.attribute.err_lex[1] = '!';
					return currentToken;
				}
			}
			//If the lexical analyzer has made it to this point then a newline has been reached and must be updated.
			line++;
			if (c != '!') {
				return currentToken;
			}
			continue;

		case '=':
			//Account for difference between == relational operator and = assignment operator
			//Check if the next character is '='. If it isn't then retract and assign current symbol as a assignment operation token.
			if (bufferGetChar(sourceBuffer) == '='){
				currentToken.code = REL_OP_T;
				currentToken.attribute.rel_op = EQ;
				return currentToken;
			}
			else {
				bufferRetract(sourceBuffer);
				currentToken.code = ASS_OP_T;
				return currentToken;
			}
		case '$':
			//Account for difference between $$ concatenation and $ SVID identifier
			//Check if the next character is a $ then it is a concatenation token.
			//If the next character is not $ then the token could be the end of an SVID so not necessarily an error.

			newc = bufferGetChar(sourceBuffer);
			if (newc == '$') {
				currentToken.code = SCC_OP_T;
				return currentToken;
			}
		default:
			//Valid state 0 in TT is (L | Z | N | Q) everything else is an error. Specifically, if there's an EOF token for state 0 it is immediately an ER. 
			if (!(isalnum((int)c) != 0 || c == '"' || c == CHARSEOF0)) {
				currentToken.code = ERR_T;
				currentToken.attribute.err_lex[0] = c;
				currentToken.attribute.err_lex[1] = CHARSEOF0;
			}
			else {
				//Retract to the beginning of the token
				bufferRetract(sourceBuffer);
				lexStart = bufferSetMarkPosition(sourceBuffer, bufferGetCPosition(sourceBuffer));

				//Initial FSM state type is NOAS
				int currentStateType = NOAS;
				state = getNextState(state, bufferGetChar(sourceBuffer));

				//While the current state type is 'Not an Accepting State' get the next state until it is an accepting state.
				//BUG: State 11 for first keyword PROGRAM - State = 1 currentstatetype = 2
				while (currentStateType == NOAS) {
					c = bufferGetChar(sourceBuffer);
					state = getNextState(state, c);
					currentStateType = stateType[state];
				}
				//Once reaching this point, the state is either ASWR/ASNR

				//If the accepting state is ASWR, need to retract the buffer
				if (currentStateType == ASWR) {
					bufferRetract(sourceBuffer);
				}

				//Create a temporary lexeme buffer in fixed operation mode
				//Handle buffer creation failure
				lexEnd = bufferGetCPosition(sourceBuffer);
				lexLength = lexEnd - lexStart;

				//Warning C4244 - reasoning: To conform with design specifications. Data loss is unlikely to happen and effet the buffer creation.
				lexemeBuffer = bufferCreate(lexLength, 0, 'f');
				if (!lexemeBuffer) {
					currentToken.code = RTE_T;
					errorNumber = 1;
					strcpy(currentToken.attribute.err_lex, "RUN TIME ERROR:");
					return currentToken;
				}

				//Set lexical analyzer getCPosition back to mark
				bufferReset(sourceBuffer);

				//Copy lexeme into the temporary lexeme
				for (i = lexStart; i < lexEnd; i++) {
					c = bufferGetChar(sourceBuffer);
					bufferAddChar(lexemeBuffer, c);
				}
				bufferSetEnd(lexemeBuffer, CHARSEOF0);

				//Set the token by using the finalStateTable
				//The state represents the entry in the finalStateTable with the lexeme argument being the lexemebuffer
				currentToken = finalStateTable[state](bufferGetString(lexemeBuffer, 0));
				free(lexemeBuffer);

				return currentToken;
			}

			return currentToken;
		} // switch

	} //while
	return currentToken;
} //processToken


/* DO NOT MODIFY THE CODE OF THIS FUNCTION
YOU CAN REMOVE THE COMMENTS ONLY */
/*************************************************************
 * Get Next State
 ************************************************************/

int getNextState(int state, char c) {
	int col;
	int next;
	if (debugMode)
		fprintf(stderr, "[03] getNextState\n");
	col = nextTokenClass(c);
	next = transitionTable[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*
* Purpose: Gets the next token class
* Author: Ryan Arreola
* Version: 1.0
* Called functions: fprintf(), isalpha(), isdigit()
* Parameters: char c - represents the symbol to be matched in the transition table
* Return value: int - The value to be used when referencing the columns in the transition table.
*/
int nextTokenClass(char c) {
	int val = -1;
	if (debugMode)
		fprintf(stderr, "[04] NextTokenClass\n");

	/*
	THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS [A-Za-z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A,B,...,Z,a,b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)
	*/

	//L [A-z](0)
	if (isalpha(c)) {
		val = 0;
	}
	//Z 0(1)
	else if (c == '0') {
		val = 1;
	}
	//N [1-9](2)
	else if (isdigit(c) && c != '0') {
		val = 2;
	}
	//P .(3)
	else if (c == '.') {
		val = 3;
	}
	//$(4)
	else if (c == '$') {
		val = 4;
	}
	//"(5)
	else if (c == '"') {
		val = 5;
	}
	//SEOF(6)
	else if (c == CHARSEOF255 || c == CHARSEOF0) {
		val = 6;
	}
	//Other (7)
	else {
		val = 7;
	}

	return val;
}


/*
* Acceptance State Function AVID/KW
* Purpose: Function for reconizing arithmetic variable identifiers and keywords.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: isKeyword(), strlen()
* Parameters: char * lexeme - a pointer to a lexeme
* Return value: Token - Returns eithe ra keywort token or an AVID token.
* Algorithm:Chekc sif the given lexeme is a keyword, if it is then set the current token to a keyword if not set it to an AVID.
*/
Token aStateFuncAVID(char* lexeme) {
	Token currentToken = { 0 };
	int i;
	int keyWordNum = isKeyword(lexeme);

	if (keyWordNum != RT_FAIL_1) {
		currentToken.code = KW_T;
		currentToken.attribute.kwt_idx = keyWordNum;
		return currentToken;
	}
	//At this point token is not a keyword

	//If lexeme is longer than the variable id length, only the first VID_LEN characters are stored in the attributes vid_lex[]. SEOF is added to the end of the variable id signifying the end of the variable.
	currentToken.code = AVID_T;
	if (strlen(lexeme) > VID_LEN) {
		for (i = 0; i < VID_LEN; i++) {
			currentToken.attribute.vid_lex[i] = lexeme[i];
		}
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0;
	}
	else {
		strcpy(currentToken.attribute.vid_lex, lexeme);
	}

	return currentToken;
}


/*
* Acceptance State Function SVID
* Purpose: Function for reconizing string variable identifiers.
* Author: Ryan Arreola
* Version: 1.0
* Called functions:
* Parameters: void
* Return value:
* Algorithm:
*/
Token aStateFuncSVID(char* lexeme) {
	Token currentToken = { 0 };
	int i;
	currentToken.code = SVID_T;
	int lexLength = strlen(lexeme);

	//If the lexeme is longer than the allowed variable id length, add the lexeme characters equal to the vid length -1 with the second to last index holding the $ string identifier and the last holding the SEOF value.
	if (lexLength > VID_LEN) {
		for (i = 0; i < VID_LEN - 1; i++) {
			currentToken.attribute.vid_lex[i] = lexeme[i];
		}
		currentToken.attribute.vid_lex[VID_LEN - 1] = '$';
		currentToken.attribute.vid_lex[VID_LEN] = CHARSEOF0;
	}
	else {
		//Store the entire lexeme into the vid_lex
		strncpy(currentToken.attribute.vid_lex, lexeme, lexLength);
		currentToken.attribute.vid_lex[strlen(lexeme)] = CHARSEOF0;
	}
	return currentToken;
}

/*
* Acceptance State Function IL
* Purpose: Function for reconizing string variable identifiers.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: atol()
* Parameters: char * lexeme -  Pointer to a lexeme
* Return value: Token - Returns either an error or IL token.
*/
Token aStateFuncIL(char* lexeme) {
	Token currentToken = { 0 };
	long temp = atol(lexeme);

	if (temp > SHRT_MAX || temp < SHRT_MIN) {
		currentToken = finalStateTable[ES](lexeme);
	}
	else {
		currentToken.code = INL_T;
		currentToken.attribute.int_value = temp;
	}
	
	return currentToken;
}

/*
* Acceptance State Function FPL
* Purpose: Function for reconizing string variable identifiers.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: strlen()
* Parameters: char* lexeme - pointer to a lexme
* Return value: Token - Error token or an FLP token
*/
Token aStateFuncFPL(char* lexeme) {
	Token currentToken = { 0 };
	float temp = strtof(lexeme, NULL);

	//If the lexeme is out of range
	if (((temp >= 0 && strlen(lexeme) > 7) && (temp < FLT_MIN || temp > FLT_MAX)) || (temp < 0)) {
		currentToken = finalStateTable[ES](lexeme);
	}
	else {
		currentToken.code = FPL_T;
		currentToken.attribute.flt_value = temp;
	}

	return currentToken;
}

/*
* Acceptance State Function SL
* Purpose: Function for reconizing string literals.
* Author: Ryan Arreola
* Version: 1.0
* Called functions:
* Parameters: char* lexeme
* Return value: Token - String literal token
*/
Token aStateFuncSL(char* lexeme) {
	Token currentToken = { 0 };
	int i;
	//String offset to where the first char of the lexeme will be added to
	currentToken.attribute.str_offset = bufferGetAddCPosition(stringLiteralTable);

	//Go through the lexeme, if the character is not an end quote then add to the string literal table. If the symbol is a newline then increase the global line
	for (i = 0; i < (int)strlen(lexeme); i++) {
		if (lexeme[i] != '"') {
			bufferAddChar(stringLiteralTable, lexeme[i]);
		}
		if (lexeme[i] == '\n') {
			line++;
		}
	}
	//Add the SEOF to the end of the buffer to make the string c-type.
	bufferAddChar(stringLiteralTable, CHARSEOF0);
	currentToken.code = STR_T;

	return currentToken;
}


/*
* Acceptance State Function Error
* Purpose: Set the error token when DFA is not finishing with correct error token.
* Author: Ryan Arreola
* Version: 1.0
* Called functions: strlen(), strncpy()
* Parameters: char *lexeme - pointer to a lexeme
* Return value: Token - Error token
*/
Token aStateFuncErr(char* lexeme) {
	Token currentToken = { 0 };
	int i;
	int lexLength = strlen(lexeme);
	currentToken.code = ERR_T;

	//If the length of the error_lex. Copy the maximum allowed length of characters into the token error lexeme with the last 3 charcters being periods.
	if (lexLength > ERR_LEN) {
		strncpy(currentToken.attribute.err_lex, lexeme, ERR_LEN - 3);
		currentToken.attribute.err_lex[ERR_LEN - 3] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 2] = '.';
		currentToken.attribute.err_lex[ERR_LEN - 1] = '.';

		currentToken.attribute.err_lex[ERR_LEN] = CHARSEOF0;
	}
	else {
		//If the error can fit, copy directly and set tge SEOF
		strncpy(currentToken.attribute.err_lex, lexeme, lexLength);
		currentToken.attribute.err_lex[lexLength] = CHARSEOF0;
	}
	//If the line contains line terminatiors, increment the line variable
	for (i = 0; i < lexLength; i++) {
		if (lexeme[i] == '\n') {
			line++;
		}
	}

	return currentToken;
}

/*
* Purpose: Acceptance State Function Error - with retract for the error token
* Author: Ryan Arreola
* Version: 1.0
* Parameters: char *lexeme
* Return value: Token - Returns an error token
*/
Token aStateFuncErrorRet(char* lexeme) {
	Token currentToken = { 0 }; /* Token to be returned */
	currentToken = finalStateTable[ES](lexeme);
	return currentToken;
}

/*
* Purpose: Checks if a string is a keyword, returning the position in the list
* Author: Ryan Arreola
* Version: 1.0
* Called functions: strcmp()
* Parameters: char* kw_lexeme
* Return value: intefer of keyword index or return failure code
*/
int isKeyword(char* kw_lexeme) {
	int i;
	for (i = 0; i < KWT_SIZE; i++) {
		if (strcmp(keywordTable[i], kw_lexeme) == 0) {
			return i;
		}
	}
	return RT_FAIL_1;
}

/*
HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS (IF ANY).
FOR EXAMPLE
*/
