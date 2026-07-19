#include "worker.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

static uint64_t htonll(uint64_t v) { const int n=1; return *(char*)&n==1?((uint64_t)htonl(v&0xFFFFFFFF)<<32)|htonl(v>>32):v; }
static uint64_t ntohll(uint64_t v) { const int n=1; return *(char*)&n==1?((uint64_t)ntohl(v&0xFFFFFFFF)<<32)|ntohl(v>>32):v; }

int worker_init(worker_context_t* w, int fd, struct sockaddr_in addr, storage_t* s, metadata_t* m, int id) {
    if(!w||fd<0||!s||!m) return -1;
    w->client_fd=fd;
    w->client_addr=addr;
    w->storage=s;
    w->metadata=m;
    w->worker_id=id;
    w->running=1;
    utils_set_socket_timeout(fd,300);
    return 0;
}

void worker_cleanup(worker_context_t* w) {
    if(w) {
        if(w->client_fd>=0) close(w->client_fd);
        w->running=0;
    }
}

static int recv_request(int fd, request_packet_t* r) {
    uint8_t h[16];
    if(utils_recv_exact(fd,h,16)!=0) return -1;
    uint32_t c; memcpy(&c,h,4); r->command_type=ntohl(c);
    uint32_t fl; memcpy(&fl,h+4,4); r->filename_length=ntohl(fl);
    uint64_t fs; memcpy(&fs,h+8,8); r->file_size=ntohll(fs);
    if(r->filename_length>0) utils_recv_exact(fd,(uint8_t*)r->filename,r->filename_length);
    if(r->file_size>0&&r->command_type==CMD_UPLOAD) {
        r->payload=malloc(r->file_size);
        utils_recv_exact(fd,r->payload,r->file_size);
    }
    return 0;
}

static int send_response(int fd, response_packet_t* r) {
    size_t sz=protocol_response_size(r);
    uint8_t* b=malloc(sz);
    protocol_serialize_response(r,b,sz);
    utils_send_all(fd,b,sz);
    free(b);
    return 0;
}

void* worker_thread(void* arg) {
    worker_context_t* w=(worker_context_t*)arg;
    while(w->running) {
        request_packet_t req;
        memset(&req,0,sizeof(req));
        if(recv_request(w->client_fd,&req)!=0) break;
        response_packet_t resp;
        memset(&resp,0,sizeof(resp));
        status_code_t st=STATUS_SUCCESS;
        switch(req.command_type) {
            case CMD_UPLOAD:
                if(storage_file_exists(w->storage,req.filename)) st=STATUS_FILE_EXISTS;
                else {
                    uint32_t cs=utils_calculate_checksum(req.payload,req.file_size);
                    if(storage_store_file(w->storage,req.filename,req.payload,req.file_size)==0) metadata_add(w->metadata,req.filename,req.file_size,cs);
                    else st=STATUS_SERVER_ERROR;
                }
                protocol_init_response(&resp,st,req.filename,req.file_size,NULL,0);
                break;
            case CMD_DOWNLOAD:
                if(!storage_file_exists(w->storage,req.filename)) st=STATUS_FILE_NOT_FOUND;
                else {
                    uint8_t* d=NULL; size_t sz=0;
                    storage_retrieve_file(w->storage,req.filename,&d,&sz);
                    protocol_init_response(&resp,STATUS_SUCCESS,req.filename,sz,d,sz);
                }
                break;
            case CMD_DELETE:
                if(!storage_file_exists(w->storage,req.filename)) st=STATUS_FILE_NOT_FOUND;
                else { storage_delete_file(w->storage,req.filename); metadata_remove(w->metadata,req.filename); }
                protocol_init_response(&resp,st,req.filename,0,NULL,0);
                break;
            case CMD_LIST: {
                char files[1000][256]; int cnt=0;
                storage_list_files(w->storage,(char**)files,&cnt);
                char buf[10000]; int off=0;
                for(int i=0;i<cnt;i++) off+=snprintf(buf+off,sizeof(buf)-off,"%s\n",files[i]);
                protocol_init_response(&resp,STATUS_SUCCESS,"",0,(uint8_t*)buf,strlen(buf));
                break; }
            case CMD_INFO: {
                metadata_entry_t e;
                if(metadata_get(w->metadata,req.filename,&e)!=0) st=STATUS_FILE_NOT_FOUND;
                else {
                    char buf[512];
                    snprintf(buf,sizeof(buf),"Size: %lu\nUpload: %ld\nModify: %ld\nChecksum: %u\n",e.size,e.upload_time,e.modify_time,e.checksum);
                    protocol_init_response(&resp,STATUS_SUCCESS,req.filename,e.size,(uint8_t*)buf,strlen(buf));
                }
                break; }
            case CMD_EXIT:
                w->running=0;
                protocol_init_response(&resp,STATUS_SUCCESS,NULL,0,NULL,0);
                break;
            default:
                st=STATUS_INVALID_COMMAND;
                protocol_init_response(&resp,st,NULL,0,NULL,0);
        }
        send_response(w->client_fd,&resp);
        if(resp.payload) free(resp.payload);
        if(req.payload) free(req.payload);
        if(req.command_type==CMD_EXIT) break;
    }
    close(w->client_fd);
    return NULL;
}
