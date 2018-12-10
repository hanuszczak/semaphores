#ifndef _PROC_H
#define _PROC_H


int* initProcWaiting();

void wakeUpProc(int semid, int *fifo, int* procWaiting, int procId);

void createSubproc(void (*subprocFunc)());

void A1();

void A2();

void B1();

void B2();

int produceA1(int *fifo, int i);

int produceA2(int *fifo, int i);

int consumeB1(int *fifo);

int consumeB2(int *fifo);

int canA1produce(int *fifo);

int canA2produce(int *fifo);

int canB1consume(int *fifo);

int canB2consume(int *fifo);


#endif //_PROC_H