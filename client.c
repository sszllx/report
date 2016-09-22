#include "defs.h"
#include "event_loop.h"
#include "report_socket.h"
#include "protocol.h"

#include <errno.h>

static UNUSED void *read_cb(int sock, void *private_data)
{
    
    return NULL;
}

int
create_client (int parent)
{
    // int sock;
    event_loop_mgr_t *mgr;
    //    report_header_t *rhdr;

    //    sock = connect_server ();
    // if (sock < 0)
    // return 1;

    /* rhdr = create_package (ADD_MSG, REQUEST, "test", 5); */
    /* if (rhdr == NULL) */
    /*     return 1; */

    /* report_send (sock, rhdr); */

    mgr = event_loop_init ();
    event_loop_read_add (mgr, parent, read_cb, NULL);
    event_loop_enter (mgr);

    //    close (sock);
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
