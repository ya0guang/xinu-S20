/* future.c - future implementation */

#include <xinu.h>

/* ------------------------------------------
future_t* future_alloc(future_mode_t mode, uint size, uint nelems) - Allocates a new future (in the FUTURE_EMPTY state) with the given mode. We will use the getmem() call to allocate size amount of space for the new future's data.

Parameters:
    future_mode_t mode - The mode this future will operate in
    uint size - The size of the data element in bytes
    uint nelems - The number of elements (You will only need this when using the future in FUTURE_QUEUE mode, so you don't have to use this argument for this assignment - during implementation you can pass 1 in place for this)

Returns: 
    future_t - SYSERR or a pointer to a valid future
*/

future_t *future_alloc(future_mode_t mode, uint size, uint nelems)
{

    // save and disable interupt
    intmask mask;
    future_t *futptr;

    int i;

    mask = disable();

    for (i = 0; i < NFUT; i++)
    {
        futptr = &futab[i];

        if (futptr->state == FUTURE_FREE)
        {
            futptr->data = getmem(size);
            if ((int32)futptr->data == SYSERR)
            {
                restore(mask);
                return (future_t *)SYSERR;
            }

            futptr->state = FUTURE_EMPTY;
            futptr->mode = mode;
            futptr->size = size;
            futptr->set_queue = (struct myqueue_t *)getmem(sizeof(struct myqueue_t));
            futptr->get_queue = (struct myqueue_t *)getmem(sizeof(struct myqueue_t));

            futptr->set_queue->head = 0;
            futptr->set_queue->tail = 0;
            futptr->get_queue->head = 0;
            futptr->get_queue->tail = 0;
            restore(mask);
            return futptr;
        }
    }
    restore(mask);
    return (future_t *)SYSERR;
}

/*------------------------------------------
syscall future_free(future_t* f) - Frees the allocated future. Use the freemem() system call to free the data space.

Parameters:
    future_t* f - future to free

Returns: 
    syscall - SYSERR or OK
*/

syscall future_free(future_t *f)
{
    freemem(f->data, f->size);
    freemem((char *)f->set_queue, sizeof(struct myqueue_t));
    freemem((char *)f->get_queue, sizeof(struct myqueue_t));
    f->state = FUTURE_FREE;
    return OK;
}

/*---------------------------------
syscall future_get(future_t* f, char* out) - Get the value of a future set by an operation and may change the state of future.

Parameters:
    future_t* f - Future on which to wait for and obtain value
    char* out - Address into which the future's value will be copied.

Returns: 
    syscall - SYSERR or OK
*/
syscall future_get(future_t *f, char *out)
{
    int mask;
    mask = disable();

    struct procent *prptr; /* Ptr to process' table entry	*/

    prptr = &proctab[currpid];
    f->pid = currpid;

    // error future getting 
    if (f->state == FUTURE_FREE)
    {
        printf("Trying to get a free future");
        return SYSERR;
    }

    if (f->state == FUTURE_WAITING && f->mode == FUTURE_EXCLUSIVE)
    {
        printf("Trying to get from an exclusive future more than once");
        return SYSERR;
    }

    // reschedule
    if(f->state != FUTURE_READY) {
        in_myquue(f->get_queue, currpid);
        prptr->prstate = PR_WAIT;
        f->state = FUTURE_WAITING;
        resched();
    }

    if(f->state == FUTURE_READY) {
        memcpy((void *)f->data, (void *)out, f->size);
        if(f->mode == FUTURE_EXCLUSIVE) {
            f->state = FUTURE_EMPTY;
        }
    }

    restore(mask);
    return OK;
}

/*
syscall future_set(future_t* f, char* in) - Set a value in a future and may change the state of future.

Parameters:
    future_t* f - Future in which to set the value
    char* in - Result of an operation to be set as data in the future

Returns: syscall - SYSERR or OK
*/

syscall future_set(future_t* f, char* in) {
    int mask;
    mask = disable();

    pid32 pid_to_ready;

    // error future getting 
    if (f->state == FUTURE_FREE)
    {
        printf("Trying to set a free future");
        return SYSERR;
    }

    if (f->state == FUTURE_READY && f->mode == FUTURE_EXCLUSIVE)
    {
        printf("Trying to set a ready future for an exclusive future more than once");
        return SYSERR;
    }

    while (size_myqueue(f->get_queue))
    {
        pid_to_ready = out_myqueue(f->get_queue);
        ready(pid_to_ready);
    }

    restore(mask);
    retrun OK;
}