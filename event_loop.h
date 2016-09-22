#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

typedef struct event_loop_mgr_s event_loop_mgr_t;
typedef struct socket_cb_s socket_cb_t;
typedef void *(*callback)(int sock, void *private_data);

event_loop_mgr_t *event_loop_init();
int event_loop_write_add(event_loop_mgr_t *mgr, int fd, callback cb);
int event_loop_read_add(event_loop_mgr_t *mgr, int fd, callback cb, void *private_data);
int event_loop_write_rm(event_loop_mgr_t *mgr, int fd);
int event_loop_read_rm(event_loop_mgr_t *mgr, int fd);
int event_loop_set_timeout(event_loop_mgr_t *mgr, int sec);
int event_loop_enter(event_loop_mgr_t *mgr);

#endif // EVENT_LOOP_H
