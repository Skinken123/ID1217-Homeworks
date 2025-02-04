/* Finding Palindromes & Semordnilaps

  This document was originaly written by Alexander Nordgren, Gustav Moritz 

  Description:

  The program when excuted finds all Palindromes & Semordnilaps of a text file,
  and then stores all of the words/phrases inside a new text file. The program also
  shows how many words where found by each worker/thread, the total amount of words/phrases found, 
  and the excecution time of the program. 

*/
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#define TOTAL_WORDS 25143  /* maximum dictionary words from file*/
#define MAX_WORD_LENGTH 22  /* max byte length of words in file */
#define MAXWORKERS 16   /* maximum number of workers */

int numWorkers = 0;           /* number of workers */ 
int palindromeCount = 0;
int semordnilapCount = 0;

double start_time, end_time; /* start and end times of parallel execution*/

int sums[MAXWORKERS][2]; /* partial sums, how many palindromes/semordnilaps each worker/thread has found*/
char **resultPalindromes = NULL; // Dynamic array for palindromes
char **resultSemordnilaps = NULL; // Dynamic array for Semordnilaps
char **words = NULL; // Array of words

int main(int argc, char *argv[]) {
  long l; /* use long in case of a 64-bit system */

  /* read command line args if any */
  numWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  /* parse dictionary file into global variable array*/
  if (readWordsFromFile("words", &words, TOTAL_WORDS)) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
  }

}
//End of document