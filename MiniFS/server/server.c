#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#include "../common/utils.h"
#include "../common/protocol.h"
#include "storage.h"
#include "metadata.h"
#include "worker.h"

static volatile int running=1;
static void sig_handler(int sig) { running=0; }

int main(int argc, char** argv) {
    int port=8080;
    const char* storage_dir="../storage";
    const char* metadata_file="../storage/.metadata";
    
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i],"-p")==0&&i+1<argc) port=atoi(argv[++i]);
        else if(strcmp(argv[i],"-s")==0&&i+1<argc) storage_dir=argv[++i];
        else if(strcmp(argv[i],"-m")==0&&i+1<argc) metadata_file=argv[++i];
    }
    
    utils_set_log_level(LOG_INFO);
    utils_log_init(NULL);
    signal(SIGINT,sig_handler);
    signal(SIGTERM,sig_handler);
    
    storage_t storage;
    if(storage_init(&storage,storage_dir)!=0) {
        fprintf(stderr,"Failed to init storage\n");
        return 1;
    }
    
    metadata_t metadata;
    if(metadata_init(&metadata,metadata_file)!=0) {
        fprintf(stderr,"Failed to init metadata\n");
        return 1;
    }
    
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    int opt=1;
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(port);
    
    if(bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))<0) {
        fprintf(stderr,"Bind failed\n");
        return 1;
    }
    
    listen(server_fd,10);
    printf("MiniFS Server running on port %d\n",port);
    
    while(running) {
        struct sockaddr_in client_addr;
        socklen_t len=sizeof(client_addr);
        int client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&len);
        if(client_fd<0) continue;
        
        worker_context_t* w=malloc(sizeof(worker_context_t));
        static int worker_id=0;
        worker_init(w,client_fd,client_addr,&storage,&metadata,worker_id++);
        
        pthread_t tid;
        pthread_create(&tid,NULL,worker_thread,w);
        pthread_detach(tid);
    }
    
    metadata_save(&metadata);
    metadata_cleanup(&metadata);
    storage_cleanup(&storage);
    close(server_fd);
    utils_log_cleanup();
    
    return 0;
}
