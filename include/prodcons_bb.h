#ifndef
#define STORAGEBUF 200
#endif

// declare globally shared array
extern int bb_array[];

// declare globally shared semaphores
extern sid32 bb_mutex, bb_available, bb_buf;

// declare globally shared read and write indices
extern int bb_index_write;
extern int bb_index_read;

// declare globally shared producer counter
extern int bb_number;

// function prototypes
void consumer_bb(int count);
void producer_bb(int count);