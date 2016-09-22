#ifndef SERVER_H
#define SERVER_H

#include "task.h"

typedef struct server_mgr_s server_mgr_t;

void add_task_to_server(server_mgr_t *mgr, task_t *task);

#endif // SERVER_H
