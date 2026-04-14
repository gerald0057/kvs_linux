#ifndef __KVS_LINUX_H__
#define __KVS_LINUX_H__

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int kvs_linux_init(const char *bin_file_path);
int kvs_linux_init_offset(const char *bin_file_path, long offset);
FILE *kvs_linux_get_file(void);
int kvs_linux_write(const char *key, const void *buf, size_t size);
int kvs_linux_read(const char *key, void *buf, size_t size);
size_t kvs_linux_get_value_size(const char *key);
int kvs_linux_delete(const char *key);
void kvs_linux_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
