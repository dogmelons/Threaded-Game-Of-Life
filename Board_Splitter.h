//Splits game board into parts depending on number of threads.
//Splits by rows.

#include "cyclic_barrier.h"

typedef struct thread_param_t {
    int length;
    int firstRow;
    int firstCol;
    cyclic_barrier_t* barrier;
} thread_param_t;

int splitBoard(thread_param_t* threadList, int threadCount, int rows, int columns);