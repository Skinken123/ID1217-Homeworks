#ifndef PRINT_H
#define PRINT_H

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void printResults(double avgExecutionTime, int numWorkers, int totalWords, int palindromeCount, int semordnilapCount, int (*sums)[2]);
void writeResultsToFile(int palindromeCount, int semordnilapCount, char **resultPalindromes, char **resultSemordnilaps, int wordIndex);
void printSpeedUp(double (*programTime)[2], int argc, int totWords);

#endif /* PRINT_H */