#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "request.h"
#include "dict.h"
#include "log.h"
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

int req_dequeue(struct req_buffer *rb) {
  sem_wait(&rb->underflow);
  pthread_mutex_lock(&rb->access);

  int val = rb->fds[rb->start];
  rb->curr_sz--;
  rb->start = (rb->start + 1) % rb->alloc_sz;

  pthread_mutex_unlock(&rb->access);
  sem_post(&rb->overflow);
  return val;
}

void req_queue(struct req_buffer *rb, int fd) {
  sem_wait(&rb->overflow);
  pthread_mutex_lock(&rb->access);

  int pos = (rb->start + rb->curr_sz) % rb->alloc_sz;
  rb->fds[pos] = fd;

  rb->curr_sz++;

  pthread_mutex_unlock(&rb->access);
  sem_post(&rb->underflow);
}

void req_responder(struct req_buffer *rb) {
  int client_fd;
  while(1) {
    client_fd = req_dequeue(rb);
    // handle client
    handle_client(client_fd);
    close(client_fd);
  }
}

void handle_word(int, char *);

void handle_client(int client_fd) {
  int buff_len = 1024;
  size_t buff_sz = buff_len * sizeof(char);
  char *buff = ec_malloc(buff_sz);
  memset(buff, 0x0, buff_sz);
  int curr_sz = 0;
  int ret_val;
  int processed;
  char *buff_p = buff;
  int first_zero = 0;
  while((ret_val = read(client_fd, buff, buff_sz)) != EOF) {
    if(first_zero && ret_val == 0) break;
    else if(ret_val == 0) first_zero = 1;

    curr_sz = curr_sz + ret_val;
    processed = 0;
    for(int i = 0; i < curr_sz; i++) {
      switch(buff[i]) {
        case '\n':
        case ' ':
        case '\t':
          buff[i] = '\0';
          handle_word(client_fd, buff + processed);
          processed = i + 1;
          break;
        default:
          break;
      }
    }
    //curr_sz = buff_len - processed;
    //strncpy(buff, buff + processed, curr_sz);
    //buff_p = buff + curr_sz;
    memset(buff, 0x0, buff_sz);
  }
  char *last = strndup(buff, curr_sz * sizeof(char));
  handle_word(client_fd, last);
  free(last);
  free(buff);
}


void handle_word(int client_fd, char *word) {
  if(!*word) return;

  int word_ok = is_in_dict(&dictionary, word);

  size_t word_sz = strlen(word) * sizeof(char);
  write(client_fd, word, word_sz);

  const char *ok = "OK";
  size_t ok_sz = strlen(ok) * sizeof(char);
  const char *wrong = "WRONG";
  size_t wrong_sz = strlen(wrong) * sizeof(char);

  if(word_ok) {
    write(client_fd, ok, ok_sz);
  } else {
    write(client_fd, wrong, wrong_sz);
  }

  char *str = strdup(word);
  struct req_result rr = {str, word_ok};
  log_queue(&req_log, rr);
}



void *req_responder_thread(void *args) {
  struct req_responder_args *rr_args = (struct req_responder_args *) args;
  struct req_buffer *rb = rr_args->rb;
  req_responder(rb);

  return NULL;
}
