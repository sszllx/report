#ifndef R_SOCKET_H
#define R_SOCKET_H

#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <event.h>
#include <fcntl.h>

#include "protocol.h"

int connect_server();
int create_server();

int report_send(int sock, report_header_t *header);
report_header_t *report_recv(int sock, char **out_data);

#endif // R_SOCKET_H
