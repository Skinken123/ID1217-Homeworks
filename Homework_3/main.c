#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define SHARED 1
#define MAX_WORKERS 16

sem_t enterBathroom, womenWait, menWait;
int nrOfWaitingMen, nrOfWaitingWomen, nrOfPeopleInBathroom;

void *man(void *arg) {

}

void *women(void *arg) {
    
}

int main(int argc, char *argv[]) {
    pthread_t workerId[MAX_WORKERS];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    sem_init(&enterBathroom, SHARED, 1);
    sem_init(&womenWait, SHARED, 0);
    sem_init(&menWait, SHARED, 0);
}