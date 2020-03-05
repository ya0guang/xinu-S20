#include <xinu.h>
#include <stream_proc.h>
#include "tscdf_input.h"

int stream_proc(int nargs, char *args[])
{
    /* For Timing */
    ulong secs, msecs, time;
    secs = clktime;
    msecs = clkticks;

    int i;
    int char* ch;
    int char c;

    /* for streaming */
    struct stream * sts;

    // Parse arguments

    /* default arguments */
    int num_streams = 10;
    int work_queue_depth = 15;
    int time_window = 10;
    int output_time = 20;

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
        printf("ptcreate failed\n");
        return (-1);
    }

    // Create streams

    sts = getmem(sizeof(struct stream) * num_streams);


    // Create consumer processes and initialize streams
    // Use `i` as the stream id.
    for (i = 0; i < num_streams; i++)
    {
        uint32 pm;
        pm = ptrecv(pcport);
        printf("process %d exited\n", pm);
    }

    ptdelete(pcport, 0);

    // Parse input header file data and populate work queue

    /* timer */
    time = (((clktime * 1000) + clkticks) - ((secs * 1000) + msecs));
    printf("time in ms: %u\n", time);

    return (0);
}

// Following is the interface for the consumer process. Here id is the stream id.

void stream_consumer(int32 id, struct stream *str)
{
    char * a;
    int st, ts, v;

    a = (char *)stream_input[i];
    st = atoi(a);
    while (*a++ != '\t')
        ;
    ts = atoi(a);
    while (*a++ != '\t')
        ;
    v = atoi(a);

    ptsend(pcport, getpid());
}
