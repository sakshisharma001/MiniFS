#ifndef WORKER_H
#define WORKER_H
#include <pthread.h>
#include <netinet/in.h>
#include "../common/protocol.h"
#include "storage.h"
#include "metadata.h"
typedef struct {
    int client_fd;
    struct sockaddr_in client_addr;
    storage_t* storage;
    metadata_t* metadata;
    int worker_id;
    int running;
} worker_context_t;
void* worker_thread(void* arg);
int worker_init(worker_context_t* w, int fd, struct sockaddr_in addr, storage_t* s, metadata_t* m, int id);
void worker_cleanup(worker_context_t* w);
#endif
