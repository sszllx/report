#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "client.h"
#include "defs.h"
#include "event_loop.h"
#include "report_socket.h"

typedef struct {
    event_loop_mgr_t *loop_mgr;
    pid_t client_pid;
} monitor_mgr_t;

static UNUSED int daemonize()
{
    pid_t pid, sid;
    int fd; 

    if(getppid() == 1) {
        return -1;
    }

    pid = fork();
    if (pid < 0) {
        return -1;
    }

    if (pid > 0) {
        exit(0);
    }

    sid = setsid();
    if (sid < 0) {
        return -1;
    }

    if((chdir("/")) < 0) {
        return -1;
    }

    fd = open("/dev/null",O_RDWR, 0);

    if (fd != -1) {
        dup2 (fd, STDIN_FILENO);
        dup2 (fd, STDOUT_FILENO);
        dup2 (fd, STDERR_FILENO);

        if (fd > 2) {
            close (fd);
        }
    }

    umask(027);
    return 0;
}

void sig_handle(int sig)
{

}

static void *
read_cb(int sock, void *private_data)
{
    int rlen;
    char buffer[PACKAGE_SIZE];
    report_header_t *rhdr;
    // monitor_mgr_t *mgr = (monitor_mgr_t *)private_data;

 retry:
    rlen = read(sock, buffer, PACKAGE_SIZE);
    if (rlen < 0 && errno == EINTR) {
        goto retry;
    }

    rhdr = (report_header_t *)buffer;

    switch (rhdr->code) {
    case VERSION_CHANGED:
        {
            int version;
            if (rhdr->op != REQUEST) {
                break;
            }

            memcpy(&version, rhdr->data, sizeof(int));
            printf("version updata: %d\n", version);
        }
        break;
    default:
        break;
    }

    return NULL;
}

static UNUSED void *
client_monitor(int sock, void *private_data)
{
    printf("%s %d \n", __FUNCTION__, __LINE__);
    return NULL;
}


int main(int argc, char const *argv[])
{
    pid_t pid;
    int sock[2];
    int ret;
    monitor_mgr_t *mgr;
    event_loop_mgr_t *loop_mgr;
    int server_sock;

    signal(SIGCHLD, sig_handle);

    // daemonize();

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sock);
    if (ret < 0) {
        perror("create sockepair failed\n");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        close (sock[0]);
        create_client (sock[1]);

        return 0;
    }

    close (sock[1]);

    mgr = calloc(1, sizeof(monitor_mgr_t));
    mgr->client_pid = pid;

    server_sock = connect_server();
    if (server_sock < 0) {
        perror("update connect server failed\n");
        return 1;
    }

    loop_mgr = event_loop_init ();
    mgr->loop_mgr = loop_mgr;

    event_loop_read_add (loop_mgr, sock[0], client_monitor, mgr);
    event_loop_read_add (loop_mgr, server_sock, read_cb, mgr);
    event_loop_enter (loop_mgr);

    close (sock[0]);

    return 0;
}
