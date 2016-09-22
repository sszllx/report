#ifndef PRO_H
#define PRO_H

typedef enum {
    ADD_MSG,
    DEL_MSG,
    VERSION_CHANGED,
    VERSION_DOWNLOAD,
    CODE_NUM,
} code_t;

typedef enum {
    REQUEST,
    REPLY,
    OP_NUM,
} op_t;

typedef struct
{
    code_t code;
    op_t op;

    int data_len;
    char data[0];
} report_header_t;

#endif // PRO_H
