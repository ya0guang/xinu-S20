#include <xinu.h>
#include <stdio.h>
#include <prodcons.h>
#include <prodcons_bb.h>

void producer(int count)
{

    // Iterates from 0 to count, setting
    // the value of the global variable 'n'
    // each time.
    // print produced value e.g. produced : 8
    int i;

    for (i = 0; i <= count; i += 1)
    {
        n = i;
        printf("produced value is: %d \n", n);
    }
}

void producer_bb(int count)
{
    // Iterate from 0 to count and for each iteration add iteration value to the global array `arr_q`,
    // print producer process name and written value as,
    // name : producer_1, write : 8
    int i, write_number;
    char *proc_name;

    for (i = 0; i < count; i += 1){
        wait(bb_buf);
        wait(bb_mutex);
        bb_array[bb_index_write] = i;
        bb_index_write = (bb_index_write + 1) % STORAGEBUF;
        write_number = bb_number++;
        signal(bb_mutex);
        signal(bb_available);
        proc_name = proctab[getpid()].prname;
        printf("Producer: %s, write: %d", proc_name, write_number);
    }
}
