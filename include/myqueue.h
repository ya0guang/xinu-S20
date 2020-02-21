
#ifndef MYQUEUELEN
#define MYQUEUELEN 100
#endif

#ifndef MYQUEUE_H
#define MYQUEUE_H

typedef struct myqueue_t {
    int head;
    int tail; //index of the end of the array which doesn't hold a value
    pid32 parray[MYQUEUELEN];
} myqueue_t;


int size_myqueue(myqueue_t* q);
int in_myquue(myqueue_t* q, pid32 p);
pid32 out_myqueue(myqueue_t* q);

#endif