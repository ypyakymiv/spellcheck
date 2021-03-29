#ifndef REQUEST_HEADER_INCLUDED
#define REQUEST_HEADER_INCLUDED

#include <semaphore.h>

enum status {
  WRONG, OK
};

#define DEFAULT_REQ_SZ 1024

struct req_buffer {
  int *fds;
  int alloc_sz;
  sem_t reqs;
};

void init_req_buffer(struct req_buffer *);

#endif
