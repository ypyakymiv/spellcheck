#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include "utils.h"
#include "dict.h"
#include "server.h"
#include "request.h"


int main(int argc, char **argv) {
  char *dict_file = DEFAULT_DICTIONARY;
  if(argc > 1) dict_file = argv[1];

  load_dict(&dictionary, dict_file);
  struct req_buffer *rb = ec_malloc(sizeof(struct req_buffer));
  int *server_fd = ec_malloc(sizeof(int));
  server_init(rb, server_fd);
  struct connection_producer_args *cp_args = ec_malloc(sizeof(struct connection_producer_args));
  cp_args->rb = rb;
  cp_args->socket_fd = *server_fd;

  pthread_t server_thread;
  pthread_create(&server_thread, NULL, connection_producer_thread, cp_args);
  pthread_join(server_thread, NULL);
}

void server_init(struct req_buffer *rb, int *accepting_fd) {
  init_req_buffer(rb);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // ec this func
  if(socket_fd == -1) {
    printf("could not open socket\n");
    exit(1);
  }

  printf("socket opened with fd: %d\n", socket_fd);

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(DEFAULT_PORT);

  int bind_return = bind(socket_fd, (struct sockaddr *) &sin, sizeof(sin)); // ec

  const int BACKLOG_SZ = 1024;
  int listen_return = listen(socket_fd, BACKLOG_SZ);

  *accepting_fd = socket_fd;
}

void connection_producer(struct req_buffer *rb, int socket_fd) {
  int client_fd;
  while((client_fd = accept(socket_fd, NULL, NULL))) {
    sem_wait(&rb->overflow);
    pthread_mutex_lock(&rb->access);
    // critical section
    queue(rb, client_fd);
    pthread_mutex_unlock(&rb->access);
    sem_post(&rb->underflow);
  }
}

void *connection_producer_thread(void *args) {
  struct connection_producer_args *cp_args = args;
  struct req_buffer *rb = cp_args->rb;
  int socket_fd = cp_args->socket_fd;

  connection_producer(rb, socket_fd);

  return NULL;
}
