#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "log.h"
#include "dict.h"
#include "utils.h"

void init_log_buffer(struct log_buffer *lb, char *file_name) {
  lb->file = ec_fopen(file_name, "a");
  lb->rrs = ec_malloc(sizeof(struct req_result) * DEFAULT_LOG_SZ);
  lb->alloc_sz = DEFAULT_LOG_SZ;
  lb->curr_sz = 0;
  lb->start = 0;
  sem_init(&lb->underflow, 0x0, 0);
  sem_init(&lb->overflow, 0x0, lb->alloc_sz);
  pthread_mutex_init(&lb->access, NULL);
}

struct req_result log_dequeue(struct log_buffer *lb) {
  sem_wait(&lb->underflow);
  pthread_mutex_lock(&lb->access);

  struct req_result val = lb->rrs[lb->start];
  lb->curr_sz--;
  lb->start = (lb->start + 1) % lb->alloc_sz;

  pthread_mutex_unlock(&lb->access);
  sem_post(&lb->overflow);
  return val;
}

void log_queue(struct log_buffer *lb, struct req_result rr) {
  sem_wait(&lb->overflow);
  pthread_mutex_lock(&lb->access);

  int pos = (lb->start + lb->curr_sz) % lb->alloc_sz;
  lb->rrs[pos] = rr;

  lb->curr_sz++;

  pthread_mutex_unlock(&lb->access);
  sem_post(&lb->underflow);
}

void logger(struct log_buffer *lb) {
  struct req_result rr;
  while(1) {
    rr = log_dequeue(lb);
    r_log(lb->file, rr);
    free(rr.word);
  }
}
void *logger_thread(void *args) {
  struct logger_args *rr_args = (struct logger_args *) args;
  struct log_buffer *lb = rr_args->lb;
  logger(lb);

  return NULL;
}

void r_log(FILE *f, struct req_result rr) {
  printf("%s:%s\n", rr.word, rr.correct ? "OK" : "WRONG");
  fprintf(f, "%s:%s\n", rr.word, rr.correct ? "OK" : "WRONG");
  fflush(f);
}
