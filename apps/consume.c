#include <xinu.h>
#include <stdio.h>
#include <prodcons.h>
#include <prodcons_bb.h>

void consumer(int count)
{

    int i;
    for (i = 0; i <= count; i += 1)
    {
        printf("consumed value is: %d \n", n);
    }
}

void consumer_bb(int count)
{
    // Iterate from 0 to count and for each iteration read the next available value from the global array `arr_q`
    // print consumer process name and read value as,
    // name : consumer_1, read : 8
    int i, read_number;
    char *proc_name;

    for (i = 0; i < count; i += 1){
        wait(bb_available);
        wait(bb_mutex);
        read_number = bb_array[bb_index_read];
        bb_index_read = (bb_index_read + 1) % STORAGEBUF;
        

        }
    }
}