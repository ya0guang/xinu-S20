#include <xinu.h>
#include <stream_proc.h>
// #include "tscdf_input.h"
#include "tscdf.h"


int stream_proc(int nargs, char *args[])
{
    /* For Timing */
    ulong secs, msecs, time;
    secs = clktime;
    msecs = clkticks;

    int i;
    char *ch;
    char c;

    char consumer_name[] = "stream_consumer?";

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

    /* for streaming */
    struct stream sts[num_streams];

    if ((pcport = ptcreate(num_streams)) == SYSERR)
    {
        printf("ptcreate failed\n");
        return (-1);
    }

    // Create consumer processes and initialize streams
    pid32 c_pid;

    for (i = 0; i < num_streams; i++)
    {
        // Initialize streams
        sts[i].spaces = semcreate(work_queue_depth);
        sts[i].items = semcreate(0);
        sts[i].mutex = semcreate(1);
        sts[i].head = 0;
        sts[i].tail = 0;
        queue_length = work_queue_depth + 1;
        sts[i].queue = (de *)getmem(sizeof(de) * queue_length);

        // Create consumer process
        consumer_name[15] = (char)(48 + i);

        c_pid = create((void *)stream_consumer, 4096, 20, consumer_name, 2, i, &sts[i]);
        resume(c_pid);
        printf("stream_consumer id:%d (pid:%d) \n", i, c_pid);
    }

    // Parse input header file data and populate work queue
    char *a;
    int st, ts, v;

    for (i = 0; i < n_input; i++)
    {

        a = (char *)stream_input[i];
        st = atoi(a);
        while (*a++ != '\t')
            ;
        ts = atoi(a);
        while (*a++ != '\t')
            ;
        v = atoi(a);

        wait(sts[st].spaces);
        wait(sts[st].mutex);
        sts[st].queue[sts[st].tail].time = ts;
        sts[st].queue[sts[st].tail].value = v;
        sts[st].tail = (sts[st].tail + 1) % queue_length;

        signal(sts[st].mutex);
        signal(sts[st].items);
    }

    // Use `i` as the stream id.
    for (i = 0; i < num_streams; i++)
    {
        uint32 pm;
        pm = ptrecv(pcport);
        printf("process %d exited\n", pm);
    }

    ptdelete(pcport, 0);

    /* timer */
    time = (((clktime * 1000) + clkticks) - ((secs * 1000) + msecs));
    printf("time in ms: %u\n", time);

    return (0);
}

// Following is the interface for the consumer process. Here id is the stream id.

void stream_consumer(int32 id, struct stream *str)
{
    int32 ts, v;
    ts = 1;

    struct tscdf *tscdf_ptr;
    tscdf_ptr = tscdf_init(time_window);

    int update_times = 0;
    int32 *  qarray;
    char output[100];

    while (ts != 0)
    {

        wait(str->items);
        wait(str->mutex);
        ts = str->queue[str->head].time;
        v = str->queue[str->head].value;
        str->head = (str->head + 1) % queue_length;
        signal(str->mutex);
        signal(str->spaces);

        tscdf_update(tscdf_ptr, ts, v);
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

    printf("stream_consumer exiting\n");
    ptsend(pcport, getpid());
}
