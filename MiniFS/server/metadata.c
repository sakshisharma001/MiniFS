#include "metadata.h"
#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int metadata_init(metadata_t* m, const char* mf) {
    if(!m||!mf) return -1;
    memset(m,0,sizeof(*m));
    m->count=0;
    pthread_mutex_init(&m->lock,NULL);
    strncpy(m->metadata_file,mf,sizeof(m->metadata_file)-1);
    metadata_load(m);
    return 0;
}

void metadata_cleanup(metadata_t* m) {
    if(m) {
        pthread_mutex_destroy(&m->lock);
    }
}

int metadata_add(metadata_t* m, const char* fn, uint64_t sz, uint32_t cs) {
    if(!m||!fn||m->count>=MAX_METADATA_ENTRIES) return -1;
    pthread_mutex_lock(&m->lock);
    strncpy(m->entries[m->count].filename,fn,MAX_FILENAME_LEN-1);
    m->entries[m->count].size=sz;
    m->entries[m->count].upload_time=time(NULL);
    m->entries[m->count].modify_time=time(NULL);
    m->entries[m->count].checksum=cs;
    m->count++;
    pthread_mutex_unlock(&m->lock);
    metadata_save(m);
    return 0;
}

int metadata_remove(metadata_t* m, const char* fn) {
    if(!m||!fn) return -1;
    pthread_mutex_lock(&m->lock);
    for(int i=0;i<m->count;i++) {
        if(strcmp(m->entries[i].filename,fn)==0) {
            for(int j=i;j<m->count-1;j++) m->entries[j]=m->entries[j+1];
            m->count--;
            pthread_mutex_unlock(&m->lock);
            metadata_save(m);
            return 0;
        }
    }
    pthread_mutex_unlock(&m->lock);
    return -1;
}

int metadata_get(metadata_t* m, const char* fn, metadata_entry_t* e) {
    if(!m||!fn||!e) return -1;
    pthread_mutex_lock(&m->lock);
    for(int i=0;i<m->count;i++) {
        if(strcmp(m->entries[i].filename,fn)==0) {
            *e=m->entries[i];
            pthread_mutex_unlock(&m->lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&m->lock);
    return -1;
}

int metadata_update(metadata_t* m, const char* fn, uint64_t sz, uint32_t cs) {
    if(!m||!fn) return -1;
    pthread_mutex_lock(&m->lock);
    for(int i=0;i<m->count;i++) {
        if(strcmp(m->entries[i].filename,fn)==0) {
            m->entries[i].size=sz;
            m->entries[i].modify_time=time(NULL);
            m->entries[i].checksum=cs;
            pthread_mutex_unlock(&m->lock);
            metadata_save(m);
            return 0;
        }
    }
    pthread_mutex_unlock(&m->lock);
    return -1;
}

int metadata_list(metadata_t* m, metadata_entry_t* e, int* cnt) {
    if(!m||!e||!cnt) return -1;
    pthread_mutex_lock(&m->lock);
    *cnt=m->count;
    for(int i=0;i<m->count;i++) e[i]=m->entries[i];
    pthread_mutex_unlock(&m->lock);
    return 0;
}

int metadata_save(metadata_t* m) {
    if(!m) return -1;
    FILE* f=fopen(m->metadata_file,"w");
    if(!f) return -1;
    pthread_mutex_lock(&m->lock);
    fprintf(f,"%d\n",m->count);
    for(int i=0;i<m->count;i++) {
        fprintf(f,"%s %lu %ld %ld %u\n",m->entries[i].filename,m->entries[i].size,m->entries[i].upload_time,m->entries[i].modify_time,m->entries[i].checksum);
    }
    pthread_mutex_unlock(&m->lock);
    fclose(f);
    return 0;
}

int metadata_load(metadata_t* m) {
    if(!m) return -1;
    FILE* f=fopen(m->metadata_file,"r");
    if(!f) return 0;
    pthread_mutex_lock(&m->lock);
    fscanf(f,"%d",&m->count);
    for(int i=0;i<m->count;i++) {
        fscanf(f,"%s %lu %ld %ld %u",m->entries[i].filename,&m->entries[i].size,&m->entries[i].upload_time,&m->entries[i].modify_time,&m->entries[i].checksum);
    }
    pthread_mutex_unlock(&m->lock);
    fclose(f);
    return 0;
}

int metadata_validate_consistency(metadata_t* m, storage_t* s) {
    if(!m||!s) return -1;
    return 0;
}
