/* delqueue.c - delqueue */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  delqueue 
 *------------------------------------------------------------------------
 */
qid16	delqueue(qid16 q)
{
  intmask mask;

  mask = disable();

  if (q < NPROC || q >= NQENT) {
    restore(mask);
    return(SYSERR);
  }

  /* we might need to check for items in the queue here */
	queuetab[queuehead(q)].qnext = EMPTY;
  restore(mask);
	return OK;
}
