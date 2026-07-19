#define _GNU_SOURCE
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

int storage_init(storage_t* storage, const char* storage_dir) {
    if(!storage||!storage_dir) return -1;
    memset(storage,0,sizeof(*storage));
    strncpy(storage->storage_dir,storage_dir,sizeof(storage->storage_dir)-1);
    pthread_mutex_init(&storage->mutex,NULL);
    mkdir(storage->storage_dir,0755);
    return 0;
}

void storage_cleanup(storage_t* storage) {
    if(storage) pthread_mutex_destroy(&storage->mutex);
}

static char* get_file_path(storage_t* storage, const char* filename) {
    static char path[512];
    snprintf(path,sizeof(path),"%s/%s",storage->storage_dir,filename);
    return path;
}

int storage_store_file(storage_t* storage, const char* filename, const uint8_t* data, size_t size) {
    if(!storage||!filename||!data) return -1;
    pthread_mutex_lock(&storage->mutex);
    char* path=get_file_path(storage,filename);
    FILE* f=fopen(path,"wb");
    if(!f) { pthread_mutex_unlock(&storage->mutex); return -1; }
    fwrite(data,1,size,f);
    fclose(f);
    storage->total_files++;
    storage->total_size+=size;
    pthread_mutex_unlock(&storage->mutex);
    return 0;
}

int storage_retrieve_file(storage_t* storage, const char* filename, uint8_t** data, size_t* size) {
    if(!storage||!filename||!data||!size) return -1;
    pthread_mutex_lock(&storage->mutex);
    char* path=get_file_path(storage,filename);
    FILE* f=fopen(path,"rb");
    if(!f) { pthread_mutex_unlock(&storage->mutex); return -1; }
    fseek(f,0,SEEK_END);
    long sz=ftell(f);
    fseek(f,0,SEEK_SET);
    *data=malloc(sz);
    fread(*data,1,sz,f);
    fclose(f);
    *size=sz;
    pthread_mutex_unlock(&storage->mutex);
    return 0;
}

int storage_delete_file(storage_t* storage, const char* filename) {
    if(!storage||!filename) return -1;
    pthread_mutex_lock(&storage->mutex);
    char* path=get_file_path(storage,filename);
    unlink(path);
    storage->total_files--;
    pthread_mutex_unlock(&storage->mutex);
    return 0;
}

int storage_file_exists(storage_t* storage, const char* filename) {
    if(!storage||!filename) return 0;
    pthread_mutex_lock(&storage->mutex);
    char* path=get_file_path(storage,filename);
    int exists=access(path,F_OK)==0;
    pthread_mutex_unlock(&storage->mutex);
    return exists;
}

int storage_list_files(storage_t* storage, char** files, int* count) {
    if(!storage||!files||!count) return -1;
    pthread_mutex_lock(&storage->mutex);
    DIR* dir=opendir(storage->storage_dir);
    if(!dir) { pthread_mutex_unlock(&storage->mutex); return -1; }
    struct dirent* entry;
    int cnt=0;
    while((entry=readdir(dir))!=NULL) {
        if(entry->d_type==DT_REG) cnt++;
    }
    rewinddir(dir);
    *count=cnt;
    int i=0;
    while((entry=readdir(dir))!=NULL&&i<cnt) {
        if(entry->d_type==DT_REG) {
            strcpy(files[i],entry->d_name);
            i++;
        }
    }
    closedir(dir);
    pthread_mutex_unlock(&storage->mutex);
    return 0;
}

int storage_get_file_info(storage_t* storage, const char* filename, uint64_t* size, time_t* upload_time, time_t* modify_time, uint32_t* checksum) {
    if(!storage||!filename) return -1;
    pthread_mutex_lock(&storage->mutex);
    char* path=get_file_path(storage,filename);
    struct stat st;
    stat(path,&st);
    if(size) *size=st.st_size;
    if(upload_time) *upload_time=st.st_ctime;
    if(modify_time) *modify_time=st.st_mtime;
    pthread_mutex_unlock(&storage->mutex);
    return 0;
}

int storage_verify_checksum(storage_t* storage, const char* filename, uint32_t expected_checksum) {
    if(!storage||!filename) return -1;
    uint8_t* data=NULL;
    size_t size=0;
    if(storage_retrieve_file(storage,filename,&data,&size)!=0) return -1;
    uint32_t cs=0;
    for(size_t i=0;i<size;i++) cs=(cs<<8)^(cs>>24)+data[i];
    free(data);
    return cs==expected_checksum?0:-1;
}

void storage_get_stats(storage_t* storage, uint64_t* total_files, uint64_t* total_size) {
    if(!storage) return;
    pthread_mutex_lock(&storage->mutex);
    if(total_files) *total_files=storage->total_files;
    if(total_size) *total_size=storage->total_size;
    pthread_mutex_unlock(&storage->mutex);
}
