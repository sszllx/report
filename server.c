#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/inotify.h>

#include "defs.h"
#include "event_loop.h"
#include "list.h"
#include "notify_fs.h"
#include "protocol.h"
#include "report_socket.h"
#include "task.h"

struct server_mgr_s{
    int server_sock;
    task_mgr_t *task_mgr;
    struct list_head clients;
};

typedef struct {
    server_mgr_t *mgr;
    event_loop_mgr_t *event_mgr;
    int client_sock;
    struct list_head list;
} client_t;

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
    client_t *client = (client_t *)args;
    report_header_t *rhdr;
    char *data = NULL;
    // server_mgr_t *server_mgr = client->mgr;

    while (1) {
        if (data) {
            free (data);
            data = NULL;
        }

        rhdr = report_recv (client->client_sock, &data);
        if (rhdr == NULL) {
            printf("\033[1;32;40mreport_recv failed  %s %d \033[0m\n", __FUNCTION__, __LINE__);
            break;
        }

        process_package (rhdr, data);
    }

    list_del(&client->list);

    if (data)
        free (data);
    free (client);
    return NULL;
}

static void *accept_client (server_mgr_t *server_mgr)
{
    pthread_t tid;
    int client_sock;
    socklen_t cliaddr_len;
    struct sockaddr_in client_addr;
    pthread_attr_t attr;
    int ret;
    client_t *client;

    client_sock = accept(server_mgr->server_sock,
                        (struct sockaddr *) &client_addr,
                        &cliaddr_len);

    if (client_sock < 0) {
        perror("accept failed\n");
        return NULL;
    }

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

    client = (client_t *)calloc(1, sizeof(client_t));
    client->client_sock = client_sock;
    client->mgr = server_mgr;

    list_add_tail(&client->list, &server_mgr->clients);

    pthread_create(&tid, &attr, client_process, client);
    pthread_attr_destroy(&attr);

    return NULL;
}

void *accept_thread(void *arg)
{
    server_mgr_t *server_mgr = (server_mgr_t *)arg;

    while (1) {
        accept_client(server_mgr);
    }

    return NULL;
}

void add_task_to_server(server_mgr_t *mgr, task_t *task)
{
    add_task(mgr->task_mgr, task);
}

static int get_version(int fd)
{
    char buff[16] = { 0 };
    int len;

 retry:
    len = read(fd, buff, 16);
    if (len < 0 && errno == EINTR) {
        goto retry;
    }

    return atoi(buff);
}

static int do_task(server_mgr_t *mgr)
{
    int ret = 0;
    struct list_head *pos, *n;
    task_mgr_t *task_mgr;

    // printf("%s %d\n", __FUNCTION__, __LINE__);

    task_mgr = mgr->task_mgr;
    task_mgr_lock(task_mgr);

    list_for_each_safe(pos, n, &task_mgr->tasks) {
        task_t *task = list_entry(pos, task_t, list);

        switch (task->type) {
        case VERSION: {
            int fd;
            int version;
            char *file_name = (char *)get_data_task(task);
            char version_path[32];
            report_header_t *rhdr;

            snprintf(version_path, 32, "./version/%s", file_name);
            fd = open (version_path, O_RDONLY, 00777);
            if (fd < 0) {
                perror("version file open failed\n");
                break;
            }

            version = get_version(fd);

            rhdr = new_package0();
            init_package(rhdr, VERSION_CHANGED, REQUEST);
            set_package_data(rhdr, sizeof(int), &version);

            list_for_each(pos, &mgr->clients) {
                client_t *client = list_entry(pos, client_t, list);
                if (report_send(client->client_sock, rhdr) < 0) {
                    perror("report send failed\n");
                }
            }

            free_package(rhdr);
            close(fd);
        }
            break;
        default:
            break;
        }

        list_del(&task->list);
    }

    task_mgr_unlock(mgr->task_mgr);

    return ret;
}

int main(int argc, char const *argv[])
{
    int server_sock;
    pthread_attr_t attr;
    server_mgr_t *server_mgr;
    int ret;
    pthread_t tid;
    task_mgr_t *task_mgr;

    signal(SIGPIPE, SIG_IGN);

    task_mgr = init_task_mgr();
    if (unlikely(!task_mgr)) {
        return 1;
    }

    server_sock = create_server ();
    if (server_sock < 0) {
        goto err_free_task_mgr;
    }

    server_mgr = (server_mgr_t *)calloc(1, sizeof(server_mgr_t));
    if (!server_mgr) {
        goto err_close_server;
    }

    server_mgr->server_sock = server_sock;
    server_mgr->task_mgr = task_mgr;

    INIT_LIST_HEAD(&server_mgr->clients);

    ret = pthread_attr_init(&attr);
    if (ret != 0) {
        perror("attr init failed\n");
        goto err_close_server;
    }

    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0) {
        perror("attr setdetachstate failed\n");
        goto err_free_pthread_attr;
    }

    pthread_create(&tid, &attr, accept_thread, server_mgr);
    pthread_attr_destroy(&attr);

    if (init_fs_notify(server_mgr) < 0) {
        goto err_free_pthread_attr;
    }

    printf("enter loop\n");
    while (1) {
        if (do_task(server_mgr) < 0) {
            break;;
        }

        usleep(1000 * 100);
    }

    close(server_sock);
    free(task_mgr);
    return 0;

err_free_pthread_attr:
    pthread_attr_destroy(&attr);
err_close_server:
    close(server_sock);
err_free_task_mgr:
    free(task_mgr);
    return 1;
}

