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
#include "functions.h"
#include "print.h"
#define MAX_WORD_LENGTH 22  /* max byte length of words in file */
#define MAXWORKERS 16   /* maximum number of workers */

int totWords = 25143;
int numWords[3] = {25143, 12571, 6285};
int numWorkers = 0;           /* number of workers */ 
int palindromeCount = 0;
int semordnilapCount = 0;

double start_time, end_time; /* start and end times of parallel execution*/

int sums[MAXWORKERS][2]; /* partial sums, how many palindromes/semordnilaps each worker/thread has found*/
char **resultPalindromes = NULL; // Dynamic array for palindromes
char **resultSemordnilaps = NULL; // Dynamic array for Semordnilaps
char **words = NULL; // Array of words
double programTime[16][2];

void worker(){
 #pragma omp parallel num_threads(numWorkers) 
  {
    char **localPalindromes = NULL, **localSemordnilaps = NULL;
    int palIndex = 0, semIndex = 0;
    char reversed[MAX_WORD_LENGTH];
    int threadId = omp_get_thread_num();
    sums[threadId][0] = 0;
    sums[threadId][1] = 0;

    #pragma omp for schedule(guided,2) //static, dynamic,((totWords/numWorkers)-200), guided,2, runtime
      for (int i = 0; i < totWords; i++) {
        if (isPalindrome(words[i])) {
          localPalindromes = realloc(localPalindromes, (palIndex + 1) * sizeof(char *));
          localPalindromes[palIndex] = strdup(words[i]);
          palIndex++;
          sums[threadId][0]++;
        } else {
            strcpy(reversed, words[i]);
            reverseString(reversed);
            const char *reversedWord = reversed;
            if(binarySearch(words, reversedWord, totWords)) {
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
}

int main(int argc, char *argv[]) {

  /* parse dictionary file into global variable array*/
  if (readWordsFromFile("words", &words, totWords, MAX_WORD_LENGTH)) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
  }

  for (int wordV = 0; wordV < 3; wordV++){
    totWords = numWords[wordV];
    for (int argIndex = 1; argIndex < argc; argIndex++) {
      numWorkers = atoi(argv[argIndex]);
      if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
      if (numWorkers < 1) {
          fprintf(stderr, "Invalid numWorkers value: %d\n", numWorkers);
          continue;  // Skip invalid values
      }

      double totalExecutionTime = 0;
      for(int i = 0; i < 100; i++) {
        palindromeCount = 0;
        semordnilapCount = 0;
        start_time = omp_get_wtime();
        worker();
        end_time = omp_get_wtime();

        totalExecutionTime += (end_time - start_time);
      }
      double avgExecutionTime = totalExecutionTime / 100;
      programTime[argIndex-1][0] = avgExecutionTime;
      programTime[argIndex-1][1] = atoi(argv[argIndex]); 
    }
    writeResultsToFile(palindromeCount, semordnilapCount, resultPalindromes, resultSemordnilaps, wordV);
    printSpeedUp(programTime, argc-1, totWords);
  }
  return 0;
}
//End of document