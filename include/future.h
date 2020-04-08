#ifndef _FUTURE_H_
#define _FUTURE_H_

// #ifndef	NFUT
// #define	NFUT		120	/* Number of semaphores, if not defined	*/
// #endif

#include <myqueue.h>

typedef enum {
  FUTURE_EMPTY,
  FUTURE_WAITING,
  FUTURE_FREE,
  FUTURE_READY
} future_state_t;

typedef enum {
  FUTURE_EXCLUSIVE,
  FUTURE_SHARED,
  FUTURE_QUEUE
} future_mode_t;

typedef struct future_t {
  char *data;
  uint size;
  future_state_t state;
  future_mode_t mode;
  pid32 pid;   // used in FUTURE_EXCLUSIVE
  myqueue_t *set_queue;
  myqueue_t *get_queue;  // used in FUTURE_SHARED

  // new fields
  uint16 max_elems;
  uint16 count;
  uint16 head;
  uint16 tail;
} future_t;

// extern struct future_t futab[];

/* Interface for the Futures system calls */
future_t* future_alloc(future_mode_t mode, uint size, uint nelems);
syscall future_free(future_t*);
syscall future_get(future_t*, char*);
syscall future_set(future_t*, char*);
syscall resume_head_pid(myqueue_t *q);

#endif /* _FUTURE_H_ */