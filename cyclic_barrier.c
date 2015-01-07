#include "cyclic_barrier.h"
#include <pthread.h>

/* This  implementation  assumes  that  there are  no  truly  spurious
 * returns from pthread_cond_await.  This  can be dealt with by adding
 * a  second outstanding  variable  and switching  between  them in  a
 * master-slave or two-phase manner.
 *
 * This implementation does not  return meaningful error codes instead
 * they always return 0.
 */

int cyclic_barrier_init(cyclic_barrier_t* barrier, pthread_mutexattr_t* barrier_mutexattr, pthread_condattr_t* barrier_condattr,  int participants, void (*barrier_action)(void)) {
  pthread_mutex_init(&(barrier->barrier_mutex), barrier_mutexattr);
  pthread_cond_init(&(barrier->barrier_cond), barrier_condattr);
  barrier->participants = participants;
  barrier->outstanding = participants;
  barrier->barrier_action = barrier_action;

  return 0;
}

int cyclic_barrier_await(cyclic_barrier_t* barrier)
{
  pthread_mutex_lock(&(barrier->barrier_mutex));

  barrier->outstanding = barrier->outstanding - 1;

  if (0 != barrier->outstanding) {
    pthread_cond_wait(&(barrier->barrier_cond), &(barrier->barrier_mutex));
  } else {
    (barrier->barrier_action)();
    barrier->outstanding = barrier->participants;
    pthread_cond_broadcast(&(barrier->barrier_cond));
  }

  pthread_mutex_unlock(&(barrier->barrier_mutex));

  return 0;
}

int cyclic_barrier_destroy(cyclic_barrier_t* barrier)
{
  pthread_mutex_destroy(&(barrier->barrier_mutex));
  pthread_cond_destroy(&(barrier->barrier_cond));

  return 0;
}
