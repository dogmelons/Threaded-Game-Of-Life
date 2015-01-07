#ifndef CYCLIC_BARRIER_H
#define CYCLIC_BARRIER_H

#include <pthread.h>

typedef struct cyclic_barrier_t {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  int participants;
  int outstanding;
  void (*barrier_action) (void);
} cyclic_barrier_t;

int cyclic_barrier_init(cyclic_barrier_t* barrier, pthread_mutexattr_t* barrier_mutexattr, pthread_condattr_t* barrier_condattr, int participants, void (*barrier_action)(void));

int cyclic_barrier_await(cyclic_barrier_t* barrier);

int cyclic_barrier_destroy(cyclic_barrier_t* barrier);

#endif
