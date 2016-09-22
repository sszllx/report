#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "task.h"

task_mgr_t *init_task_mgr()
{
    pthread_mutexattr_t mattr;
    task_mgr_t *mgr;
    int ret;

    mgr = (task_mgr_t *)calloc(1, sizeof(task_mgr_t));
    if (unlikely(!mgr)) {
        goto err;
    }

    INIT_LIST_HEAD(&mgr->tasks);

    ret = pthread_mutexattr_init(&mattr);
    if (ret < 0) {
        goto err_free_mgr;
    }

    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mgr->mutex, &mattr);

    ret = pthread_mutexattr_destroy(&mattr);
    if (ret < 0) {
        goto err_free_mgr;
    }

    return mgr;

 err_free_mgr:
    free(mgr);
 err:
    return NULL;
}

task_t *alloc_task(TASK_TYPE type, task_fun fun)
{
    task_t *task;

    task = (task_t *)calloc(1, sizeof(task_t));

    return task;
}

int add_task(task_mgr_t *mgr, task_t *task)
{
    pthread_mutex_lock(&mgr->mutex);
    list_add_tail(&task->list, &mgr->tasks);
    pthread_mutex_unlock(&mgr->mutex);
    return 0;
}

int del_task(task_mgr_t *mgr, task_t *task)
{
    pthread_mutex_lock(&mgr->mutex);
    list_del(&task->list);
    pthread_mutex_unlock(&mgr->mutex);
    return 0;
}

int run_task(task_t *task)
{
    if (unlikely(!task) || unlikely(!task->fun)) {
        return -1;
    }

    task->fun(task->private_data);

    return 0;
}

int
set_data_task(task_t *task, void *data, int data_len)
{
    if (unlikely(!task)) {
        return -1;
    }

    if (data_len > TASK_MAX_SIZE) {
        printf("private data is too long\n");
        return -1;
    }

    task->private_data = malloc(data_len);
    memcpy(task->private_data, data, data_len);

    task->data_len = data_len;

    return 0;
}

void *get_data_task(task_t *task)
{
    if (unlikely(!task)) {
        return NULL;
    }

    return task->private_data;
}

void task_mgr_lock(task_mgr_t *mgr)
{
    pthread_mutex_lock(&mgr->mutex);
}

void task_mgr_unlock(task_mgr_t *mgr)
{
    pthread_mutex_unlock(&mgr->mutex);
}
