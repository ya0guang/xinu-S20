#ifndef STREAM_PROC_H
#define STREAM_PROC_H

int32 pcport;

int queue_length;
int time_window = 10;
int output_time = 20;

typedef struct data_element {
  int32 time;
  int32 value;
} de;

struct stream {
  sid32 spaces;
  sid32 items;
  sid32 mutex;
  int32 head;
  int32 tail;
  struct data_element *queue;
};

//APIs in stream_proc.c
int stream_proc(int nargs, char *args[]);
void stream_consumer(int32 id, struct stream *str);

//FUTURE_QUEUE APIs in stream_proc_futures.c
int stream_proc_futures(int nargs, char *args[]);
void stream_consumer_future(int32 id, future_t *f);



#endif /*STREAM_PROC_H*/