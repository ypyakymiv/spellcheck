#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "dict.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

char *choose_word(struct dict *);
int fail(double);
char *mess_up(char *);
int connect_socket();

int main(int argc, char **argv) {
  if(argc < 4) {
    printf("usage: ./test <num-words> <fail_prob> <interval>\n");
    exit(1);
  }

  int sock = connect_socket();

  srand(time(NULL));

  int num_words = atoi(argv[1]);
  double fail_prob = atof(argv[2]);
  int interval = atoi(argv[3]);

  load_dict(&dictionary, DEFAULT_DICTIONARY);

  char *word;
  int failed;
  for(int i = 0; i < num_words; i++) {
    word = choose_word(&dictionary);
    failed = fail(fail_prob);
    if(failed) {
      word = mess_up(word);
    }

    printf("sending %-20s %s\n", word, failed ? "FAILED" : "CORRECT");
    dprintf(sock, "%s ", word);
    fsync(sock);

    if(failed) free(word);
    sleep(interval);
  }

  close(sock);
}

char *choose_word(struct dict *d) {
  int index = rand() % d->sz;
  return d->words[index];
}

int fail(double f) {
  return rand() < (f * RAND_MAX);
}

char *mess_up(char *s) {
  s = strdup(s);
  s[0] = s[0] == 'z' ? 'y' : 'z';
  return s;
}

int connect_socket() {
  struct sockaddr_in server;
  int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc == -1) {
    printf("Could not create socket\n");
    exit(1);
  }

  server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);

  if(connect(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
    printf("connection error\n");
    perror(NULL);
    exit(1);
  }

  return socket_desc;
}
