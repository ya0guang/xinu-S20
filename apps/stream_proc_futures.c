#include <xinu.h>
#include <stream_proc.h>
// #include "tscdf_input.h"
#include "tscdf.h"


int stream_proc_futures(int nargs, char *args[])
{
    /* For Timing */
    ulong secs, msecs, time;
    secs = clktime;
    msecs = clkticks;

    // Parse arguments
    int i;
    char *ch;
    char c;

    char consumer_name[] = "stream_consumer_future?";

    // Parse arguments

    /* default arguments */
    int num_streams = 10;
    int work_queue_depth = 15;

    char usage[] = "Usage: -s num_streams -w work_queue_depth -t time_window -o output_time\n";

    /* Parse arguments out of flags */
    /* if not even # args, print error and exit */
    if (!(nargs % 2))
    {
        printf("%s", usage);
        return (-1);
    }
    else
    {
        i = nargs - 1;
        while (i > 0)
        {
            ch = args[i - 1];
            c = *(++ch);

            switch (c)
            {
            case 's':
                num_streams = atoi(args[i]);
                break;

            case 'w':
                work_queue_depth = atoi(args[i]);
                break;

            case 't':
                time_window = atoi(args[i]);
                break;

            case 'o':
                output_time = atoi(args[i]);
                break;

            default:
                printf("%s", usage);
                return (-1);
            }

            i -= 2;
        }
    }

    if ((pcport = ptcreate(num_streams)) == SYSERR)
    {
        kprintf("ptcreate failed\n");
        return (-1);
    }

    // Create array to hold `n_streams` number of futures
    future_t * farray[num_streams];

    // Create consumer processes and allocate futures
    // Use `i` as the stream id.

    pid32 c_pid;

    for (i = 0; i < num_streams; i++)
    {
        //create FUTURE_QUEUE for streaming
        // Future mode = FUTURE_QUEUE
        // Size of element = sizeof(struct data_element)
        // Number of elements = work_queue_depth
        farray[i] = future_alloc(FUTURE_QUEUE, sizeof(de), work_queue_depth);

        // Create consumer process
        consumer_name[15] = (char)(48 + i);

        c_pid = create((void *)stream_consumer_future, 4096, 20, consumer_name, 2, i, farray[i]);
        resume(c_pid);
        kprintf("stream_consumer_future id:%d (pid:%d) \n", i, c_pid);
    }

    // Parse input header file data and set future values
    char *a;
    int st;
    de de_to_set;
    for (i = 0; i < n_input; i++)
    {

        a = (char *)stream_input[i];
        st = atoi(a);
        while (*a++ != '\t')
            ;
        de_to_set.time = atoi(a);
        while (*a++ != '\t')
            ;
        de_to_set.value = atoi(a);

        future_set(farray[st], (char *)&de_to_set);
    }

    // Wait for all consumers to exit
    // Use `i` as the stream id.
    for (i = 0; i < num_streams; i++)
    {
        uint32 pm;
        pm = ptrecv(pcport);
        kprintf("process %d exited\n", pm);
    }
    
    ptdelete(pcport, 0);

    // free futures array
    for (i = 0; i < num_streams; i++)
    {
        future_free(farray[i]);
    }

    time = (((clktime * 1000) + clkticks) - ((secs * 1000) + msecs));
    printf("time in ms: %u\n", time);

    return 0;
}

void stream_consumer_future(int32 id, future_t *f)
{
    de de_to_get;

    struct tscdf *tscdf_ptr;
    tscdf_ptr = tscdf_init(time_window);

    int update_times = 0;
    int32 *qarray;
    char output[100];

    while (de_to_get.time != 0)
    {
        future_get(f, (char *)&de_to_get);
        // kprintf("[DEBUG] id: %d [GETTING] time: %d, value: %d\n", id, de_to_get.time, de_to_get.value);

        tscdf_update(tscdf_ptr, de_to_get.time, de_to_get.value);
        update_times += 1;

        if (update_times == output_time)
        {
            update_times = 0;

            qarray = tscdf_quartiles(tscdf_ptr);

            if (qarray == NULL)
            {
                kprintf("tscdf_quartiles returned NULL\n");
                continue;
            }

            sprintf(output, "s%d: %d %d %d %d %d", id, qarray[0], qarray[1], qarray[2], qarray[3], qarray[4]);
            kprintf("%s\n", output);

            freemem((char *)qarray, (6 * sizeof(int32)));
        }
    }

    // kprintf("stream_consumer_future exiting\n");
    ptsend(pcport, getpid());
}
