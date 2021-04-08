#ifndef LOG_HEADER_INCLUDED
#define LOG_HEADER_INCLUDED

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "dict.h"

#define DEFAULT_LOG_SZ 1024
#define DEFAULT_LOG_FILE "log"

struct req_result {
  char *word;
  int correct;
  // other info possible
};

struct log_buffer {
  FILE *file;
  struct req_result *rrs;
  int alloc_sz;
  int curr_sz;
  int start;
  sem_t underflow;
  sem_t overflow;
  pthread_mutex_t access;
};

void log_queue(struct log_buffer *, struct req_result);
struct req_result log_dequeue(struct log_buffer *);

void init_log_buffer(struct log_buffer *, char *);
void logger(struct log_buffer *);

struct logger_args { struct log_buffer *lb; };
void *logger_thread(void *);

void r_log(FILE *, struct req_result);

struct log_buffer req_log;

#endif

