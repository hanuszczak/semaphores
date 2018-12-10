#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>


static struct sembuf buf;

int P(int semid, int semnum, int procID) {
    buf.sem_num = semnum;
    buf.sem_op = -1;
    semop(semid, &buf, 1);
    //printf("Proc %d put semaphore %d down\n", procID, semnum);
}

int V(int semid, int semnum, int procID) {
    buf.sem_num = semnum;
    buf.sem_op = 1;
    semop(semid, &buf, 1);
     //printf("Proc %d put semaphore %d up\n", procID, semnum);
}