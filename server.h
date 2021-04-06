#ifndef SERVER_HEADER_INCLUDED
#define SERVER_HEADER_INCLUDED

#include "dict.h"
#include "request.h"

#define DEFAULT_PORT 8080

void server_init(struct req_buffer *, int *);
void connection_producer(struct req_buffer *, int socket_fd);
struct connection_producer_args { struct req_buffer *rb; int socket_fd; };
void *connection_producer_thread(void *);
#endif
