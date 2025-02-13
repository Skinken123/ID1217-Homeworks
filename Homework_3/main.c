#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define SHARED 0
#define MAX_WORKERS 8

// Semaphores
sem_t mutex;      // Protects the shared state
sem_t womenWait;  // For waiting women
sem_t menWait;    // For waiting men

// Shared state variables
int nrOfWaitingMen = 0, nrOfWaitingWomen = 0;
int nrOfPeopleInBathroom = 0; // total persons currently in bathroom
char currentGender = 'N';     // 'M' if men are inside, 'W' if women, 'N' if empty

void *man(void *arg) {
    int myid = *(int *)arg;
    while (1) {
        // Simulate working
        sleep(rand() % 3 + 1);

        // Request to enter the bathroom
        sem_wait(&mutex);
        printf("Man aquired mutex\n");
        if (nrOfPeopleInBathroom == 0) {
            // Bathroom is empty; set current gender to male.
            currentGender = 'M';
        }
        if (currentGender == 'M') {
            // Same gender or empty—enter immediately.
            nrOfPeopleInBathroom++;
            printf("Man %d entered the bathroom. (Total inside: %d)\n", myid, nrOfPeopleInBathroom);
            // If there are other waiting men, let one proceed.
            if (nrOfWaitingMen > 0) {
                nrOfWaitingMen--;
                sem_post(&menWait);
            } else {
                sem_post(&mutex);
                printf("Man released mutex\n");
            }
        } else {
            // Bathroom is occupied by women. Must wait.
            nrOfWaitingMen++;
            sem_post(&mutex);
            sem_wait(&menWait);
            // Once signaled, re-acquire mutex to update state.
            sem_wait(&mutex);
            nrOfPeopleInBathroom++;
            printf("Man %d entered the bathroom. (Total inside: %d)\n", myid, nrOfPeopleInBathroom);
            if (nrOfWaitingMen > 0) {
                nrOfWaitingMen--;
                sem_post(&menWait);
            } else {
                sem_post(&mutex);
            }
        }

        // Simulate using the bathroom
        sleep(rand() % 2);

        // Exit the bathroom
        sem_wait(&mutex);
        nrOfPeopleInBathroom--;
        printf("Man %d left the bathroom. (Remaining inside: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfPeopleInBathroom == 0) {
            // Bathroom is now empty.
            if (nrOfWaitingWomen > 0) {
                currentGender = 'W'; // Switch gender for next access.
                nrOfWaitingWomen--;
                sem_post(&womenWait);
            } else {
                currentGender = 'N';
                sem_post(&mutex);
            }
        } else {
            sem_post(&mutex);
        }
    }
    return NULL;
}

void *women(void *arg) {
    int myid = *(int *)arg;
    while (1) {
        // Simulate working
        sleep(rand() % 3 + 1);

        // Request to enter the bathroom
        sem_wait(&mutex);
        printf("Woman aquired mutex\n");
        if (nrOfPeopleInBathroom == 0) {
            // Bathroom is empty; set current gender to female.
            currentGender = 'W';
        }
        if (currentGender == 'W') {
            // Same gender or empty—enter immediately.
            nrOfPeopleInBathroom++;
            printf("Woman %d entered the bathroom. (Total inside: %d)\n", myid, nrOfPeopleInBathroom);
            // If there are other waiting women, let one proceed.
            if (nrOfWaitingWomen > 0) {
                nrOfWaitingWomen--;
                sem_post(&womenWait);
            } else {
                sem_post(&mutex);
                printf("Woman released mutex\n");
            }
        } else {
            // Bathroom is occupied by men. Must wait.
            nrOfWaitingWomen++;
            sem_post(&mutex);
            sem_wait(&womenWait);
            // Once signaled, re-acquire mutex to update state.
            sem_wait(&mutex);
            nrOfPeopleInBathroom++;
            printf("Woman %d entered the bathroom. (Total inside: %d)\n", myid, nrOfPeopleInBathroom);
            if (nrOfWaitingWomen > 0) {
                nrOfWaitingWomen--;
                sem_post(&womenWait);
            } else {
                sem_post(&mutex);
            }
        }

        // Simulate using the bathroom
        sleep(rand() % 2);

        // Exit the bathroom
        sem_wait(&mutex);
        nrOfPeopleInBathroom--;
        printf("Woman %d left the bathroom. (Remaining inside: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfPeopleInBathroom == 0) {
            // Bathroom is now empty.
            if (nrOfWaitingMen > 0) {
                currentGender = 'M'; // Switch gender for next access.
                nrOfWaitingMen--;
                sem_post(&menWait);
            } else {
                currentGender = 'N';
                sem_post(&mutex);
            }
        } else {
            sem_post(&mutex);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t workers[MAX_WORKERS];
    int thread_ids[MAX_WORKERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    // Initialize semaphores.
    sem_init(&mutex, SHARED, 1);
    sem_init(&womenWait, SHARED, 0);
    sem_init(&menWait, SHARED, 0);

    srand(time(NULL)); // Seed random number generator.

    for (int i = 0; i < MAX_WORKERS; i++) {
        thread_ids[i] = i;
        if (i % 2 == 0) {
            pthread_create(&workers[i], &attr, women, &thread_ids[i]);
        } else {
            pthread_create(&workers[i], &attr, man, &thread_ids[i]);
        }
    }
    pthread_exit(NULL);
}