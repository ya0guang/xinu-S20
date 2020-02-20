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
    future_t * futptr;

    mask = disable();

    for (i = 0; i < NFUT; i++)
    {
        futptr = &futab[i];

        if (futptr->state == FUTURE_EMPTY)
        {
            futptr->state = FUTURE_WAITING;
            futptr->mode = mode;
            futptr->data = getmem(size);
            if((int32)futptr->data == SYSERR) {
                restore(mask);
                return (future_t *)SYSERR;
            }
            return futptr;
        }
    }
    restore(mask);
    return (future_t *)SYSERR;
}