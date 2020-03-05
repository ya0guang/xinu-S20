#ifndef STREAM_PROC_H
#define STREAM_PROC_H

int32 pcport;

int stream_proc(int nargs, char *args[]);

void stream_consumer(int32 id, struct stream *str);

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

#endif /*STREAM_PROC_H*/