
// for future fib implementation
static int zero = 0, one = 1;

future_t **fibfut;
int ffib(int n);


// for future producer and consumer implementation
uint future_prod(future_t *fut, char *value);
uint future_cons(future_t *fut);
