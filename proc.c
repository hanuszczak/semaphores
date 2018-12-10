#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "proc.h"
#include "semaphore.h"
#include "fifo.h"
#include <time.h>



int* initProcWaiting() {
    
    int shmid = shmget(KEY + 1, (SEM_NUM)*sizeof(int), IPC_CREAT|0600);
    if (shmid == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej na tablicę procesów oczekujących");
		exit(1);
    }

	int *procWaiting = (int*)shmat(shmid, NULL, 0);
    if (procWaiting == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }

    for(int i = 0; i < SEM_NUM; i++) {
        procWaiting[i] = FREE;
    }
    return procWaiting;
}

void wakeUpProc(int semid, int *fifo, int* procWaiting, int procId){
    if (procId != SEM_A1 && procWaiting[SEM_A1]>0 && canA1produce(fifo)) {
        V(semid, SEM_A1, procId);
        //printf("Proc %d waked up proc SEM_A1\n", procId );
    }
    else if (procId != SEM_A2 && procWaiting[SEM_A2]>0 && canA2produce(fifo)) {
        V(semid, SEM_A2, procId);
        //printf("Proc %d waked up proc SEM_A2\n", procId );
    }
    else if (procId != SEM_B1 && procWaiting[SEM_B1]>0 && canB1consume(fifo)) {
        V(semid, SEM_B1, procId);
        //printf("Proc %d waked up proc SEM_B1\n", procId );
    }
    else if (procId != SEM_B2 && procWaiting[SEM_B2]>0 && canB2consume(fifo)) {
        V(semid, SEM_B2, procId);
        //printf("Proc %d waked up proc SEM_B2\n", procId );
    }
    else {
        V(semid, MUTEX, procId);
        //printf("Proc %d waked up MUTEX\n", procId );
    }
        
}

void createSubproc(void (*subprocFunc)()) {
    
    int ForkResult = fork();
	if(ForkResult == 0){
		subprocFunc();
		exit(0);
	}
}

// Full proccesses

void A1() {
    int semid, shmidFifo, shmidProc;
    int *fifo, *procWaiting;
    semid = semget(KEY, SEM_NUM, IPC_CREAT|0600);
    if(semid == -1){
        perror("Creating semaphores table\n");
        exit(1);
    }
    shmidFifo = shmget(KEY, (NMAX+2)*sizeof(int), IPC_CREAT|0600);
    if (shmidFifo == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    fifo = (int*)shmat(shmidFifo, NULL, 0);
    if (fifo == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    shmidProc = shmget(KEY + 1, (SEM_NUM)*sizeof(int), IPC_CREAT|0600);
    if (shmidProc == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej na tablicę procesów oczekujących");
		exit(1);
    }
    procWaiting = (int*)shmat(shmidProc, NULL, 0);
    if (procWaiting == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    int num = 0;
    while(1) {
        P(semid, MUTEX, SEM_A1);
        if(!canA1produce(fifo)){
            ++procWaiting[SEM_A1];
            V(semid, MUTEX, SEM_A1);
            printf("Proc A1 went sleep \n");
            P(semid, SEM_A1, SEM_A1);
            --procWaiting[SEM_A1];
            printf("Proc A1 woke up \n");
        }
        int val;
        val = produceA1(fifo, num);
        num = (num + 2)%100;
        printf("Proc A1 pushed %d\n", val);
        wakeUpProc(semid, fifo, procWaiting, SEM_A1);
        usleep(500000);
    }
}

void A2() {
    int semid, shmidFifo, shmidProc;
    int *fifo, *procWaiting;
    semid = semget(KEY, SEM_NUM, IPC_CREAT|0600);
    if(semid == -1){
        perror("Creating semaphores table\n");
        exit(1);
    }
    shmidFifo = shmget(KEY, (NMAX+2)*sizeof(int), IPC_CREAT|0600);
    if (shmidFifo == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    fifo = (int*)shmat(shmidFifo, NULL, 0);
    if (fifo == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    shmidProc = shmget(KEY + 1, (SEM_NUM)*sizeof(int), IPC_CREAT|0600);
    if (shmidProc == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej na tablicę procesów oczekujących");
		exit(1);
    }
    procWaiting = (int*)shmat(shmidProc, NULL, 0);
    if (procWaiting == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    int num = 1;
    while(1) {
        P(semid, MUTEX, SEM_A2);
        if(!canA2produce(fifo)){
            ++procWaiting[SEM_A2];
            V(semid, MUTEX, SEM_A2);
            printf("Proc A2 went sleep\n");
            P(semid, SEM_A2, SEM_A2);
            --procWaiting[SEM_A2];
            printf("Proc A2 woke up\n");
        }
        int val;
        val = produceA2(fifo, num);
        num = (num + 2)%100;
        printf("Proc A2 pushed %d\n", val);
        wakeUpProc(semid, fifo, procWaiting, SEM_A2);
        usleep(500000);
    }   
}

void B1() {
    int semid, shmidFifo, shmidProc;
    int *fifo, *procWaiting;
    semid = semget(KEY, SEM_NUM, IPC_CREAT|0600);
    if(semid == -1){
        perror("Creating semaphores table\n");
        exit(1);
    }
    shmidFifo = shmget(KEY, (NMAX+2)*sizeof(int), IPC_CREAT|0600);
    if (shmidFifo == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    fifo = (int*)shmat(shmidFifo, NULL, 0);
    if (fifo == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    shmidProc = shmget(KEY + 1, (SEM_NUM)*sizeof(int), IPC_CREAT|0600);
    if (shmidProc == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej na tablicę procesów oczekujących");
		exit(1);
    }
    procWaiting = (int*)shmat(shmidProc, NULL, 0);
    if (procWaiting == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    while(1) {
        P(semid, MUTEX, SEM_B1);
        if(!canB1consume(fifo)){
            ++procWaiting[SEM_B1];
            V(semid, MUTEX, SEM_B1);
            printf("Proc B1 went sleep\n");
            P(semid, SEM_B1, SEM_B1);
            --procWaiting[SEM_B1];
            printf("Proc B1 woke up\n");
        }
        int val;
        val = consumeB1(fifo);
        printf("Proc B1 popped %d\n", val);
        wakeUpProc(semid, fifo, procWaiting, SEM_B1);
        usleep(500000);
    }
}

void B2() {
    int semid, shmidFifo, shmidProc;
    int *fifo, *procWaiting;
    semid = semget(KEY, SEM_NUM, IPC_CREAT|0600);
    if(semid == -1){
        perror("Creating semaphores table\n");
        exit(1);
    }
    shmidFifo = shmget(KEY, (NMAX+2)*sizeof(int), IPC_CREAT|0600);
    if (shmidFifo == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    fifo = (int*)shmat(shmidFifo, NULL, 0);
    if (fifo == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    shmidProc = shmget(KEY + 1, (SEM_NUM)*sizeof(int), IPC_CREAT|0600);
    if (shmidProc == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej na tablicę procesów oczekujących");
		exit(1);
    }
    procWaiting = (int*)shmat(shmidProc, NULL, 0);
    if (procWaiting == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }
    while(1){
        P(semid, MUTEX, SEM_B2);
        if(!canB2consume(fifo)){
            ++procWaiting[SEM_B2];
            V(semid, MUTEX, SEM_B2);
            printf("Proc B2 went sleep\n");
            P(semid, SEM_B2, SEM_B2);
            --procWaiting[SEM_B2];
            printf("Proc B2 woke up\n");
        }
        int val;
        val = consumeB2(fifo);
        printf("Proc B2 popped %d\n", val);
        wakeUpProc(semid, fifo, procWaiting, SEM_B2);
        usleep(500000);
    }
}

// Functions of producing/consuming by proccesses A1..B2

int produceA1 (int *fifo, int i) {
    push(fifo, i);
    return i;
}

int produceA2 (int *fifo, int i) {
    push(fifo, i);
    return i;
}

int consumeB1 (int *fifo) {
    int i;
    i = pop(fifo);
    return i;
}

int consumeB2 (int *fifo) {
    int i;
    i = pop(fifo);
    return i;
}

// Functions checking if proccesses A1..B2 can produce/consume

int canA1produce(int *fifo) {
    return (countEven(fifo) < 10);
}

int canA2produce(int *fifo) {
    return (countOdd(fifo) < countEven(fifo));
}

int canB1consume(int *fifo) {
    return (!ifOdd(checkFirst(fifo)) && countElem(fifo) >= 3);
}

int canB2consume(int *fifo) {
    return (ifOdd(checkFirst(fifo)) && countElem(fifo) >= 7);
}