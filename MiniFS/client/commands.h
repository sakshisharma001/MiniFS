#ifndef COMMANDS_H
#define COMMANDS_H
#include "../common/protocol.h"
typedef struct {
    int server_fd;
    char server_addr[256];
    int server_port;
    int connected;
} client_context_t;
int client_init(client_context_t* c, const char* addr, int port);
int client_connect(client_context_t* c);
void client_disconnect(client_context_t* c);
int cmd_upload(client_context_t* c, const char* local, const char* remote);
int cmd_download(client_context_t* c, const char* remote, const char* local);
int cmd_delete(client_context_t* c, const char* filename);
int cmd_list(client_context_t* c);
int cmd_info(client_context_t* c, const char* filename);
int cmd_exit(client_context_t* c);
#endif
