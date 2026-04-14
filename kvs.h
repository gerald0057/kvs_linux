#ifndef __KVS_H__
#define __KVS_H__

#include <stddef.h>

int kvs_init(void);
int kvs_write(const char *key, const void *buf, size_t size);
int kvs_read(const char *key, void *buf, size_t size);
size_t kvs_get_value_size(const char *key);
int kvs_delete(const char *key);

#endif
