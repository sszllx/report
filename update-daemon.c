#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "client.h"
#include "event_loop.h"

static int daemonize()
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
read_cb(event_loop_mgr_t *mgr, int sock)
{

    return NULL;
}

int main(int argc, char const *argv[])
{
    pid_t pid;
    int sock[2];
    int ret;
    event_loop_mgr_t *mgr;

    signal(SIGCHLD, sig_handle);

    daemonize();

    ret = socketpair(AF_LOCAL, 0, SOCK_STREAM, sock);
    if (ret < 0) {
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

    mgr = event_loop_init ();
    event_loop_read_add (mgr, sock[0], read_cb);
    event_loop_enter (mgr);

    close (sock[0]);

    return 0;
}