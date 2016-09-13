#include "event_loop.h"
#include "report_socket.h"
#include "protocol.h"

#include <errno.h>

static report_header_t *
create_package (code_t code, op_t op, void *data, int len)
{
    report_header_t *package;

    package = malloc(sizeof(report_header_t) + len);
    if (!package)
        return NULL;

    package->code = code;
    package->op = op;
    package->data_len = len;
    memcpy(package->data, data, len);

    return package;
}

static void *read_cb(event_loop_mgr_t *mgr, int sock)
{

    return NULL;
}

int
create_client (int parent)
{
    int sock;
    event_loop_mgr_t *mgr;
    report_header_t *rhdr;

    sock = connect_server ();
    if (sock < 0)
        return 1;

    rhdr = create_package (ADD_MSG, REQUEST, "test", 5);
    if (rhdr == NULL)
        return 1;

    report_send (sock, rhdr);
    mgr = event_loop_init ();
    event_loop_read_add (mgr, parent, read_cb);
    event_loop_enter (mgr);

    close (sock);
    return 0;
}

// int
// main (int argc, char const *argv[])
// {
//     int sock;
//     report_header_t *rhdr;

//     sock = connect_server ();
//     if (sock < 0)
//         return 1;

//     rhdr = create_package (ADD_MSG, REQUEST, "test", 5);
//     if (rhdr == NULL)
//         return 1;

//     report_send (sock, rhdr);

//     getchar ();
//     close (sock);

//     return 0;
// }