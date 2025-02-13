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
#define MAX_WORKERS 16

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
        sleep(rand() % 2 + 2);
        //Exit bathroom
        sem_wait(&bathroom);
        nrOfPeopleInBathroom--;
        printf("Man %d left the bathroom. (Men remaining: %d)\n", myid, nrOfPeopleInBathroom);
        if (nrOfPeopleInBathroom == 0) {
            if (nrOfWaitingWomen > 0) {
                printf("Women can now enter bathroom\n\n");
                nrOfWaitingWomen--;
                sem_post(&womenWait);
            } else if(nrOfWaitingMen > 0){
                printf("No women were waiting, men can enter\n\n");
                nrOfWaitingMen--;
                sem_post(&menWait);
            } else{
                printf("No one was waiting, anyone can enter\n\n");
                sem_post(&bathroom);
            }
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
        if (nrOfPeopleInBathroom == 0) {
            if (nrOfWaitingMen > 0) {
                printf("Men can now enter bathroom\n\n");
                nrOfWaitingMen--;
                sem_post(&menWait);
            } else if(nrOfWaitingWomen > 0){
                printf("No men were waiting, men can enter\n\n");
                nrOfWaitingWomen--;
                sem_post(&womenWait);
            } else{
                printf("No one was waiting, anyone can enter\n\n");
                sem_post(&bathroom);
            }
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

    for (int i = 0; i < MAX_WORKERS/2; i++) {
        thread_ids[i] = i;
        pthread_create(&workers[i], &attr, women, &thread_ids[i]);
        pthread_create(&workers[i], &attr, man, &thread_ids[i]);
    }
    pthread_exit(NULL);
}