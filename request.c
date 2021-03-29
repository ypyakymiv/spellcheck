#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "request.h"
#include "utils.h"

void init_req_buffer(struct req_buffer *rb) {
  rb->fds = ec_malloc(sizeof(int) * DEFAULT_REQ_SZ);
  rb->alloc_sz = DEFAULT_REQ_SZ;
  sem_init(&rb->reqs, 0x0, rb->alloc_sz); 
}
