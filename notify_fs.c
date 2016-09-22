#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "defs.h"
#include "notify_fs.h"
#include "task.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

static UNUSED void handle_events(int fd, int wd)
{
    char buf[4096]
        __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    int len;
    // int version;
    char *ptr;

retry:
    len = read(fd, buf, sizeof buf);
    if (len == -1 && errno == EINTR) {
        goto retry;
    } else {
        perror("read");
        return;
    }

    for (ptr = buf; ptr < buf + len;
         ptr += sizeof(struct inotify_event) + event->len) {
        event = (const struct inotify_event *) ptr;

        if (wd == event->wd) {
            printf("file changed: %s\n", event->name);
        }
    }


}

static void *start_notify_fs(void *arg)
{
    server_mgr_t *mgr = (server_mgr_t *)arg;
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    while (1) {
        fd = inotify_init();

        if ( fd < 0 ) {
            perror ("inotify_init");
            return NULL;
        }

        wd = inotify_add_watch (fd, "./version", IN_MODIFY);

        i = 0;
        length = read (fd, buffer, EVENT_BUF_LEN);
        if (length < 0) {
            perror ("read");
            inotify_rm_watch (fd, wd);
            close (fd);
            return NULL;
        }

        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_MODIFY) {
                    if (!(event->mask & IN_ISDIR)) {
                        printf( "New file %s modify.\n", event->name );
                        task_t *task;
                        task = alloc_task(VERSION, NULL);
                        set_data_task(task, (void *)event->name, strlen(event->name) + 1);
                        add_task_to_server(mgr, task);
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }

        inotify_rm_watch( fd, wd );
        close( fd );
    }

    return NULL;
}

int init_fs_notify(server_mgr_t *server_mgr)
{
    int ret;
    pthread_t tid;
    pthread_attr_t attr;

    ret = pthread_attr_init(&attr);
    if (ret != 0) {
        perror("attr init failed\n");
        return -1;
    }

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0) {
        perror("attr setdetachstate failed\n");
        return -1;
    }

    pthread_create(&tid, &attr, start_notify_fs, server_mgr);
    pthread_attr_destroy(&attr);

    return 0;
}

