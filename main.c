#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "fifo.h"
#include "proc.h"
#include "semaphore.h"
#include <sys/wait.h>

int main(int argc,  char* argv[]) {
        
    // initiating Fifo
    int *fifo = initFifo();

    // initiating table of waiting proccesses
    int *procWaiting = initProcWaiting();

    // initiating table of semaphores
    int semid, i;

    semid = semget(KEY, SEM_NUM, IPC_CREAT|0600);
    if(semid == -1){
        perror("Creating semaphores table\n");
        exit(1);
    }
    
    //  for(i = 0; i<SEM_NUM; i++) {
    //     if(semctl(semid, i, SETVAL, (int)1) == -1) {
    //         perror("Setting value of semaphore\n");
    //         exit(1);
    //     }
    // }

    for(i = 0; i<SEM_NUM - 2; i++) {
        if(semctl(semid, i, SETVAL, (int)1) == -1) {
            perror("Setting value of semaphore\n");
            exit(1);
        }
    }

    for(i = 0; i < 2; i++) {
        if(semctl(semid, i+3, SETVAL, (int)0) == -1) {
            perror("Setting value of semaphore\n");
            exit(1);
        }
    }

    createSubproc(&A1);
    createSubproc(&A2);
    createSubproc(&B1);
    createSubproc(&B2);

    while(wait(NULL) > 0) {}

    return 0;
}