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
  if (readWordsFromFile("words", &words, TOTAL_WORDS, MAX_WORD_LENGTH)) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
  }

  #pragma omp parallel num_threads(numWorkers) 
  {
    char **localPalindromes = NULL, **localSemordnilaps = NULL;
    int palIndex = 0, semIndex = 0;
    char reversed[MAX_WORD_LENGTH];
    int threadId = 0;
    sums[threadId][0] = 0;
    sums[threadId][1] = 0;

    #pragma omp for schedule(static)
      for (int i = 0; i < TOTAL_WORDS; i++) {
        if (isPalindrome(words[i])) {
          localPalindromes = realloc(localPalindromes, (palIndex + 1) * sizeof(char *));
          localPalindromes[palIndex] = strdup(words[i]);
          palIndex++;
          sums[threadId][0]++;
        } else {
            strcpy(reversed, words[i]);
            reverseString(reversed);
            const char *reversedWord = reversed;
            if(binarySearch(words, reversedWord, TOTAL_WORDS)) {
              localSemordnilaps = realloc(localSemordnilaps, (semIndex + 1) * sizeof(char *));
              localSemordnilaps[semIndex] = strdup(words[i]);
              semIndex++;
              sums[threadId][1]++;
            }
        }
      }

    #pragma omp critical 
    {
      //Palindromes
      int transferIndex = palindromeCount;
      palindromeCount = palindromeCount + palIndex;
      resultPalindromes = realloc(resultPalindromes, (palindromeCount + 1) * sizeof(char *));
      for (int i = 0; i < palIndex; i++){
        resultPalindromes[transferIndex] = strdup(localPalindromes[i]);
        transferIndex++;
      }
      //Semordnilaps
      transferIndex = semordnilapCount;
      semordnilapCount = semordnilapCount + semIndex;
      resultSemordnilaps = realloc(resultSemordnilaps, (semordnilapCount + 1) * sizeof(char *));
      for (int i = 0; i < semIndex; i++){
        resultSemordnilaps[transferIndex] = strdup(localSemordnilaps[i]);
        transferIndex++;
      }
    }
  }

  writeResultsToFile("results.txt", palindromeCount, semordnilapCount, resultPalindromes, resultSemordnilaps);
  printResults(10.0, 12.0, numWorkers, TOTAL_WORDS, palindromeCount, semordnilapCount, sums);
}
//End of document