#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define SHARED 1
#define MAX_WORKERS 16

// Semophores 
sem_t enterBathroom, womenWait, menWait;

// Counters 
int nrOfWaitingMen = 0, nrOfWaitingWomen = 0, nrOfPeopleInBathroom = 0;
char currentGender = 'N'; // 'M' for men, 'W' for women, 'N' for none

void *man(void *arg) {
    int myid = *(int *)arg;
    while(1) {
        sleep(rand() % 3 + 1);

        sem_wait(&enterBathroom);
        if (currentGender == 'W') {
            nrOfWaitingMen++;
            sem_post(&enterBathroom);
            sem_wait(&menWait);
        } else {
            currentGender = 'M';
            nrOfPeopleInBathroom++;
            sem_post(&enterBathroom);
        }

        printf("Man %ld entered the bathroom. (Men in: %d)\n", myid, nrOfPeopleInBathroom);
        sleep(rand() % 2 + 1);

        sem_wait(&enterBathroom);
        nrOfPeopleInBathroom--;
        printf("Man %ld left the bathroom. (Men remaining: %d)\n", myid, nrOfPeopleInBathroom);

        if (nrOfPeopleInBathroom == 0) {
            currentGender = 'N';
            if (nrOfWaitingWomen > 0) {
                nrOfWaitingWomen --;
                currentGender = 'W';
                sem_post(&womenWait);
            } else if (nrOfWaitingMen > 0) {
                nrOfWaitingMen--;
                currentGender = 'M';
                sem_post(&menWait);
            }
        } else {
            sem_post(&menWait);
        }
        sem_post(&enterBathroom);
    }
    return NULL;
}

void *women(void *arg) {
    int myid = *(int *)arg;
    while(1) {
        sleep(rand() % 3 + 1);

        sem_wait(&enterBathroom);
        if (currentGender == 'M') {
            nrOfWaitingWomen++;
            sem_post(&enterBathroom);
            sem_wait(&womenWait);
        } else {
            currentGender = 'W';
            nrOfPeopleInBathroom++;
            sem_post(&enterBathroom);
        }

        printf("Woman %ld entered the bathroom. (Women in: %d)\n", myid, nrOfPeopleInBathroom);
        sleep(rand() % 2 + 1);

        sem_wait(&enterBathroom);
        nrOfPeopleInBathroom--;
        printf("Woman %ld left the bathroom. (Women remaining: %d)\n", myid, nrOfPeopleInBathroom);

        if (nrOfPeopleInBathroom == 0) {
            currentGender = 'N';
            if (nrOfWaitingMen > 0) {
                nrOfWaitingMen --;
                currentGender = 'M';
                sem_post(&menWait);
            } else if (nrOfWaitingWomen > 0) {
                nrOfWaitingWomen --;
                currentGender = 'W';
                sem_post(&womenWait);
            }
        } else {
            sem_post(&womenWait);
        }
        sem_post(&enterBathroom);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t workers[MAX_WORKERS];
    int thread_ids[MAX_WORKERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    sem_init(&enterBathroom, SHARED, 1);
    sem_init(&womenWait, SHARED, 0);
    sem_init(&menWait, SHARED, 0);

    srand(time(NULL));

    for (int i = 0; i < MAX_WORKERS; i++) {
        thread_ids[i] = i;
        if (i % 2 == 0) {
            pthread_create(&workers[i], &attr, women, &thread_ids[i]);
        } else {
            pthread_create(&workers[i], &attr, man, &thread_ids[i]);
        }
    }

    for (long l = 0; l < MAX_WORKERS; l++) {
        pthread_join(workers[l], NULL);
    }

    sem_destroy(&enterBathroom);
    sem_destroy(&womenWait);
    sem_destroy(&menWait);
}