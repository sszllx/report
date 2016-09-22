#ifndef TASK_H
#define TASK_H

#include "list.h"

typedef struct task_s task_t;
typedef struct task_mgr_s task_mgr_t;
typedef int (*task_fun)(void *private_data);

#define TASK_MAX_SIZE 4*1024

typedef enum {
    VERSION,
    TASK_TYPE_NUM,
} TASK_TYPE;

struct task_s
{
    TASK_TYPE type;

    int data_len;
    void *private_data;

    task_fun fun;
    struct list_head list;
};

struct task_mgr_s
{
    pthread_mutex_t mutex;
    struct list_head tasks;
};

task_mgr_t *init_task_mgr();
task_t *alloc_task(TASK_TYPE type, task_fun fun);
int add_task(task_mgr_t *mgr, task_t *task);
int del_task(task_mgr_t *mgr, task_t *task);
int run_task(task_t *task);
int set_data_task(task_t *task, void *data, int data_len);
void *get_data_task(task_t *task);

void task_mgr_lock(task_mgr_t *mgr);
void task_mgr_unlock(task_mgr_t *mgr);

#endif
