#ifndef METADATA_H
#define METADATA_H
#include <pthread.h>
#include <time.h>
#include "../common/utils.h"
#include "storage.h"
#define MAX_METADATA_ENTRIES 1000
typedef struct {
    char filename[MAX_FILENAME_LEN];
    uint64_t size;
    time_t upload_time;
    time_t modify_time;
    uint32_t checksum;
} metadata_entry_t;
typedef struct {
    metadata_entry_t entries[MAX_METADATA_ENTRIES];
    int count;
    pthread_mutex_t lock;
    char metadata_file[256];
} metadata_t;
int metadata_init(metadata_t* m, const char* mf);
void metadata_cleanup(metadata_t* m);
int metadata_add(metadata_t* m, const char* fn, uint64_t sz, uint32_t cs);
int metadata_remove(metadata_t* m, const char* fn);
int metadata_get(metadata_t* m, const char* fn, metadata_entry_t* e);
int metadata_update(metadata_t* m, const char* fn, uint64_t sz, uint32_t cs);
int metadata_list(metadata_t* m, metadata_entry_t* e, int* cnt);
int metadata_save(metadata_t* m);
int metadata_load(metadata_t* m);
int metadata_validate_consistency(metadata_t* m, storage_t* s);
#endif
