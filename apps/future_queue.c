#include <xinu.h>
#include <future_test.h>

void futureq_test1 (int nargs, char *args[]) {
    int three = 3, four = 4, five = 5, six = 6;
    future *f_queue;
    f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

    resume(create(future_cons, 1024, 20, "fcons6", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons7", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons8", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons9", 1, f_queue));
    resume(create(future_prod, 1024, 20, "fprod3", 2, f_queue, (char *)&three));
    resume(create(future_prod, 1024, 20, "fprod4", 2, f_queue, (char *)&four));
    resume(create(future_prod, 1024, 20, "fprod5", 2, f_queue, (char *)&five));
    resume(create(future_prod, 1024, 20, "fprod6", 2, f_queue, (char *)&six));
    sleep(1);
}

void futureq_test2 (int nargs, char *args[]) {
    int seven = 7, eight = 8, nine=9, ten = 10, eleven = 11;
    future *f_queue;
    f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

    resume(create(future_prod, 1024, 20, "fprod10", 2, f_queue, (char *)&seven));
    resume(create(future_prod, 1024, 20, "fprod11", 2, f_queue, (char *)&eight));
    resume(create(future_prod, 1024, 20, "fprod12", 2, f_queue, (char *)&nine));
    resume(create(future_prod, 1024, 20, "fprod13", 2, f_queue, (char *)&ten));
    resume(create(future_prod, 1024, 20, "fprod13", 2, f_queue, (char *)&eleven));

    resume(create(future_cons, 1024, 20, "fcons14", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons15", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons16", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons17", 1, f_queue));
    resume(create(future_cons, 1024, 20, "fcons18", 1, f_queue));
    sleep(1);
}

void futureq_test3 (int nargs, char *args[]) {
    int three = 3, four = 4, five = 5, six = 6;
    future *f_queue;
    f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

    resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
    resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue, (char*) &three) );
    resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue, (char*) &four) );
    resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue, (char*) &five) );
    resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue, (char*) &six) );
    resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons8", 1, f_queue) );
    resume( create(future_cons, 1024, 20, "fcons9", 1, f_queue) );
    sleep(1);
}