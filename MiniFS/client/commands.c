#include "commands.h"
#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

static uint64_t htonll(uint64_t v) { const int n=1; return *(char*)&n==1?((uint64_t)htonl(v&0xFFFFFFFF)<<32)|htonl(v>>32):v; }
static uint64_t ntohll(uint64_t v) { const int n=1; return *(char*)&n==1?((uint64_t)ntohl(v&0xFFFFFFFF)<<32)|ntohl(v>>32):v; }

int client_init(client_context_t* c, const char* addr, int port) {
    if(!c||!addr) return -1;
    memset(c,0,sizeof(*c));
    strncpy(c->server_addr,addr,sizeof(c->server_addr)-1);
    c->server_port=port;
    c->connected=0;
    c->server_fd=-1;
    return 0;
}

int client_connect(client_context_t* c) {
    if(!c) return -1;
    c->server_fd=socket(AF_INET,SOCK_STREAM,0);
    if(c->server_fd<0) return -1;
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(c->server_port);
    if(inet_pton(AF_INET,c->server_addr,&addr.sin_addr)<=0) { close(c->server_fd); return -1; }
    if(connect(c->server_fd,(struct sockaddr*)&addr,sizeof(addr))<0) { close(c->server_fd); return -1; }
    c->connected=1;
    return 0;
}

void client_disconnect(client_context_t* c) {
    if(c) {
        if(c->server_fd>=0) close(c->server_fd);
        c->connected=0;
    }
}

static int send_req(client_context_t* c, request_packet_t* r) {
    size_t sz=protocol_request_size(r);
    uint8_t* b=malloc(sz);
    protocol_serialize_request(r,b,sz);
    utils_send_all(c->server_fd,b,sz);
    free(b);
    return 0;
}

static int recv_resp(client_context_t* c, response_packet_t* r) {
    uint8_t h[20];
    utils_recv_exact(c->server_fd,h,20);
    uint32_t st; memcpy(&st,h,4); r->status=ntohl(st);
    uint32_t fl; memcpy(&fl,h+4,4); r->filename_length=ntohl(fl);
    uint64_t fs; memcpy(&fs,h+8,8); r->file_size=ntohll(fs);
    uint32_t ps; memcpy(&ps,h+16,4); r->payload_size=ntohl(ps);
    if(r->filename_length>0) utils_recv_exact(c->server_fd,(uint8_t*)r->filename,r->filename_length);
    if(r->payload_size>0) {
        r->payload=malloc(r->payload_size);
        utils_recv_exact(c->server_fd,r->payload,r->payload_size);
    }
    return 0;
}

int cmd_upload(client_context_t* c, const char* local, const char* remote) {
    uint8_t* d=NULL; size_t sz=0;
    if(utils_read_file(local,&d,&sz)!=0) return -1;
    printf("Uploading %s (%zu bytes)...\n",local,sz);
    request_packet_t req;
    protocol_init_request(&req,CMD_UPLOAD,remote,sz,d,sz);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    printf("Upload: %s\n",resp.status==STATUS_SUCCESS?"Success":"Failed");
    if(resp.payload) free(resp.payload);
    free(d);
    return resp.status==STATUS_SUCCESS?0:-1;
}

int cmd_download(client_context_t* c, const char* remote, const char* local) {
    printf("Downloading %s...\n",remote);
    request_packet_t req;
    protocol_init_request(&req,CMD_DOWNLOAD,remote,0,NULL,0);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    if(resp.status==STATUS_SUCCESS) {
        utils_write_file(local,resp.payload,resp.payload_size);
        printf("Download successful (%u bytes)\n",resp.payload_size);
    } else printf("Download failed\n");
    if(resp.payload) free(resp.payload);
    return resp.status==STATUS_SUCCESS?0:-1;
}

int cmd_delete(client_context_t* c, const char* filename) {
    printf("Deleting %s...\n",filename);
    request_packet_t req;
    protocol_init_request(&req,CMD_DELETE,filename,0,NULL,0);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    printf("Delete: %s\n",resp.status==STATUS_SUCCESS?"Success":"Failed");
    return resp.status==STATUS_SUCCESS?0:-1;
}

int cmd_list(client_context_t* c) {
    printf("Listing files...\n");
    request_packet_t req;
    protocol_init_request(&req,CMD_LIST,NULL,0,NULL,0);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    if(resp.payload) printf("%s",resp.payload);
    if(resp.payload) free(resp.payload);
    return resp.status==STATUS_SUCCESS?0:-1;
}

int cmd_info(client_context_t* c, const char* filename) {
    printf("Getting info for %s...\n",filename);
    request_packet_t req;
    protocol_init_request(&req,CMD_INFO,filename,0,NULL,0);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    if(resp.payload) printf("%s",resp.payload);
    if(resp.payload) free(resp.payload);
    return resp.status==STATUS_SUCCESS?0:-1;
}

int cmd_exit(client_context_t* c) {
    printf("Exiting...\n");
    request_packet_t req;
    protocol_init_request(&req,CMD_EXIT,NULL,0,NULL,0);
    send_req(c,&req);
    response_packet_t resp;
    memset(&resp,0,sizeof(resp));
    recv_resp(c,&resp);
    client_disconnect(c);
    return 0;
}
