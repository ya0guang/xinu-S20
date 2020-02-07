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

//run prodcons_bb 20 10 50 100

shellcmd xsh_run(int nargs, char *args[])
{

    // check if the user wants  to list all avaliable commands
    if ((nargs == 1) || (strncmp(args[1], "list", 5) == 0))
    {
        printf("prodcons_bb\n");
        return OK;
    }

    /* This will go past "run" and pass the function/process name and its
    * arguments.
    */
    args++;
    nargs--;

    if (strncmp(args[0], "prodcons_bb", 11) == 0)
    {
        /* create a process with the function as an entry point. */
        resume(create((void *)prodcons_bb, 4096, 20, "prodcons_bb", 2, nargs, args));
    }
    
    return 0;
}

// Generate a string of process name with the seq# of process of the same name
void gen_proc_name(int proc, int process_seq_num, char* name){

    const char default_fun_name[] = "default";
    const char producer_fun_name[] = "producer_bb";
    const char consumer_fun_name[] = "consumer_bb";

    char* function_name;

    char number[3] = {0};

    switch (proc)
    {
        // 1 for producer
    case 1: function_name = producer_fun_name;
        break;
        // 2 for consumer
    case 2: function_name = consumer_fun_name;
        break;
    default: function_name = default_fun_name;
        break;
    }

    number[0] = process_seq_num / 10;
    number[1] = process_seq_num % 10;

    strncpy(name, function_name, 20);
    strncat(name, number, 3);
    
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
    //nothing to do here

    //create and initialize semaphores to necessary values
    bb_mutex = semcreate(1);
    bb_available = semcreate(0);
    bb_buf = semcreate(STORAGEBUF);

    //initialize read and write indices for the queue
    bb_index_read = 0;
    bb_index_write = 0;

    //create producer and consumer processes and put them in ready queue
    int i;
    const char producer_fun_name[] = "producer_bb";
    const char consumer_fun_name[] = "consumer_bb";

    for(i = 0; i < producer_count; i += 1){
        //create producer thread

        //generate the string or the process name
        char producer_name[25];
        gen_proc_name(1, i, producer_name);

        resume(create((void *)producer_bb, 4096, 20, producer_name, 1, producer_iter));
    }

    for(i = 0; i < consumer_count; i += 1){
        //create consumer thread

        //generate the string or the process name
        char consumer_name[25];
        gen_proc_name(2, i, producer_name);

        resume(create((void *)consumer_bb, 4096, 20, consumer_name, 1, consumer_iter));
    }

}