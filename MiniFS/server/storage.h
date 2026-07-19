#ifndef STORAGE_H
#define STORAGE_H
#include <pthread.h>
#include "../common/utils.h"
typedef struct {
    char storage_dir[256];
    pthread_mutex_t mutex;
    uint64_t total_files;
    uint64_t total_size;
} storage_t;
int storage_init(storage_t* storage, const char* storage_dir);
void storage_cleanup(storage_t* storage);
int storage_store_file(storage_t* storage, const char* filename, const uint8_t* data, size_t size);
int storage_retrieve_file(storage_t* storage, const char* filename, uint8_t** data, size_t* size);
int storage_delete_file(storage_t* storage, const char* filename);
int storage_file_exists(storage_t* storage, const char* filename);
int storage_list_files(storage_t* storage, char** files, int* count);
int storage_get_file_info(storage_t* storage, const char* filename, uint64_t* size, time_t* upload_time, time_t* modify_time, uint32_t* checksum);
int storage_verify_checksum(storage_t* storage, const char* filename, uint32_t expected_checksum);
void storage_get_stats(storage_t* storage, uint64_t* total_files, uint64_t* total_size);
#endif
