#ifndef REQUEST_HEADER_INCLUDED
#define REQUEST_HEADER_INCLUDED

#include <pthread.h>
#include <semaphore.h>
#include "dict.h"

enum status {
  WRONG, OK
};

#define DEFAULT_REQ_SZ 1024

struct req_buffer {
  int *fds;
  int alloc_sz;
  int curr_sz;
  int start;
  sem_t underflow;
  sem_t overflow;
  pthread_mutex_t access;
};

void queue(struct req_buffer *, int);
int dequeue(struct req_buffer *);

void init_req_buffer(struct req_buffer *);
void req_responder(struct req_buffer *, struct dict *);
void handle_client(int client_fd);

#endif

