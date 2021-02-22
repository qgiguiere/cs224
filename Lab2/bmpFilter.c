#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE  1
#define FALSE 0
#define BAD_NUMBER_ARGS 1
#define FSEEK_ERROR 2
#define FREAD_ERROR 3
#define MALLOC_ERROR 4
#define FWRITE_ERROR 5

/**
 * Parses the command line.
 *
 * argc:      the number of items on the command line (and length of the
 *            argv array) including the executable
 * argv:      the array of arguments as strings (char* array)
 * grayscale: the integer value is set to TRUE if grayscale output indicated
 *            outherwise FALSE for threshold output
 *
 * returns the input file pointer (FILE*)
 **/
FILE *parseCommandLine(int argc, char **argv, int *isGrayscale) {
  if (argc > 2) {
    printf("Usage: %s [-g]\n", argv[0]);
    exit(BAD_NUMBER_ARGS);
  }
  if (argc == 2 && strcmp(argv[1], "-g") == 0) {
    *isGrayscale = TRUE;
  } 
  else {
    *isGrayscale = FALSE;
  }

  return stdin;
}

unsigned getFileSizeInBytes(FILE* stream) {
  unsigned fileSizeInBytes = 0;
  rewind(stream);
  if (fseek(stream, 0L, SEEK_END) != 0) {
    exit(FSEEK_ERROR);
  }
  fileSizeInBytes = ftell(stream);

  return fileSizeInBytes;
}

void getBmpFileAsBytes(unsigned char* ptr, unsigned fileSizeInBytes, FILE* stream) {
  rewind(stream);
  if (fread(ptr, fileSizeInBytes, 1, stream) != 1) {
#ifdef DEBUG
  printf("feof() = %x\n", feof(stream));
  printf("ferror() = %x\n", ferror(stream));
#endif
  exit(FREAD_ERROR);
  }
}

unsigned char getAverageIntensity(unsigned char blue, unsigned char green, unsigned char red) {
  unsigned char charAvg = (blue + green + red) / 3;

  return charAvg;
}

void applyGrayscaleToPixel(unsigned char* pixel) {
  unsigned char* blueptr = pixel;
  unsigned char* greenptr = pixel + 1;
  unsigned char* redptr = pixel + 2;
  unsigned char blueValue = *blueptr;
  unsigned char greenValue = *greenptr;
  unsigned char redValue = *redptr;
  unsigned char avgIntensity = getAverageIntensity(blueValue, greenValue, redValue);

  *blueptr = avgIntensity;
  *greenptr = avgIntensity;
  *redptr = avgIntensity;
}

void applyThresholdToPixel(unsigned char* pixel) {
  unsigned char* blueptr = pixel;
  unsigned char* greenptr = pixel + 1;
  unsigned char* redptr = pixel + 2;
  unsigned char blueValue = *blueptr;
  unsigned char greenValue = *greenptr;
  unsigned char redValue = *redptr;
  unsigned char avgIntensity = getAverageIntensity(blueValue, greenValue, redValue);

  if (avgIntensity >= 128) {
    *blueptr = 0xff;
    *greenptr = 0xff;
    *redptr = 0xff;
  }
  else {
    *blueptr = 0x00;
    *greenptr = 0x00;
    *redptr = 0x00;  
  }
}

void applyFilterToPixel(unsigned char* pixel, int isGrayscale) {
  if (isGrayscale) {
    applyGrayscaleToPixel(pixel);
  }
  else {
    applyThresholdToPixel(pixel);
  }
}

void applyFilterToRow(unsigned char* row, int width, int isGrayscale) {
  int pixelSize = 3;
  unsigned char* currentPixel = row;

  for (size_t i = 0; i < width; i++) {
    applyFilterToPixel(currentPixel, isGrayscale);
    currentPixel += pixelSize;
  }
}

void applyFilterToPixelArray(unsigned char* pixelArray, int width, int height, int isGrayscale) {
  int pixelSize = 3;
  int widthInBytes = width * pixelSize;
  int rowSizeInBytes = widthInBytes;
  int padding = 0;
  unsigned char* currentRow = pixelArray;

  if (widthInBytes % 4 != 0) {
    padding = 4 - (widthInBytes % 4);
  }
  rowSizeInBytes = widthInBytes + padding;
  for (size_t i = 0; i < height; i++) {
    applyFilterToRow(currentRow, width, isGrayscale);
    currentRow += rowSizeInBytes;
  }
}

void parseHeaderAndApplyFilter(unsigned char* bmpFileAsBytes, int isGrayscale) {
  int offsetFirstBytePixelArray = 0;
  int width = 0;
  int height = 0;
  unsigned char* pixelArray = NULL;

  offsetFirstBytePixelArray =  *(int *)(bmpFileAsBytes + 10);
  width = *(int *)(bmpFileAsBytes + 14 + 4);
  height = *(int *)(bmpFileAsBytes + 14 + 8);
  pixelArray = bmpFileAsBytes + offsetFirstBytePixelArray;
  applyFilterToPixelArray(pixelArray, width, height, isGrayscale);
}

int main(int argc, char **argv) {
  int grayscale = FALSE;
  unsigned fileSizeInBytes = 0;
  unsigned char* bmpFileAsBytes = NULL;
  FILE *stream = NULL;
  
  stream = parseCommandLine(argc, argv, &grayscale);
  fileSizeInBytes = getFileSizeInBytes(stream);

#ifdef DEBUG
  printf("fileSizeInBytes = %u\n", fileSizeInBytes);
#endif

  bmpFileAsBytes = (unsigned char *)malloc(fileSizeInBytes);
  if (bmpFileAsBytes == NULL) {
    exit(MALLOC_ERROR);
  }
  getBmpFileAsBytes(bmpFileAsBytes, fileSizeInBytes, stream);
  parseHeaderAndApplyFilter(bmpFileAsBytes, grayscale);

#ifndef DEBUG
  if (fwrite(bmpFileAsBytes, fileSizeInBytes, 1, stdout) != 1) {
    exit(FWRITE_ERROR);
  }
#endif

  free(bmpFileAsBytes);
  
  return 0;
}