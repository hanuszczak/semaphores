#ifndef _FIFO_H
#define _FIFO_H

#define NMAX 30
#define HEAD 30
#define TAIL 31
#define FREE 0


int * initFifo();

void push(int *fifo, int num);

int pop(int *fifo);

int ifOdd(int elem);

int checkFirst(int *fifo);

int countElem(int * fifo);

int countEven(int *fifo);

int countOdd(int *fifo);

#endif //_FIFO_H