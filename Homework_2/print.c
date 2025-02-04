#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

const int BOX_WIDTH = 55;

/* Prints the output of the program with a nice format */
void printResults(double avgExecutionTime, int numWorkers, int totalWords, int palindromeCount, int semordnilapCount, int (*sums)[2]) {
    // Print the header
    for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n| %-*s %-*s |\n", 40, "Word Analysis Results", BOX_WIDTH - 40 - 5, "");
    for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");

    // Total words
    printf("| %-40s %10d |\n", "Total number of words:", totalWords);
    for (int i = 0; i < BOX_WIDTH; i++) printf("-");
    printf("\n");

    // Palindrome count
    printf("| %-40s %10d |\n", "Palindrome(s) found:", palindromeCount);
    for (int i = 0; i < BOX_WIDTH; i++) printf("-");
    printf("\n");

    // Semordnilap count
    printf("| %-40s %10d |\n", "Semordnilap(s) found:", semordnilapCount);
    for (int i = 0; i < BOX_WIDTH; i++) printf("-");
    printf("\n");

    // Execution time 
    printf("| %-36s %.8f sec |\n", "Program AVG execution time:", avgExecutionTime);
    for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");

    // Worker results
    for (int i = 0; i < numWorkers; i++) {
        printf("| Worker %-2d Found: %3d Palindromes & %3d Semordnilaps |\n",
               i, sums[i][0], sums[i][1]);
        for (int i = 0; i < BOX_WIDTH; i++) printf("-");
        printf("\n");
    }

    // Footer
    for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");
}

/* Function to write results to a file */
void writeResultsToFile(const char *filename, int palindromeCount, int semordnilapCount, char **resultPalindromes, char **resultSemordnilaps) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Write Palindromes
    fprintf(file, "==================== Palindromes ====================\n");
    for (int i = 0; i < palindromeCount; i++) {
        fprintf(file, "%s\n", resultPalindromes[i]);
    }

    fprintf(file, "\n==================== Semordnilaps ====================\n");
    for (int i = 0; i < semordnilapCount; i++) {
        fprintf(file, "%s\n", resultSemordnilaps[i]);
    }
  
  fclose(file);
}

void printSpeedUp(double (*programTime)[2], int argc) {
    printf("Index\tExecution Time\tThreads\n");
    for (int i = 0; i < argc; i++) {
        printf("%d\t%.8f\t\t%.0f\t%.2f\n", i + 1, programTime[i][0], programTime[i][1], programTime[0][0] / programTime[i][0]);
    }
}