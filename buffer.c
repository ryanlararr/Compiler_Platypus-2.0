
#include "buffer.h"

/*
File name: buffer.c
Compiler: MS Visual Studio 2019
Author: Ryan Arreola, 040950298
Course: CST8152-Compilers, Lab Section 020
Assignment: 1
Date: 10/4/2020
Professor: Paulo Sousa/Haider Miraj
Purpose: Demonstrate knowledge of buffers for a compiler
Function list: bufferCreate(), bufferAddChar(), bufferClear(), bufferFree(), bufferIsFull(), bufferGetAddCPosition(), bufferGetCapacity(), bufferGetOpMode(), bufferGetIncrement(), bufferLoad(), bufferIsEmpty(), bufferGetChar(), bufferPrint(), bufferSetEnd(), bufferRetract(), bufferReset(), bufferRewind(), bufferGetString(), bufferSetMarkPosition(), bufferGetFlags()
*/

/*
Function Name: bufferCreate
Purpose: To create a new buffer with an initial capacity, increment, and operational mode
Author: Ryan Arreola
Versions: 1
Called Functions: calloc(), malloc(), free()
Parameters:
			short initCapacity: Sets the initial capacity of the buffer string
			char increment: The increment factor of the buffer during reallocation
			char opMode: Sets the operational mode of the buffer
Return value: A pointer to a buffer entity
Algorithm: Allocate dynamic memory for buffer and the symbol, manipulates increment based on opmode - or free up memory, set the flag and capacity, return pointer to the buffer.
*/
Buffer * bufferCreate(short initCapacity, char increment, char opMode) {
	//Allocation
	pBuffer buffer = calloc(1, sizeof(Buffer));

	//Validation
	if (buffer == NULL) {
		return NULL;
	}
	if (initCapacity < 0 || initCapacity  > MAX_PVAL || buffer == NULL) {
		free(buffer);
		return NULL;
	}
	buffer->string = malloc(sizeof(char) * initCapacity);
	if (buffer->string == NULL) {
		free(buffer);
		return NULL;
	}
	if (opMode == FIXED_MODE && initCapacity <= 0) {
		free(buffer->string);
		free(buffer);
		return NULL;
	}

	//Set the mode 
	if (opMode == FIXED_MODE || increment == FIXED) {
		buffer->opMode = FIXED;
		buffer->increment = increment;
	}
	else if (opMode == ADDITIVE_MODE && (unsigned char)increment > ADDITIVE_MIN && (unsigned char)increment <= ADDITIVE_MAX) {
		buffer->opMode = ADDITIVE;
		buffer->increment = increment;
	}
	else if (opMode == MULTIPLICATIVE_MODE && increment >= MULTIPLICATIVE_MIN && increment <= MULTIPLICATIVE_MAX) {
		buffer->opMode = MULTIPLICATIVE;
		buffer->increment = increment;
	}
	else {
		free(buffer);
		buffer = NULL;
		return NULL;
	}

	//return the buffer
	buffer->capacity = initCapacity;
	buffer->flags = DEFAULT_FLAGS;
	return buffer;
}

/*
Function Name: bufferAddChar
Purpose: Add a character symbol to the buffer
Author: Ryan Arreola
Versions: 1
Called Functions: realloc()
Parameters:
			const pBE: A pointer to a buffer entity
			char Symbol: A character that will be added to the buffer
Return value: A pointer to a buffer entity
Algorithm:
*/
pBuffer bufferAddChar(pBuffer const pBE, char symbol) {
	short updatedCapacity = 0;
	short updatedIncrement = 0;

	//Guard for a nonexistent buffer entity
	if (pBE == NULL) {
		return NULL;
	}

	pBE->flags &= RESET_R_FLAG;
	if (pBE->addCPosition < pBE->capacity){
		*(pBE->string + pBE->addCPosition++) = symbol;
		return pBE;
	}

	//resize buffer based on operational mode
	switch (pBE->opMode) {
	case FIXED:
		return NULL;
		break;
	case ADDITIVE: //Validate capacity and assign the updated capacity if applicable
		if (pBE->capacity >= MAX_PVAL) {
			return NULL;
		}
		updatedCapacity = pBE->capacity + (unsigned char)pBE->increment;
		if (updatedCapacity < 0) {
			return NULL;
		}
		else if (updatedCapacity > MAX_PVAL) {
			updatedCapacity = MAX_PVAL;
		}
		break;
	case MULTIPLICATIVE: //Validate capacity and assign updated capacity with the updated  increment if applicable
		if (pBE->capacity >= MAX_PVAL) {
			return NULL;
		}
		updatedIncrement = (MAX_PVAL - pBE->capacity) * (unsigned long)pBE->increment / 100;
		
		//In the case that the capacity and updated increment is above the maximum allowed positive value the capacity is updated and reduced to be the maximum
		if ((pBE->capacity + updatedIncrement < MAX_PVAL) && updatedIncrement != 0){
			updatedCapacity = pBE->capacity + updatedIncrement;
		}
		else if (pBE->capacity < MAX_PVAL){
			updatedCapacity = MAX_PVAL;
		}
		break;
	default: //Invalid operational mode
		return NULL;
		break;
	}

	//Update the given capacity with the above calculated updated capacity
	//Resize the given buffer and validate the memory address
	pBE->capacity = updatedCapacity;
	char* hold = NULL;
	hold = realloc(pBE->string, pBE->capacity);
	if (hold == NULL) {
		return NULL;
	}
	if (hold != (pBE->string)){
		pBE->flags |= SET_R_FLAG;
		pBE->string = hold;
	}
	//Check if there is space in the buffer for the symbol
	if (pBE->addCPosition < pBE->capacity) {
		*(pBE->string + pBE->addCPosition++) = symbol;
	}
	else {
		return NULL;
	}
	return pBE;
}

/*
Function Name: bufferClear
Purpose: Clears the buffer and markers and sets the flags to default
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: Pointer to a buffer entity
Return value: An integer representing the success/failure of the function
Algorithm: Bitwise AND on flags
*/
int bufferClear(Buffer* const pBE) {
	if (!pBE) return RT_FAIL_1;
	pBE->addCPosition = 0;
	pBE->getCPosition = 0;
	pBE->markCPosition = 0;
	pBE->flags &= DEFAULT_FLAGS; //bit operation to set default flags
	return 0;
}

/*
Function Name: bufferFree
Purpose: Frees the memory allocated for the buffer
Author: Ryan Arreola
Versions: 1
Called Functions: free()
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value:
Algorithm:
*/
void bufferFree(Buffer* const pBE) {
	//In the case that the buffer entity exists and the pointer to the character symbols of the buffer exists then free it
	if (pBE != NULL) {
		if (pBE->string) {
			free(pBE->string);
		}
		free(pBE);
	}
}


/*
Function Name: bufferIsFull
Purpose: 
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: An integer detailing the success/failure of the function
Algorithm:
*/
int bufferIsFull(Buffer* const pBE) {
	if (pBE != NULL) {
		if (pBE->addCPosition == pBE->capacity) {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferGetAddCPosition
Purpose: Get the current offset of the buffer entity
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: Short: Either the position or a failure
Algorithm:
*/
short bufferGetAddCPosition(Buffer* const pBE) {
	if (pBE != NULL) {
		return pBE->addCPosition;
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferGetCapacity
Purpose:
Author: Ryan Arreola
Versions: 1
Called Functions: Buffer* pBE: A pointer to a buffer entity
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: short: capacity of the buffer entity or a return failure
Algorithm:
*/
short bufferGetCapacity(Buffer* const pBE) {
	if (pBE != NULL) {
		return pBE->capacity;
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferGetOpMode
Purpose: 
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: int: the operational mode of the buffer or a failure
Algorithm:
*/
int bufferGetOpMode(Buffer* const pBE) {
	if (pBE != NULL) {
		return pBE->opMode;
	}
	else {
		return RT_FAIL_2;
	}
}

/*
Function Name: bufferGetIncrement
Purpose:
Author: Ryan Arreola
Versions: 1
Called Functions: 
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: size_t: an unsigned integer representing the size of an object in bytes
Algorithm:
*/
size_t bufferGetIncrement(Buffer* const pBE) {
	
	//size_t data type is an unsigned char, increment must be typecasted to be appropriate
	//return special 0x100 Hexadecimal runtime error in the case of a null buffer entity due to size_t being in bytes and hexadecimal
	if (pBE != NULL) {
		return (unsigned char)pBE->increment;
	}
	else {
		return 0x100;
	}
}

/*
Function Name: bufferLoad
Purpose: Loads the file into the buffer entity
Author: Ryan Arreola
Versions: 1
Called Functions: fgetC(), ungetc(), printf(), bufferAddChar()
Parameters: 
			FILE* fi: Platypus file in the file stream
			Buffer* pBE: A pointer to a buffer entity
Return value: int: An integer of the char position or a negative integer representing a load failure
Algorithm: Loop until end of file
*/
int bufferLoad(FILE* const fi, Buffer* const pBE) {
	if (pBE == NULL || fi == NULL) {
		return RT_FAIL_1;
	}

	//Loop through file and add the character symbol to the buffer if there is space and return to file stream
	char symbol = 0;
	while(1){
		symbol = (char)fgetc(fi);
		if (feof(fi)){
			break;
		}
		if (bufferAddChar(pBE, symbol) == NULL){
			ungetc(symbol, fi);
			return LOAD_FAIL;
		}
	}
	return pBE->addCPosition;
}

/*
Function Name: bufferIsEmpty
Purpose: Check if the buffer is empty
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: int: An integer representing the success/failure if the function
Algorithm:
*/
int bufferIsEmpty(Buffer* const pBE) {
	if (pBE != NULL) {
		if (pBE->addCPosition == 0) {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferGetChar
Purpose: Read the character in the buffer
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: char: A char representing the symbol in the offset at the buffer
Algorithm: Sets the flags appropriately based on the current position
*/
char bufferGetChar(Buffer* const pBE) {
	if (pBE == NULL) {
		return RT_FAIL_2;
	}

	if (pBE->getCPosition == pBE->addCPosition){
		pBE->flags |= SET_EOB;
		return 0;
	}
	pBE->flags &= RESET_EOB;

	return *(pBE->string + pBE->getCPosition++);
}

/*
Function Name: bufferPrint
Purpose: Prints the character symbols in the buffer
Author: Ryan Arreola
Versions: 1
Called Functions: bufferGetChar(), printf()
Parameters: 
			Buffer* pBE: A pointer to a buffer entity
			char newline: implemented in restBuffer.c as 
Return value: int: An integer representing the position/symbol/error
Algorithm: Check for a null buffer or an empty buffer, if pass: loop through all symbols and stops at EOB flag, then return the current position int the buffer
*/
int bufferPrint(Buffer* const pBE, char newLine) {
	//Check for a null buffer or an empty buffer
	if (pBE == NULL) {
		return RT_FAIL_1;
	}
	if (pBE->addCPosition == 0) {
		newLine;
		return 0;
	}

	//loop through buffer and print all symbols
	char symbol = 0;
	while (1) {
		symbol = bufferGetChar(pBE);
		if ((pBE->flags & CHECK_EOB) == 1) {
			break;
		}
		printf("%c", symbol);
	}
	printf("\n");
	return pBE->getCPosition;
}

/*
Function Name: bufferSetEnd
Purpose: Reallocate the buffer tehn add character symbol
Author: Ryan Arreola
Versions: 1
Called Functions: realloc()
Parameters: 
			Buffer* pBE: A pointer to a buffer entity
			char symbol: Add to the end of the character buffer
Return value: A pointer to a buffer entity
Algorithm: Check if buffer entity is null or if the position is at the maximum, then check if the reallocated beginnig of array is different from the current
*/
Buffer* bufferSetEnd(Buffer* const pBE, char symbol) {
	
	//check if buffer entity is null or at the maximum position
	if (pBE == NULL) {
		return NULL;
	}
	pBE->flags &= RESET_R_FLAG;
	if (pBE->addCPosition == SHRT_MAX) {
		return NULL;
	}

	//check if the reallocated beginning of the array is different from the current
	char* begin;
	begin = realloc(pBE->string, pBE->addCPosition + 1);
	if (begin == NULL) {
		return NULL;
	}
	if (begin != (pBE->string)) {
		pBE->flags |= SET_R_FLAG;
		pBE->string = begin;
	}
	*(pBE->string + pBE->addCPosition++) = symbol;
	pBE->capacity = pBE->addCPosition;
	return pBE;
}

/*
Function Name: bufferRetract
Purpose: Decrement buffer offset
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: short representing buffer position or failure
Algorithm:
*/
short bufferRetract(Buffer* const pBE) {
	if (pBE != NULL) {
		if (pBE->getCPosition > 0) {
			return --(pBE->getCPosition);
		}
		else {
			return RT_FAIL_1;
		}
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferReset
Purpose: sets character position to the marked character positon
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: short failure or character position
Algorithm:
*/
short bufferReset(Buffer* const pBE) {
	if (pBE != NULL){
		pBE->getCPosition = pBE->markCPosition;
		return pBE->getCPosition;
	}
	else {
		return RT_FAIL_1;
	}
}

/*
Function Name: bufferRewind
Purpose: set  character position and mark c position to 0
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: Buffer* pBE: A pointer to a buffer entity
Return value: int: character postion or failure
Algorithm:
*/
int bufferRewind(Buffer* const pBE) {
	if (pBE != NULL) {
		pBE->markCPosition = 0;
		pBE->getCPosition = 0;
		return pBE->getCPosition;
	}
	else {
		return RT_FAIL_1;
	}
}


/*
Function Name: bufferGetString
Purpose: Return pointer of character in the buffer entity
Author: Ryan Arreola
Versions: 1
Called Functions:
Parameters: 
			Buffer* pBE: A pointer to a buffer entity
			short charPosition: Position of character in buffer
Return value: pointer character or null
Algorithm:
*/
char* bufferGetString(Buffer* const pBE, short charPosition) {
	if (pBE != NULL) {
		if (charPosition >= 0 && charPosition < pBE->addCPosition) {
			return (pBE->string + charPosition);
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

/*
Function name: bufferSetMarkPosition
Purpose: Sets the offset in chars to the mark location
Parameters:
	pBuffer pBE: Buffer entity
	short mark: What to set the markCPosition as
*/
short bufferSetMarkPosition(pBuffer const pBE, short mark) {
	if (!pBE) {
		return RT_FAIL_1;
	}

	if (mark < 0 || mark > pBE->addCPosition) {
		return RT_FAIL_1;
	}

	pBE->markCPosition = mark;
	return pBE->markCPosition;
}

/*
Function name: bufferGetCPosition
Purpose: Returns getCPosition to the calling function
Parameters:
	pBuffer pBE: Buffer entity
*/
short bufferGetCPosition(Buffer* const pBE) {
	if (!pBE) {
		return RT_FAIL_1;
	}
	return pBE->getCPosition;
}

/*
Function Name: bufferGetFlags
Purpose: Return chracter reallocation flags of the buffer
Parameters: pBuffer pBE: A buffer entity
*/
unsigned short bufferGetFlags(pBuffer const pBE) {
	if (!pBE) {
		return RT_FAIL_1;
	}
	return pBE->flags;
}