#ifndef PRINT_H
#define PRINT_H

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void printResults(double start_time, double end_time, int numWorkers, int totalWords, int palindromeCount, int semordnilapCount, int (*sums)[2]);
void writeResultsToFile(const char *filename, int palindromeCount, int semordnilapCount, char **resultPalindromes, char **resultSemordnilaps);

#endif /* PRINT_H */