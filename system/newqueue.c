/* newqueue.c - newqueue */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  newqueue  -  Allocate and initialize a queue in the global queue table
 *------------------------------------------------------------------------
 */
qid16	newqueue(void)
{
	static qid16	nextqid=NPROC;	/* Next list in queuetab to use	*/
	qid16		q;		/* ID of allocated queue 	*/
  int32   i;
  intmask mask;
  
  mask = disable();
  
	q = nextqid;

  for(i=0; i < NQENT; i+=2) {
    if (q >= NQENT) {
      q = NPROC;
    }
    /* qnext == EMPTY indicates an unallocated queue */
    if ( queuetab[q].qnext == EMPTY ) {
      /* Initialize head and tail nodes to form an empty queue */
      queuetab[queuehead(q)].qnext = queuetail(q);
      queuetab[queuehead(q)].qprev = EMPTY;
      queuetab[queuehead(q)].qkey  = MAXKEY;
      queuetab[queuetail(q)].qnext = EMPTY;
      queuetab[queuetail(q)].qprev = queuehead(q);
      queuetab[queuetail(q)].qkey  = MINKEY;
      nextqid = q + 2;			/* Increment index for next call*/
      restore(mask);
      return q;
    }
    q += 2;
  }

  restore(mask);
  return SYSERR;
}
