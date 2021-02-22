#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define BAD_NUMBER_ARGS 1

/**
 * Parses the command line.
 *
 * argc: the number of items on the command line (and length of the
 *       argv array) including the executable
 * argv: the array of arguments as strings (char* array)
 * bits: the integer value is set to TRUE if bits output indicated
 *       outherwise FALSE for hex output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *bits) {
  if (argc > 2) {
    printf("Usage: %s [-b|-bits]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }

  if (argc == 2 &&
      (strcmp(argv[1], "-b") == 0 || strcmp(argv[1], "-bits") == 0)) {
    *bits = TRUE;
  } else {
    *bits = FALSE;
  }

  return stdin;
}

/**
 * Writes data to stdout in hexadecimal.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsHex(unsigned char *data, size_t size) {

  int totalCharactersNeeded = 40;

  int hexesSinceLastSpace = 0; 
  for (int i = 0; i < size; i++) {
        if((data[i] < 32) || data[i] > 126) {
		// Accounting for the chars that aren't typical alphabet chars on the ASCII table
		if (data[i] < 16) { // 1t6 Because from 16 and on the hex outputs are all two chars 
				    // instead of one.
			printf("%i", 0); // Print a leading zero because there will be only one char
			printf("%x", data[i]);
		} else {
			printf("%x", data[i]); // We can print the char normally because it will have
					       // two chars	
		}

	} 
	else {
	       	printf("%x", data[i]);		
	}
	totalCharactersNeeded -= 2; // Subtract two because each hex is two chars big
	hexesSinceLastSpace++; 
	if((hexesSinceLastSpace == 2) && (totalCharactersNeeded != 1)) {
		printf(" ");
		totalCharactersNeeded--; // By only one because a whitespace is only one char
		hexesSinceLastSpace = 0; 
 	}
  }
  for(; 1 < totalCharactersNeeded; totalCharactersNeeded--) { // We compare to one here because 
	  						      // character count isn't zero index 
							      // based.
  	printf(" ");
  }
}

/**
 * Writes data to stdout as characters.
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 16 characters
 * size: the size of the array
 **/
void printDataAsChars(unsigned char *data, size_t size) {
	for(int i = 0; i < size; i++) {
		if((data[i] < 32) || (data[i] > 126)) {
			printf(".");	
		} else {
			printf("%c", data[i]);
		}	
	}
}


/**
 * Writes data to stdout in binary
 *
 * See myxxd.md for details.
 *
 * data: an array of no more than 6 bytes
 * size: number of characters in the array
 **/

void printDataAsBinary(unsigned char *data, size_t size) {

	int bytesLacking = (6 - size);

	for(int i = 0; i < size; i++) {
		int charAsIntValue = data[i];
		int byte[8] = { 0 };
	
	
		int storedBitNums = 0;
		int tempCharAsIntValue = data[i];
		while(tempCharAsIntValue >= 2) { // 2 Because binary is base 2
			tempCharAsIntValue /= 2;
			storedBitNums++;
			if (tempCharAsIntValue == 1)	 {
				storedBitNums++; // We do this because there is technically
						 // one more round to do, but our comparison
						 // algorithm is stinky so it won't really
						 // let us do it. This is compensation.
			}
		}


		// We now know how many non zero spaces we need to fill for our array
		
		// Time to find what the bits are
		
		int emptyBitNums = 8 - storedBitNums;

		// We are just filling the array with empty bit nums for now
		for (int j = 0; j < emptyBitNums; j++) {
			byte[j] = 0;
		}


		// Start filling byte[] from right to leftwith our calculations.
		// We have filled leading zeros with the size of emptyBitNums.
		// We will fill with the index after that.
		
		for (int j = 7; j > (emptyBitNums - 1); j--) {
			byte[j] = charAsIntValue % 2;
			charAsIntValue /= 2;
		}

		// The array is now filled correctly. Now we just need to print bytes[]
		// from left to right in order.

		for (int j = 0; j < 8; j++) {
			printf("%i", byte[j]);		
		}

		// The entire byte has been printed. Now print a whitespace to match format
		// between bytes. We have to have this special work around my for loop
		// beneath this one for some reason doesn't work with my overall logic. Lol.
		
		if((bytesLacking > 0) && (i != (size - 1))) {
			printf(" ");	
		} else if ((bytesLacking == 0) && (i != (size - 1))){
			printf(" ");
		}
	}

	// Now we will handle the whitespace. Each byte is 8 bits plus one space. 
	// So that is 9 characters total. We will printf(" "); 8 times for each 
	// byte that we are short of 6
	
	for(int i = 0; i < bytesLacking; i++) {
		for(int j = 0; j < 9; j++) {
			printf(" ");
		}	
	}
}

void readAndPrintInputAsHex(FILE *input) {
  unsigned char data[16];
  int numBytesRead = fread(data, 1, 16, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printf(" ");
    printDataAsHex(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 16, input);
  }
}

/**
 * Bits output for xxd.
 *
 * See myxxd.md for details.
 *
 * input: input stream
 **/
void readAndPrintInputAsBits(FILE *input) {
  unsigned char data[12];
  int numBytesRead = fread(data, 1, 6, input);
  unsigned int offset = 0;
  while (numBytesRead != 0) {
    printf("%08x:", offset);
    offset += numBytesRead;
    printf(" ");
    printDataAsBinary(data, numBytesRead);
    printf("  ");
    printDataAsChars(data, numBytesRead);
    printf("\n");
    numBytesRead = fread(data, 1, 6, input);
  }
}

int main(int argc, char **argv) {
  int bits = FALSE;
  FILE *input = parseCommandLine(argc, argv, &bits);

  if (bits == FALSE) {
    readAndPrintInputAsHex(input);
  } else {
    readAndPrintInputAsBits(input);
  }
  return 0;
}