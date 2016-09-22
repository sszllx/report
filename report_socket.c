#include "defs.h"
#include "report_socket.h"

#include <errno.h>

static const char *external_server_ip_prefix = "172.16.22";
// static const char *inner_server_ip_prefix = "192.168.11";
static const int server_port = 12344;

/*
  ret: 0 no server found
       -1 error
       1 found server
*/
static int _loop_server()
{
    int loop_index = 1;
    int ret;
    int found_server = 0;
    struct sockaddr_in remote_addr;
    int sock = -1;
    char ip[16];

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("open sock failed\n");
        return -1;
    }

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;

    bzero(ip, 16);
    snprintf(ip, 16, "%s.131", external_server_ip_prefix);
    printf("=== %s\n", ip);
    remote_addr.sin_addr.s_addr = inet_addr(ip);
    remote_addr.sin_port = htons(server_port);
    ret = connect(sock, (struct sockaddr*) &remote_addr,
                  sizeof(struct sockaddr));
    printf("\033[1;32;40mret:%d error:%s %s %d \033[0m\n",
           ret, strerror(errno), __FUNCTION__, __LINE__);
    if (ret == 0) {
        found_server = 1;
        printf("connect success!\n");
        goto out;
    }

    for (; loop_index < 255; loop_index++) {
        bzero(ip, 16);
        snprintf(ip, 16, "%s.%d", external_server_ip_prefix, loop_index);
        printf("ip:%s\n", ip);
        remote_addr.sin_addr.s_addr = inet_addr(ip);
        remote_addr.sin_port = htons(server_port);
        ret = connect(sock, (struct sockaddr*) &remote_addr,
                      sizeof(struct sockaddr));
        if (ret == 0) {
            found_server = 1;
            printf("connect success!\n");
            goto out;
        }
    }

out:
    if (found_server)
        return sock;
    else
        return -1;
}

int connect_server()
{
    int sock;

    sock = _loop_server();
    if (sock < 0) {
        printf("there is no server available\n");
        return -1;
    }

    return sock;
}

int create_server()
{
//     int flags;
    int sock;
    struct sockaddr_in server_addr;
    int optval = 1;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        perror("create server failed");
        return -1;
    }

    // flags = fcntl (sock, F_GETFL, 0);
    // fcntl (sock, F_SETFL, flags | O_NONBLOCK);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
               &optval, (socklen_t)(sizeof(int)));


    bzero (&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons (server_port);
    server_addr.sin_addr.s_addr = htonl (INADDR_ANY);

    int br =
        bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (br == -1) {
        perror("bind failed");
        return -1;
    }

    if ((listen(sock, 20)) == -1) {
        perror("listen failed");
        return -1;
    }

    return sock;
}

int
report_send(int sock, report_header_t *header)
{
    int slen;

    slen = send(sock, header,
                sizeof(report_header_t) + header->data_len, 0);
    return slen;
}

report_header_t *report_recv(int sock, char **out_data)
{
    int offset = 0;
    report_header_t *rhdr;
    char *data;
    int rlen;

    rhdr = malloc(sizeof(report_header_t));
    if (!rhdr) {
        return NULL;
    }

read_again:
    rlen = read (sock, rhdr, sizeof(report_header_t));
    if (rlen < 0 && errno == EINTR)
        goto read_again;

    if (rlen == 0) {
        close(sock);
        goto err;
    }

    data = malloc(rhdr->data_len);
    if (!data) {
        goto err;
    }
    *out_data = data;

    while (offset < rhdr->data_len) {
        rlen = read (sock,
                     data + offset,
                     rhdr->data_len - offset);
        offset += rlen;
    }

    return rhdr;

err:
    free(rhdr);
    return NULL;
}

report_header_t *new_package0()
{
    report_header_t *rhdr;

    rhdr = calloc(1, PACKAGE_SIZE);

    return rhdr;
}

void free_package(report_header_t *h)
{
    if (unlikely(!h)) {
        return;
    }

    free(h);
}

void init_package(report_header_t *h, code_t code, op_t op)
{
    h->code = code;
    h->op = op;
}

int set_package_data(report_header_t *h, int data_len, void *data)
{
    if (data_len > PACKAGE_SIZE - sizeof(report_header_t)) {
        return -1;
    }

    h->data_len = data_len;

    memcpy(h->data, data, data_len);
    return 0;
}

