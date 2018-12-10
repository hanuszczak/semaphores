#include <stdlib.h>
#include "fifo.h"
#include "semaphore.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <stdio.h>

int* initFifo(){
    int shmid = shmget(KEY, (NMAX+2)*sizeof(int), IPC_CREAT|0600);
    if (shmid == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
    }

	int *buf = (int*)shmat(shmid, NULL, 0);
    if (buf == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
    }

    for(int i = 0; i < NMAX; i++) buf[i] = FREE;
    buf[HEAD] = 0;
    buf[TAIL] = 0;
    return buf;
}

void push(int *fifo, int num)
{
    fifo[fifo[TAIL]] = num;
    fifo[TAIL]++;
    fifo[TAIL] = fifo[TAIL]%NMAX;
    return;
}
int pop(int *fifo)
{
    if(fifo[HEAD]==fifo[TAIL]){
        return -1;
    }
    int num = fifo[fifo[HEAD]];
    fifo[fifo[HEAD]] = FREE;
    fifo[HEAD]++;
    fifo[HEAD] = fifo[HEAD]%NMAX;
    return num;
}
int checkFirst(int *fifo){
    if(fifo[HEAD]==fifo[TAIL]){
        return -1;
    }
    else
        return fifo[fifo[HEAD]];
}
int countElem(int *fifo) {
    int i, n;
    n = 0;
    i = fifo[HEAD];
    while(i != fifo[TAIL]) {
        n++;
        i++;
        i=i%30;
    }
    return n;
}
int ifOdd(int i) {
    return i&1;
}
int countEven(int *fifo) {
    int i, n;
    n = 0;
    i = fifo[HEAD];
    while(i != fifo[TAIL]) {
        if(!ifOdd(fifo[i])){
            n++;
        }
        i++;
        i = i%30;
    }
    return n;
}
int countOdd(int *fifo) {
    int i, n;
    n = 0;
    i = fifo[HEAD];
    while(i != fifo[TAIL]) {
        if(ifOdd(fifo[i])){
            n++;
        }
        i++;
        i = i%30;
    }
    return n;
}