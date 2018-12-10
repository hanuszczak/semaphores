#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#define KEY 737482

#define MUTEX 0
#define SEM_A1 1
#define SEM_A2 2
#define SEM_B1 3
#define SEM_B2 4

#define SEM_NUM 5

int P(int semid, int semnum, int procID);
int V(int semid, int semnum, int procID);

#endif
