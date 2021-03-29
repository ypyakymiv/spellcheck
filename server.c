#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include "utils.h"
#include "dict.h"
#include "server.h"
#include "request.h"

int main(int argc, char **argv) {
  char *dict_file = DEFAULT_DICTIONARY;
  if(argc > 1) dict_file = argv[1];

  struct dict *d = malloc(sizeof(struct dict));
  load_dict(d, dict_file);

  server_init(d);

}

void server_init(struct dict *d) {
  struct req_buffer *rb = ec_malloc(sizeof(struct req_buffer));
  init_req_buffer(rb);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // ec this func
  if(socket_fd == -1) {
    printf("could not open socket\n");
    exit(1);
  }

  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(DEFAULT_PORT);

  if(!bind(socket_fd, (struct sockaddr *) &sin, sizeof(sin))) { // ec
   
  }

  const int BACKLOG_SZ = 1024;
  if(!listen(socket_fd, BACKLOG_SZ)) {
  }  


  int client_fd;
  while(fd = accept(socket_fd, NULL, NULL)) {
    char c;
  }
 
  // listen on specified port
}
