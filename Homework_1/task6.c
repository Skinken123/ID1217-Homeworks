/* Finding Palindromes & Semordnilaps

  This document was originaly written by Alexander Nordgren, Gustav Moritz 

  Description:

  The program when excuted finds all Palindromes & Semordnilaps of a text file,
  and then stores all of the words/phrases inside a new text file. The program also
  shows how many words where found by each worker/thread, the total amount of words/phrases found, 
  and the excecution time of the program. 

*/
#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#define TOTAL_WORDS 25143  /* maximum dictionary words from file*/
#define MAX_WORD_LENGTH 22  /* max byte length of words in file */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_mutex_t palindrome; /* mutex lock for accessing palindrome array */
pthread_mutex_t semordnilaps; /* mutex lock for accessing semordnilaps array*/
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers = 0;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived to the barrier*/
int palindromeCount = 0;
int semordnilapCount = 0;

double start_time, end_time; /* start and end times of parallel execution*/

int sums[MAXWORKERS][2]; /* partial sums, how many palindromes/semordnilaps each worker/thread has found*/
char **resultPalindromes = NULL; // Dynamic array for palindromes
char **resultSemordnilaps = NULL; // Dynamic array for Semordnilaps
char **words = NULL; // Array of words

/* Struct storing data for each thread */
typedef struct {
  int startIndex;
  int endIndex;
  int threadId;
} ThreadData;

/* a reusable counter barrier to make sure all workers/threads has analyzed its section of the dictionary*/
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

/**
 * Function to read words from a file into an array.
 * Returns the number of words read or -1 on failure.
 */
int readWordsFromFile(const char *filename, char ***words, int maxWords) {
    FILE *file;
    char buffer[MAX_WORD_LENGTH];
    int count = 0;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Allocate memory for the array of word pointers
    *words = malloc(maxWords * sizeof(char *));
    if (*words == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return -1;
    }

    // Read words from the file
    while (count < maxWords && fscanf(file, "%s", buffer) == 1) {
        (*words)[count] = malloc(strlen(buffer) + 1); // Allocate memory for each word
        if ((*words)[count] == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            return -1;
        }
        strcpy((*words)[count], buffer); // Copy the word into the allocated space
        count++;
    }

    fclose(file);
    return 0; // Return the number of words read
}

/* calculate work load for each worker/thread */
void calculateWorkLoad(int (*array)[2] , int numberOfWorkers, int columns) {
  int workSlice = TOTAL_WORDS/numberOfWorkers;
  int remainder = TOTAL_WORDS - (workSlice*numberOfWorkers);
  int arrayIndex = 0;

  for (int i = 0; i < numberOfWorkers; i++) {
        for (int j = 0; j < columns; j++) {
            array[i][j] = arrayIndex; 
            if (j == 0) arrayIndex = arrayIndex + workSlice;
            if (i == (numberOfWorkers-1) && j == 0) arrayIndex = arrayIndex + remainder;
        }
    }
}

/* Checks if a word is a palindrome */
bool isPalindrome(const char *str) {
    int left = 0;
    int right = strlen(str) - 1;

    while (left < right) {
        if (tolower(str[left]) != tolower(str[right])) {
            return false;
        }
        left++;
        right--;
    }
    return true;
}

/* Reverses a string */
void reverseString(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}

/* Binary Searches for word */
bool binarySearch(char **words, const char *target) {
    int left = 0;
    int right = TOTAL_WORDS - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcasecmp(words[mid], target);

        if (cmp == 0) {
            return true; 
        } else if (cmp < 0) {
            left = mid + 1; 
        } else {
            right = mid - 1; 
        }
    }
    return false; 
}

/* Function to write results to a file */
void writeResultsToFile(const char *filename) {
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

/* Method each worker/thread excecutes */
void *Worker(void *arg) {
  ThreadData *data = (ThreadData *) arg;
  int startIndex = data->startIndex;
  int endIndex = data->endIndex;
  int threadId = data->threadId;

  // Analyze the array in the search index range and find all palindromes and semordnilaps
  // Store palindromes and semordnilaps in seperete arrays accessed by a pointer in a global array
  // and increment the count inside the global variable array
  char reversed[MAX_WORD_LENGTH];
  sums[threadId][0] = 0;
  sums[threadId][1] = 0;
  for (int i = startIndex; i < endIndex; i++) { 
      if (isPalindrome(words[i])) {
        // Lock
        pthread_mutex_lock(&palindrome);
        resultPalindromes = realloc(resultPalindromes, (palindromeCount + 1) * sizeof(char *));
        if (resultPalindromes == NULL) {
            perror("Memory allocation failed");
            return NULL;
        }

        resultPalindromes[palindromeCount] = malloc(strlen(words[i]) + 1);
        if (resultPalindromes[palindromeCount] == NULL) {
            perror("Memory allocation failed");
            return NULL;
        }
        strcpy(resultPalindromes[palindromeCount], words[i]);
        palindromeCount++;
        // Unlock
        pthread_mutex_unlock(&palindrome);
        sums[threadId][0]++;
      }
      else {
        strcpy(reversed, words[i]);
        reverseString(reversed);
        const char *testWord = reversed;
        if (binarySearch(words, testWord)) {
          // Lock
          pthread_mutex_lock(&semordnilaps);
          resultSemordnilaps = realloc(resultSemordnilaps, (semordnilapCount + 1) * sizeof(char *));
            if (resultSemordnilaps == NULL) {
              perror("Memory allocation failed");
              return NULL;
            }

          resultSemordnilaps[semordnilapCount] = malloc(strlen(words[i]) + 1);
            if (resultSemordnilaps[semordnilapCount] == NULL) {
              perror("Memory allocation failed");
              return NULL;
            }
            strcpy(resultSemordnilaps[semordnilapCount], words[i]);
            semordnilapCount++;
            // Unlock
            pthread_mutex_unlock(&semordnilaps);
            sums[threadId][1]++;
        }
      }
  }

  Barrier();
  end_time = read_timer();
  if (threadId == 0) {
  const int BOX_WIDTH = 55;
  writeResultsToFile("results.txt");

  // Print the header
  for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n| %-*s %-*s |\n", 40, "Word Analysis Results", BOX_WIDTH - 40 - 5, "");
  for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");

  // Total words
  printf("| %-40s %10d |\n", "Total number of words:", TOTAL_WORDS);
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
  printf("| %-38s %g sec |\n", "Program execution time:", end_time - start_time);
  for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");

  // Worker results
  for (int i = 0; i < numWorkers; i++) {
      printf("| Worker %-2d Found %3d Palindromes & %3d Semordnilaps %-1s|\n",
            i, sums[i][0], sums[i][1]);
      for (int i = 0; i < BOX_WIDTH; i++) printf("-");
        printf("\n");
  }

  // Footer
  for (int i = 0; i < BOX_WIDTH; i++) printf("=");
    printf("\n");


    
    // Free memory allocated for words
    free(resultSemordnilaps); // Free the array of pointers
    free(resultPalindromes); // Free the array of pointers
    for (int i = 0; i < TOTAL_WORDS; i++) {
        free(words[i]);
    }
    free(words);

    
  }
  // Use the barrier to enable one of the workers to write the semordnilaps and palindromes to a result text file
  // This worker will also print the total number of words found as well as how many each thread found
  // End timer
  return NULL;
}

/*
 int argc is the number of command line arguments passed to the program

 char *argv[] is an array of character pointers, where each element points 
 to a null-terminated string representing an argument passed to the program.
*/
int main(int argc, char *argv[]) {
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  numWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  /* parse dictionary file into global variable array*/
  if (readWordsFromFile("words", &words, TOTAL_WORDS)) {
        fprintf(stderr, "Failed to read words from file.\n");
        return 1;
  }
  /* calculate work load for each worker/thread*/
  int threadWorkLoadData[numWorkers][2];
  calculateWorkLoad(threadWorkLoadData, numWorkers, 2);

  /* Create array of data structs, one for each thread to be passed at thread creation*/
  ThreadData dataArray[numWorkers];
  for (int i = 0; i < numWorkers; i++){
    dataArray[i].startIndex = threadWorkLoadData[i][0];
    dataArray[i].endIndex = threadWorkLoadData[i][1];
    dataArray[i].threadId = i;
  }

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, &dataArray[l]);
  pthread_exit(NULL);
}