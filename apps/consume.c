#include <xinu.h>
#include <prodcons.h>
#include <prodcons_bb.h>


void consumer(int count) {

    int i;
    for(i = 0; i <= count; i += 1) {
        printf("consumed value is: %d \n", n);
    }
}

void consumer_bb(int count) {
  // Iterate from 0 to count and for each iteration read the next available value from the global array `arr_q`
  // print consumer process name and read value as,
  // name : consumer_1, read : 8
}