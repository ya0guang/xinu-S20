
int future_prodcons_bb(int nargs, char *args[]);
int ffib_bb(int nargs, char *args[]);


// for future fib implementation
static int zero = 0, one = 1, two = 2;
static int anumber = 998;

future_t **fibfut;
int ffib(int n);


// for future producer and consumer implementation
uint future_prod(future_t *fut, char *value);
uint future_cons(future_t *fut);

// test for FUTURE_QUEUE
void futureq_test1 (int nargs, char *args[]);
void futureq_test2 (int nargs, char *args[]);
void futureq_test3 (int nargs, char *args[]);