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
#include <time.h>
#include <sys/time.h>
#define MAXWORDS 25143  /* maximum dictionary words from file*/
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived to the barrier*/

double start_time, end_time; /* start and end times of parallel execution*/
int sums[MAXWORKERS]; /* partial sums, how many palindromes/semordnilaps each worker/thread has found*/

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

/* calculate work load for each worker/thread */
void calculateWorkLoad(int (*array)[2] , int numberOfWorkers, int columns) {
  int workSlice = MAXWORDS/numberOfWorkers;
  int remainder = MAXWORDS - (workSlice*numberOfWorkers);
  int arrayIndex = 0;

  for (int i = 0; i < numberOfWorkers; i++) {
        for (int j = 0; j < columns; j++) {
            array[i][j] = arrayIndex; 
            if (j == 0) arrayIndex = arrayIndex + workSlice;
            if (i == (numberOfWorkers-1) && j == 0) arrayIndex = arrayIndex + remainder + 1;
        }
    }
}

/* Finding if word exists in dictionary */
bool binarySearch() {
}

/* Checks if a word is a palindrome */
bool isPalindrome() {
}

/* Method each worker/thread excecutes */
void *Worker(void *arg) {
  ThreadData *data = (ThreadData *) arg;
  int startIndex = data->startIndex;
  int endIndex = data->endIndex;
  int threadId = data->threadId;

  // Get search index range from pointer argument most likely from a struct

  // Analyze the array in the search index range and find all palindromes and semordnilaps
  // Store palindromes and semordnilaps in seperete arrays accessed by a pointer in a global array
  // and increment the count inside the global variable array

  // Use the barrier to enable one of the workers to write the semordnilaps and palindromes to a result text file
  // This worker will also print the total number of words found as well as how many each thread found
  // End timer
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