// declare globally shared array
extern int bb_array[];

// declare globally shared semaphores
extern int bb_semaphore;

// declare globally shared read and write indices
extern int bb_index_write;
extern int bb_index_read;

// function prototypes
void consumer_bb(int count);
void producer_bb(int count);