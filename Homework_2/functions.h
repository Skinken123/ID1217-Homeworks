#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int readWordsFromFile(const char *filename, char ***words, int maxWords, int maxWordLength);
bool isPalindrome(const char *str);
void reverseString(char *str);
bool binarySearch(char **words, const char *target, int totalWords);

#endif /* FUNCTIONS_H */




