#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "request.h"
#include "dict.h"
#include "utils.h"

void init_req_buffer(struct req_buffer *rb) {
  rb->fds = ec_malloc(sizeof(int) * DEFAULT_REQ_SZ);
  rb->alloc_sz = DEFAULT_REQ_SZ;
  rb->curr_sz = 0;
  rb->start = 0;
  sem_init(&rb->underflow, 0x0, 0);
  sem_init(&rb->overflow, 0x0, rb->alloc_sz);
  pthread_mutex_init(&rb->access, NULL);
}

int dequeue(struct req_buffer *rb) {
  if(rb->curr_sz <= 0) return -1;

  int val = rb->fds[rb->start];
  rb->curr_sz--;
  rb->start = (rb->start + 1) % rb->alloc_sz;
  return val;
}

void queue(struct req_buffer *rb, int fd) {
  if(rb->curr_sz >= rb->alloc_sz) return;

  int pos = (rb->start + rb->curr_sz) % rb->alloc_sz;
  rb->fds[pos] = fd;

  rb->curr_sz++;
}

void req_responder(struct req_buffer *rb, struct dict *d) {
  int client_fd;
  while(1) {
    sem_wait(&rb->underflow);
    pthread_mutex_lock(&rb->access);
    // critical section
    client_fd = dequeue(rb);
    pthread_mutex_unlock(&rb->access);
    sem_post(&rb->overflow);
    // handle client
    handle_client(client_fd);
  }
}

void handle_word(int, char *);

void handle_client(int client_fd) {
  int buff_len = 1024;
  size_t buff_sz = buff_len * sizeof(char);
  char *buff = ec_malloc(buff_sz);
  int curr_sz = 0;
  int ret_val;
  int processed;
  char *buff_p = buff;
  while((ret_val = read(client_fd, buff_p, buff_sz)) != EOF) {
    curr_sz = curr_sz + ret_val;
    processed = 0;
    for(int i = 0; i < curr_sz; i++) {
      switch(buff[i]) {
        case ' ':
        case '\n':
        case '\t':
          buff[i] = '\0';
          handle_word(client_fd, buff + processed);
          processed = i + 1;
          break;
        default:
          break;
      }
    }
    curr_sz = buff_len - processed;
    strncpy(buff, buff + processed, curr_sz);
    buff_p = buff + curr_sz;
  }
  // flash
  char *last = strndup(buff, curr_sz * sizeof(char));
  handle_word(client_fd, last);
  free(last);
  free(buff);
}


void handle_word(int client_fd, char *word) {
  if(!*word) return;

  printf("handling word: %s\n", word);

  int word_ok = is_in_dict(&dictionary, word);
  // respond 
}
