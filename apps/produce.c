#include <xinu.h>
#include <prodcons.h>

void producer(int count) {
    
    // Iterates from 0 to count, setting
    // the value of the global variable 'n'
    // each time.
    // print produced value e.g. produced : 8
    int i;

    for(i = 0; i <= count; i += 1){
        n = i;
        printf("produced value is: %d \n", n);
    }
}


