#include <xinu.h>
#include <stdlib.h>
#include <string.h>
#include <prodcons_bb.h>
#include <future_test.h>
#include <stream_proc.h>

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
        printf("futures_test\n");
        printf("tscdf\n");
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

    if (strncmp(args[0], "futures_test", 12) == 0)
    {
        
        if (strncmp(args[1], "-f", 3) == 0)
        {
            resume(create((void *)ffib_bb, 4096, 20, "ffib", 2, nargs, args));
        }

        if (strncmp(args[1], "-pc", 3) == 0)
        {
            resume(create((void *)future_prodcons_bb, 4096, 20, "ffib", 2, nargs, args));
        }

        //FUTURE_QUEUE tests
        if (strncmp(args[1], "-fq1", 4) == 0)
        {
            resume(create((void *)futureq_test1, 4096, 20, "futureq_test1", 2, nargs, args));
        }
        if (strncmp(args[1], "-fq2", 4) == 0)
        {
            resume(create((void *)futureq_test2, 4096, 20, "futureq_test2", 2, nargs, args));
        }
        if (strncmp(args[1], "-fq3", 4) == 0)
        {
            resume(create((void *)futureq_test3, 4096, 20, "futureq_test3", 2, nargs, args));
        }

    }

    if (strncmp(args[0], "tscdf", 5) == 0)
    {
        resume(create((void *)stream_proc, 4096, 20, "stream_proc", 2, nargs, args));
    }

    if (strncmp(args[0], "tscdf_fq", 8) == 0)
    {
        resume(create((void *)stream_proc_futures, 4096, 20, "tscdf_fq", 2, nargs, args));
    }

    if (strncmp(args[0], "test", 4) == 0)
    {
        mytest_bb();
    }

    return 0;
}

// Generate a string of process name with the seq# of process of the same name
void gen_proc_name(int proc, int process_seq_num, char *name)
{

    const char default_fun_name[] = "default";
    const char producer_fun_name[] = "producer_bb";
    const char consumer_fun_name[] = "consumer_bb";

    char *function_name;

    char number[3] = {0};

    switch (proc)
    {
        // 1 for producer
    case 1:
        function_name = producer_fun_name;
        break;
        // 2 for consumer
    case 2:
        function_name = consumer_fun_name;
        break;
    default:
        function_name = default_fun_name;
        break;
    }

    number[0] = (process_seq_num / 10) + 48;
    number[1] = (process_seq_num % 10) + 48;

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

    if ((producer_count * producer_iter) != (consumer_count * consumer_iter))
    {
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

    for (i = 0; i < producer_count; i += 1)
    {
        //create producer thread

        //generate the string or the process name
        char producer_name[25];
        gen_proc_name(1, i, producer_name);

        resume(create((void *)producer_bb, 4096, 20, producer_name, 1, producer_iter));
    }

    for (i = 0; i < consumer_count; i += 1)
    {
        //create consumer thread

        //generate the string or the process name
        char consumer_name[25];
        gen_proc_name(2, i, consumer_name);

        resume(create((void *)consumer_bb, 4096, 20, consumer_name, 1, consumer_iter));
    }
}

int future_prodcons_bb(int nargs, char *args[])
{
    future_t *f_exclusive, *f_shared;
    f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);
    f_shared = future_alloc(FUTURE_SHARED, sizeof(int), 1);

    // Test FUTURE_EXCLUSIVE
    resume(create(future_cons, 1024, 20, "fcons1", 1, f_exclusive));
    resume(create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, (char *)&one));

    // Test FUTURE_SHARED
    resume(create(future_cons, 1024, 20, "fcons2", 1, f_shared));
    resume(create(future_cons, 1024, 20, "fcons3", 1, f_shared));
    resume(create(future_cons, 1024, 20, "fcons4", 1, f_shared));
    resume(create(future_cons, 1024, 20, "fcons5", 1, f_shared));
    resume(create(future_prod, 1024, 20, "fprod2", 2, f_shared, (char *)&two));
}

int mytest_bb()
{
    future_t *f;
    int data_in = 1111;
    int data_out;
    int *ptr_in = &data_in;
    int *ptr_out = &data_out;

    f = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);
    future_set(f, (char *)ptr_in);
    future_get(f, (char *)ptr_out);
    printf("size: %d", data_out);
}

int ffib_bb(int nargs, char *args[])
{
    int fib = -1, i;

    fib = atoi(args[2]);

    if (fib > -1)
    {
        int final_fib;
        int future_flags = FUTURE_SHARED; // TODO - add appropriate future mode here

        // create the array of future pointers
        if ((fibfut = (future_t **)getmem(sizeof(future_t *) * (fib + 1))) == (future_t **)SYSERR)
        {
            printf("getmem failed\n");
            return (SYSERR);
        }

        // get futures for the future array
        for (i = 0; i <= fib; i++)
        {
            if ((fibfut[i] = future_alloc(future_flags, sizeof(int), 1)) == (future_t *)SYSERR)
            {
                printf("future_alloc failed\n");
                return (SYSERR);
            }
        }

        // spawn fib threads and get final value
        // TODO - you need to add your code here
        for (i = 0; i <= fib; i++)
        {
            resume(create(ffib, 1024, 20, "ffib", 1, i));
        }

        future_get(fibfut[fib], (char *)&final_fib);

        for (i = 0; i <= fib; i++)
        {
            future_free(fibfut[i]);
        }

        freemem((char *)fibfut, sizeof(future_t *) * (fib + 1));
        printf("Nth Fibonacci value for N=%d is %d\n", fib, final_fib);
        return (OK);
    }

    return (OK);
}
