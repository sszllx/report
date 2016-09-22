#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "defs.h"
#include "event_loop.h"
#include "list.h"

#define SELECT_TIMEOUT 5 // second

struct socket_cb_s {
    int sock;
    callback cb;
    void *private_data;

    struct list_head list;
};

struct event_loop_mgr_s
{
    int maxfd;
    struct timeval tv;

    fd_set rfds;
    fd_set wfds;
    fd_set efds;

    struct list_head sock_cb_list;
};

event_loop_mgr_t *event_loop_init()
{
    event_loop_mgr_t *mgr;

    mgr = calloc (1, sizeof(event_loop_mgr_t));
    if (unlikely (!mgr)) {
        return NULL;
    }

    FD_ZERO (&mgr->rfds);
    FD_ZERO (&mgr->wfds);
    FD_ZERO (&mgr->efds);

    mgr->tv.tv_sec = SELECT_TIMEOUT;
    mgr->tv.tv_usec = 0;

    INIT_LIST_HEAD (&mgr->sock_cb_list);

    return mgr;
}

int
event_loop_write_add(event_loop_mgr_t *mgr,
                     int fd,
                     callback cb)
{
    FD_SET(fd, &mgr->wfds);
    mgr->maxfd = mgr->maxfd > fd ? mgr->maxfd : (fd + 1);

    return 0;
}

int
event_loop_read_add(event_loop_mgr_t *mgr,
                    int fd,
                    callback cb,
                    void *private_data)
{
    FD_SET(fd, &mgr->rfds);
    mgr->maxfd = mgr->maxfd > fd ? mgr->maxfd : (fd + 1);

    socket_cb_t *scb = calloc(1, sizeof(socket_cb_t));
    scb->sock = fd;
    scb->cb = cb;
    scb->private_data = private_data;
    list_add_tail(&scb->list, &mgr->sock_cb_list);
    return 0;
}

int event_loop_write_rm(event_loop_mgr_t *mgr, int fd)
{
    FD_CLR(fd, &mgr->wfds);
    return 0;
}

int event_loop_read_rm(event_loop_mgr_t *mgr, int fd)
{
    FD_CLR(fd, &mgr->rfds);
    return 0;
}

int event_loop_enter(event_loop_mgr_t *mgr)
{
    int ret;
    int sec;
    int usec;
    struct list_head *pos;

    struct timeval tv = mgr->tv;
    sec = tv.tv_sec;
    usec = tv.tv_usec;

    while (1) {
        tv.tv_sec = sec;
        tv.tv_usec = usec;

        printf("%s %d\n", __FUNCTION__, __LINE__);
        ret = select (mgr->maxfd, &mgr->rfds, &mgr->wfds, &mgr->efds, &tv);
        printf("%s %d ret:%d\n", __FUNCTION__, __LINE__, ret);
        if (ret == -1) {
            if (errno == EINTR)
                continue;
            perror("select()");
            break;
        } else if (ret) {
            printf("%s %d\n", __FUNCTION__, __LINE__);
            // TODO: omit efds and wfds
            list_for_each (pos, &mgr->sock_cb_list) {
                socket_cb_t *scb = list_entry (pos, socket_cb_t, list);
                printf("%s %d scb:%d\n", __FUNCTION__, __LINE__, scb->sock);
                if (FD_ISSET (scb->sock, &mgr->rfds)) {
                    scb->cb(scb->sock, scb->private_data);
                    break;
                } if (FD_ISSET (scb->sock, &mgr->wfds)) {
                    printf("%s %d\n", __FUNCTION__, __LINE__);
                } if (FD_ISSET (scb->sock, &mgr->efds)) {
                    printf("%s %d\n", __FUNCTION__, __LINE__);
                }
            }
        }
    }

    return 0;
}

int event_loop_set_timeout(event_loop_mgr_t *mgr, int sec)
{
    return 0;
}
