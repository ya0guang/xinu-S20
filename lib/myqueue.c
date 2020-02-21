#include <xinu.h>

int size_myqueue(myqueue_t *q)
{
    int result;
    if (q->head <= q->tail)
    {
        //printf("head: %d, tail: %d minus: %d \n", q->head, q->tail, q->tail - q->head);
        result = q->tail - q->head;
    }
    else
    {
        result = (q->tail + MYQUEUELEN - q->head) % MYQUEUELEN;
    }
    printf("DEBUG: result %d\n", result);
    return result;
}

int plus1(int index)
{
    return (index + 1) % MYQUEUELEN;
}

// int minus1(int index) {
//     return (index + MYQUEUELEN - 1) % MYQUEUELEN;
// }

int in_myquue(myqueue_t *q, pid32 p)
{
    int my_size;
    if (size_myqueue(q) >= MYQUEUELEN - 1)
    {
        return SYSERR;
    }
    else
    {
        q->parray[q->tail] = p;
        q->tail = plus1(q->tail);
        my_size = size_myqueue(q);
        printf("DEBUG: Put into queue, size: %d, head: %d, tail: %d\n", my_size, q->head, q->tail);
        return OK;
    }
}

pid32 out_myqueue(myqueue_t *q)
{
    pid32 result;

    if (size_myqueue(q) <= 0)
    {
        return (pid32)SYSERR;
    }

    else
    {
        result = q->parray[q->head];
        q->head = plus1(q->head);
        return result;
    }
}