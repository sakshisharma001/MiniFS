#define _GNU_SOURCE
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
static log_level_t current_log_level=LOG_INFO; static FILE* log_file=NULL; static pthread_mutex_t log_mutex=PTHREAD_MUTEX_INITIALIZER;
void utils_set_log_level(log_level_t l) { current_log_level=l; }
void utils_log_init(const char* lf) { if(lf) { log_file=fopen(lf,"a"); if(!log_file) fprintf(stderr,"Failed to open log file: %s\n",lf); } }
void utils_log_cleanup(void) { if(log_file) { fclose(log_file); log_file=NULL; } }
void utils_log(log_level_t l, const char* fmt, ...) { if(l<current_log_level) return; pthread_mutex_lock(&log_mutex); time_t now=time(NULL); struct tm* tm=localtime(&now); char ts[64]; strftime(ts,sizeof(ts),"%Y-%m-%d %H:%M:%S",tm); const char* ls; switch(l) { case LOG_DEBUG: ls="DEBUG"; break; case LOG_INFO: ls="INFO"; break; case LOG_WARN: ls="WARN"; break; case LOG_ERROR: ls="ERROR"; break; default: ls="UNKNOWN"; } FILE* out=log_file?log_file:stdout; fprintf(out,"[%s] [%s] ",ts,ls); va_list args; va_start(args,fmt); vfprintf(out,fmt,args); va_end(args); fprintf(out,"\n"); fflush(out); pthread_mutex_unlock(&log_mutex); }
int utils_validate_filename(const char* f) { if(!f||strlen(f)==0||strlen(f)>=MAX_FILENAME_LEN) return 0; if(strstr(f,"..")!=NULL||f[0]=='/') return 0; for(size_t i=0;i<strlen(f);i++) { char c=f[i]; if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.')) return 0; } return 1; }
int utils_sanitize_filename(char* f) { if(!f) return -1; for(size_t i=0;i<strlen(f);i++) { char c=f[i]; if(!((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.')) f[i]='_'; } return 0; }
uint32_t utils_calculate_checksum(const uint8_t* d, size_t s) { if(!d||s==0) return 0; uint32_t cs=0; for(size_t i=0;i<s;i++) { cs=(cs<<8)^(cs>>24); cs+=d[i]; } return cs; }
int utils_read_file(const char* fp, uint8_t** b, size_t* s) { if(!fp||!b||!s) return -1; FILE* f=fopen(fp,"rb"); if(!f) return -1; fseek(f,0,SEEK_END); long fs=ftell(f); fseek(f,0,SEEK_SET); if(fs<=0) { fclose(f); return -1; } *b=malloc(fs); if(!*b) { fclose(f); return -1; } size_t rs=fread(*b,1,fs,f); fclose(f); if(rs!=(size_t)fs) { free(*b); *b=NULL; return -1; } *s=rs; return 0; }
int utils_write_file(const char* fp, const uint8_t* b, size_t s) { if(!fp||!b) return -1; FILE* f=fopen(fp,"wb"); if(!f) return -1; size_t w=fwrite(b,1,s,f); fclose(f); return w==s?0:-1; }
int utils_file_exists(const char* fp) { if(!fp) return 0; struct stat st; return stat(fp,&st)==0?1:0; }
int utils_get_file_size(const char* fp, uint64_t* s) { if(!fp||!s) return -1; struct stat st; if(stat(fp,&st)!=0) return -1; *s=st.st_size; return 0; }
int utils_delete_file(const char* fp) { if(!fp) return -1; return unlink(fp); }
int utils_set_socket_timeout(int fd, int sec) { struct timeval to; to.tv_sec=sec; to.tv_usec=0; return setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&to,sizeof(to))<0||setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&to,sizeof(to))<0?-1:0; }
int utils_create_server_socket(int p) { int fd=socket(AF_INET,SOCK_STREAM,0); if(fd<0) return -1; int opt=1; if(setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0) { close(fd); return -1; } struct sockaddr_in a; memset(&a,0,sizeof(a)); a.sin_family=AF_INET; a.sin_addr.s_addr=INADDR_ANY; a.sin_port=htons(p); if(bind(fd,(struct sockaddr*)&a,sizeof(a))<0) { close(fd); return -1; } if(listen(fd,10)<0) { close(fd); return -1; } return fd; }
int utils_create_client_socket(const char* a, int p) { int fd=socket(AF_INET,SOCK_STREAM,0); if(fd<0) return -1; struct sockaddr_in sa; memset(&sa,0,sizeof(sa)); sa.sin_family=AF_INET; sa.sin_port=htons(p); if(inet_pton(AF_INET,a,&sa.sin_addr)<=0) { close(fd); return -1; } if(connect(fd,(struct sockaddr*)&sa,sizeof(sa))<0) { close(fd); return -1; } return fd; }
int utils_send_all(int fd, const uint8_t* d, size_t s) { size_t ts=0; while(ts<s) { ssize_t sent=send(fd,d+ts,s-ts,0); if(sent<0) return -1; if(sent==0) return -1; ts+=sent; } return 0; }
int utils_recv_exact(int fd, uint8_t* b, size_t s) { size_t tr=0; while(tr<s) { ssize_t r=recv(fd,b+tr,s-tr,0); if(r<0) return -1; if(r==0) return -1; tr+=r; } return 0; }
