/****************************************************
 * Filename: buffer.h
 * Purpose: Preprocessor directives, type declarations and prototypes necessary for buffer implementation
 * Compiler: MS Visual Studio 2019
 * Author: Ryan Arreola - 040950298
 * Course: CST8152 - Compilers
 * Assignment: 2
 * Date: November 13, 2020
 * Professor: Paulo Sousa
 * Purpose: Declaring the data and methods to be used in the scanning process.
 * Function list: aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL(), aStateFuncFPL(), aStateFuncSL(), aStateFuncErr(), aStateFuncErrorRet()
 ***************************************************
 */

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type co mments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value 1 */
#define RT_FAIL_2 (-2)         /* operation failure return value 2 */
#define LOAD_FAIL (-2)         /* load fail return value */

#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */


/* You should add your own constant definitions here */
#define FIXED 0 //fixed operational mode
#define ADDITIVE 1 //additive operational mode
#define MULTIPLICATIVE -1 //multipiocative operational mode
#define FIXED_MODE 'f' //character symbol for fixed mode
#define ADDITIVE_MODE 'a' //character symbol for additive mode
#define ADDITIVE_MIN 0 //Minimum increment factor for additive mode
#define ADDITIVE_MAX 255 //Maximum increment factor for additive mode
#define MULTIPLICATIVE_MODE 'm' //character symbol for multiplicative mode
#define MULTIPLICATIVE_MAX 100 //Maximum increment factor for multiplicative mode
#define MULTIPLICATIVE_MIN 1 //Minimum increment factor for multiplicative mode
#define MAX_PVAL (SHRT_MAX-1) //max positive value for buffer

/* Add your bit-masks constant definitions here */
#define DEFAULT_FLAGS  0xFFFC 	// 1111.1111 1111.1100
#define SET_EOB        0x0001	//0001
#define RESET_EOB      0xFFFE	//1111.1111. 1111.1110
#define CHECK_EOB      0x0001	//0001
#define SET_R_FLAG     0x0002	//0010
#define RESET_R_FLAG   0xFFFD	//1111.1111 1111.1101
#define CHECK_R_FLAG   0x0002	//0010

/* user data type declarations */
typedef struct BufferEntity {
	char* string;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addCPosition;  /* the offset (in chars) to the add-character location */
	short getCPosition;  /* the offset (in chars) to the get-character location */
	short markCPosition; /* the offset (in chars) to the mark location */
	char  increment; /* character array increment factor */
	char  opMode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;

/*
IMPORTANT NOTE (2DO):
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
Buffer * bufferCreate(short initCapacity, char incFator, char opMode);
pBuffer bufferAddChar(pBuffer const pBE, char symbol);
int bufferClear(Buffer* const pBE);
void bufferFree(Buffer* const pBE);
int bufferIsFull(Buffer* const pBE);
short bufferGetAddCPosition(Buffer* const pBE);
short bufferGetCPosition(Buffer* const pBE);
short bufferGetCapacity(Buffer* const pBE);
int bufferGetOpMode(Buffer* constpBE);
size_t bufferGetIncrement(Buffer* const pBE);
int bufferLoad(FILE* const fi, Buffer* const pBE);
int bufferIsEmpty(Buffer* const pBE);
char bufferGetChar(Buffer* const pBE);
int bufferPrint(Buffer* const pBE, char newLine);
Buffer* bufferSetEnd(Buffer* const pBE, char symbol);
short bufferRetract(Buffer* const pBE);
short bufferReset(Buffer* const pBE);
int bufferRewind(Buffer* const pBE);
char* bufferGetString(Buffer* const pBE, short charPosition);
short bufferSetMarkPosition(pBuffer const pBE, short mark);
unsigned short bufferGetFlags(pBuffer const pBE);

#endif
