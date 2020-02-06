#include <xinu.h>
#include <stdlib.h>
#include <string.h>
#include <prodcons_bb.h>

// definition of array, semaphores and indices

int bb_array[STORAGEBUF];
sid32 bb_mutex, bb_available, bb_buf;
int bb_index_write;
int bb_index_read;
int bb_number = 1;

shellcmd xsh_prodcons(int nargs, char *args[])
{
    //Argument verifications and validations
    int count = 2000; //local varible to hold count

    //check args[1] if present assign value to count

    if ((nargs == 1) || (strncmp(args[1], "list", 5) == 0))
    {
        printf("my_function_1\n");
        printf("my_function_2\n");
        return OK;
    }

    /* This will go past "run" and pass the function/process name and its
    * arguments.
    */
    args++;
    nargs--;

    if (strncmp(args[0], "my_function_1", 13) == 0)
    {
        /* create a process with the function as an entry point. */
        resume(create((void *)my_function_1, 4096, 20, "my_func_1", 2, nargs, args));
    }
}

void prodcons_bb(int nargs, char *args[])
{
    int producer_count;
    int consumer_count;
    int producer_iter;
    int consumer_iter;

    //check arguments
    if (nargs != 5)
    {
        fprintf(stderr, "%s: too many or not enough arguments\n", args[0]);
        fprintf(stderr, "Usage: %s [# of producer processes] [# of consumer processes] [# of iterations the producer runs] [# of iterations the consumer runs]\n", args[0]);
        return 1;
    }

    // assign arguments
    producer_count = atoi(args[1]);
    consumer_count = atoi(args[2]);
    producer_iter = atoi(args[3]);
    consumer_iter = atoi(args[4]);

    if((producer_count * producer_iter) != (consumer_count * consumer_iter)) {
        fprintf(stderr, "%s: argument error, \n", args[0]);
        fprintf(stderr, "Make sure that the number of total producer iterations and the number of total consumer iterations are equal. \n");
        return 1;
    }

    //initialize the array
    const int size = producer_count * producer_iter;
    int *bb_array = (int *)malloc(size * sizeof(int));

    //create and initialize semaphores to necessary values
    bb_mutex = semcreate(1);
    bb_available = semcreate(0);
    bb_buf = semcreate(STORAGEBUF);

    //initialize read and write indices for the queue
    bb_index_read = 0;
    bb_index_write = 0;

    //create producer and consumer processes and put them in ready queue
    int i;
    for(i = 0; i < producer_count; i += 1){
        //create producer thread
    }

    for(i = 0; i < consumer_count; i += 1){
        //create consumer thread
    }

    return 0;
}