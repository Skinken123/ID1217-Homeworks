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
sem_t bathroom, womenWait, menWait;

// Counters
int nrOfWaitingMen = 0, nrOfWaitingWomen = 0, nrOfPeopleInBathroom = 0;
char currentGender = 'N'; // 'M' for men, 'W' for women, 'N' for none

//Timer
void *man(void *arg) {
    int myid = *(int *) arg;
    while (1) {
        //Begin working
        sleep(rand() % 3 + 1);
        //Stop working want to go toilet
        sem_wait(&bathroom);
        if (nrOfPeopleInBathroom > 0) {
            nrOfWaitingMen++;
            sem_post(&bathroom);
            sem_wait(&menWait);
        }
        nrOfPeopleInBathroom++;
        printf("Man %d entered the bathroom. (Men remaining: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfWaitingMen > 0) {
            nrOfWaitingMen--;
            sem_post(&menWait);
        } else {
            sem_post(&bathroom);
        }
        //Sleep on toilet or watch yt video
        sleep(rand() % 2);
        //Exit bathroom
        sem_wait(&bathroom);
        nrOfPeopleInBathroom--;
        printf("Man %d left the bathroom. (Men remaining: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfPeopleInBathroom == 0 && nrOfWaitingWomen > 0) {
            printf("Women can now enter bathroom\n");
            sem_post(&womenWait);
        } else {
            sem_post(&bathroom);
        }
    }
}

void *women(void *arg) {
    int myid = *(int *) arg;
    while (1) {
        //Begin working
        sleep(rand() % 3 + 1);
        //Stop working want to go toilet
        sem_wait(&bathroom);
        if (nrOfPeopleInBathroom > 0) {
            nrOfWaitingWomen++;
            sem_post(&bathroom);
            sem_wait(&womenWait);
        }
        nrOfPeopleInBathroom++;
        printf("Woman %d entered the bathroom. (Woman remaining: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfWaitingWomen > 0) {
            nrOfWaitingWomen--;
            sem_post(&womenWait);
        } else {
            sem_post(&bathroom);
        }
        //Sleep on toilet or watch yt video
        sleep(rand() % 2);
        //Exit bathroom
        sem_wait(&bathroom);
        nrOfPeopleInBathroom--;
        printf("Woman %d left the bathroom. (Woman remaining: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfPeopleInBathroom == 0 && nrOfWaitingMen > 0) {
            printf("Men can now enter bathroom\n");
            sem_post(&menWait);
        } else {
            sem_post(&bathroom);
        }
    }
}

int main(int argc, char *argv[]) {
    pthread_t workers[MAX_WORKERS];
    int thread_ids[MAX_WORKERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    sem_init(&bathroom, SHARED, 1);
    sem_init(&womenWait, SHARED, 0);
    sem_init(&menWait, SHARED, 0);

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