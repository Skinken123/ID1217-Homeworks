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

/* Finding if word exists in dictionary */
bool binarySearch(){
}

/* Checks if a word is a palindrome */
bool isPalindrome(){

}

/*
 int argc is the number of command line arguments passed to the program

 char *argv[] is an array of character pointers, where each element points 
 to a null-terminated string representing an argument passed to the program.
*/
int main(int argc, char *argv[]){

}