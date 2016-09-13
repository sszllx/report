#include "event_loop.h"
#include "protocol.h"
#include "report_socket.h"

#include <errno.h>
#include <pthread.h>

typedef struct {
    event_loop_mgr_t *mgr;
    int client_sock;
} client_thread_arg_t;

static int process_package(report_header_t *header, char *data)
{
    switch (header->code) {
    case ADD_MSG:
        
        break;
    case DEL_MSG:
        break;
    default:
        break;
    }

    return 0;
}

static void *client_process (void *args)
{
    client_thread_arg_t *client_arg = (client_thread_arg_t *)args;
    report_header_t *rhdr;
    // int rlen = 0;
    char *data = NULL;

    while (1) {
        if (data) {
            free (data);
            data = NULL;
        }
        
        rhdr = report_recv (client_arg->client_sock, &data);
        if (rhdr == NULL) {
            printf("\033[1;32;40mreport_recv failed  %s %d \033[0m\n", __FUNCTION__, __LINE__);
            break;
        }

        process_package (rhdr, data);
    }

    if (data)
        free (data);
    free (client_arg);
    return NULL;
}

static void *accept_cb (event_loop_mgr_t *mgr, int sock)
{
    pthread_t tid;
    int client_sock;
    socklen_t cliaddr_len;
    struct sockaddr_in client_addr;
    pthread_attr_t attr;
    int ret;
    client_thread_arg_t *client_arg;

    printf("\033[1;32;40m---------------------  %s %d \033[0m\n", __FUNCTION__, __LINE__);
    client_sock = accept(sock,
                         (struct sockaddr *) &client_addr,
                         &cliaddr_len);
    printf("\033[1;32;40mserver accept  %s %d \033[0m\n", __FUNCTION__, __LINE__);

    ret = pthread_attr_init(&attr);
    if (ret != 0) {
        perror("attr init failed\n");
        return NULL;
    }

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0) {
        perror("attr setdetachstate failed\n");
        return NULL;
    }

    client_arg = malloc(sizeof(client_thread_arg_t));
    client_arg->mgr = mgr;
    client_arg->client_sock = client_sock;

    pthread_create(&tid, &attr, client_process, client_arg);
    pthread_attr_destroy(&attr);
    // event_loop_read_add(mgr, client_sock, client_process);

    return NULL;
}

int main(int argc, char const *argv[])
{
    // event_loop_mgr_t *loop_mgr;
    int ret;

    ret = create_server ();
    if (ret < 0) {
        return -1;
    }

    while (1) {
        accept_cb(NULL, ret);
    }

#if 0
    loop_mgr = event_loop_init ();
    printf("%s %d server sock:%d\n", __FUNCTION__, __LINE__, ret);
    event_loop_read_add (loop_mgr, ret, accept_cb);
    event_loop_enter (loop_mgr);
#endif

    return 0;
}