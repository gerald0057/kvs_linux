#include "kvs_linux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define ENV_HDR_SIZE 21
#define ENV_MAGIC 0x3034564B     // 'K'(0x4B) + 'V'(0x56) + '4'(0x34) + '0'(0x30) in little-endian

static FILE *kvs_file = NULL;
static char kvs_file_path[256];
static long kvs_offset = 0;

#define KVS_LINUX_DEBUG(...) printf(__VA_ARGS__)

int kvs_linux_init(const char *bin_file_path) {
    return kvs_linux_init_offset(bin_file_path, 0);
}

int kvs_linux_init_offset(const char *bin_file_path, long offset) {
    if (kvs_file != NULL) {
        KVS_LINUX_DEBUG("KVS already initialized.\n");
        return 0;
    }

    strncpy(kvs_file_path, bin_file_path, sizeof(kvs_file_path) - 1);
    kvs_file_path[sizeof(kvs_file_path) - 1] = '\0';

    kvs_file = fopen(kvs_file_path, "rb");
    if (kvs_file == NULL) {
        perror("Failed to open KVS bin file");
        return -1;
    }

    kvs_offset = offset;
    if (fseek(kvs_file, kvs_offset, SEEK_SET) != 0) {
        perror("Failed to seek to KVS offset");
        fclose(kvs_file);
        kvs_file = NULL;
        return -1;
    }

    KVS_LINUX_DEBUG("KVS initialized successfully with file: %s at offset 0x%lx\n", kvs_file_path, kvs_offset);
    return 0;
}

void kvs_linux_deinit(void) {
    if (kvs_file != NULL) {
        fclose(kvs_file);
        kvs_file = NULL;
        KVS_LINUX_DEBUG("KVS deinitialized.\n");
    }
}

FILE *kvs_linux_get_file(void) {
    return kvs_file;
}

// ENV header format at 0x3E010:
// Offset 0: status (0xEF = valid)
// Offset 1-3: padding (0xFF 0xFF 0xFF)
// Offset 4-7: magic "KV40" (0x4B 0x56 0x34 0x30)
// Offset 8-11: len (little-endian) = 54
// Offset 12-15: crc32
// Offset 16: name_len
// Then key starts at offset 17?

static int read_env_entry(uint32_t *out_magic, uint32_t *out_len, uint8_t *out_name_len, 
                          uint32_t *out_value_len, char *key_buf, size_t key_buf_size, 
                          void *value_buf, size_t value_buf_size, int *out_deleted) {
    // Header format (24 bytes total):
    // offset  0: status (1 byte)
    // offset  1-3: padding (3 bytes)
    // offset  4-7: magic (4 bytes)
    // offset  8-11: len (4 bytes) - total entry length
    // offset 12-15: crc32 (4 bytes)
    // offset 16: name_len (1 byte)
    // offset 17-19: padding (3 bytes)
    // offset 20-23: value_len (4 bytes)
    // offset 24+: key + value
    
    unsigned char status, name_len;
    uint32_t magic, len, value_len;
    
    if (fread(&status, 1, 1, kvs_file) != 1) return -1;
    fseek(kvs_file, 3, SEEK_CUR); // Skip 3 bytes padding
    if (fread(&magic, 4, 1, kvs_file) != 1) return -1;
    if (fread(&len, 4, 1, kvs_file) != 1) return -1;
    fseek(kvs_file, 4, SEEK_CUR); // Skip crc32
    if (fread(&name_len, 1, 1, kvs_file) != 1) return -1;
    fseek(kvs_file, 3, SEEK_CUR); // Skip 3 bytes padding
    if (fread(&value_len, 4, 1, kvs_file) != 1) return -1;
    
    KVS_LINUX_DEBUG("Header: status=0x%02x magic=0x%08X len=%u name_len=%u value_len=%u\n", 
                   status, magic, len, name_len, value_len);
    
    *out_magic = magic;
    *out_len = len;
    *out_name_len = name_len;
    *out_value_len = value_len;
    *out_deleted = (status == 0xff || status == 0xbf);

    if (magic != ENV_MAGIC) {
        KVS_LINUX_DEBUG("Invalid magic 0x%08X\n", magic);
        return -2;
    }

    // Read key (starts at offset 24)
    char key_name[256];
    if (fread(key_name, 1, name_len, kvs_file) != name_len) {
        return -1;
    }
    key_name[name_len] = '\0';
    KVS_LINUX_DEBUG("Found key: '%s'\n", key_name);

    if (key_buf && key_buf_size > 0) {
        size_t to_copy = (name_len < key_buf_size - 1) ? name_len : key_buf_size - 1;
        memcpy(key_buf, key_name, to_copy);
        key_buf[to_copy] = '\0';
    }

    if (status == 0xff || status == 0xbf) {
        fseek(kvs_file, value_len, SEEK_CUR);
        return 0;
    }

    // Read value
    if (value_buf && value_buf_size > 0) {
        size_t to_read = (value_len < value_buf_size) ? value_len : value_buf_size;
        if (fread(value_buf, 1, to_read, kvs_file) != to_read) {
            return -1;
        }
        if (value_len > to_read) {
            fseek(kvs_file, value_len - to_read, SEEK_CUR);
        }
        return 0;
    } else {
        fseek(kvs_file, value_len, SEEK_CUR);
    }

    return 0;
}

int kvs_linux_read(const char *key, void *buf, size_t size) {
    if (kvs_file == NULL) {
        KVS_LINUX_DEBUG("KVS not initialized.\n");
        return -1;
    }
    if (key == NULL || buf == NULL || size == 0) {
        return -1;
    }

    if (fseek(kvs_file, kvs_offset, SEEK_SET) != 0) {
        return -1;
    }

    uint32_t magic, len, value_len;
    uint8_t name_len;
    char key_name[256];
    int deleted;
    int ret = -1;

    while (read_env_entry(&magic, &len, &name_len, &value_len, key_name, sizeof(key_name), buf, size, &deleted) == 0) {
        if (!deleted && strcmp(key, key_name) == 0) {
            KVS_LINUX_DEBUG("Found key '%s' value_len=%u\n", key, value_len);
            if (value_len > size) {
                ret = value_len;
            } else {
                ret = 0;
            }
            break;
        }
    }

    return ret;
}

size_t kvs_linux_get_value_size(const char *key) {
    if (kvs_file == NULL) {
        return 0;
    }
    if (key == NULL) {
        return 0;
    }

    if (fseek(kvs_file, kvs_offset, SEEK_SET) != 0) {
        return 0;
    }

    uint32_t magic, len, value_len;
    uint8_t name_len;
    char key_name[256];
    int deleted;
    size_t ret_size = 0;

    while (read_env_entry(&magic, &len, &name_len, &value_len, key_name, sizeof(key_name), NULL, 0, &deleted) == 0) {
        if (!deleted && strcmp(key, key_name) == 0) {
            ret_size = value_len;
            break;
        }
    }

    return ret_size;
}

int kvs_linux_write(const char *key, const void *buf, size_t size) {
    (void)key;
    (void)buf;
    (void)size;
    return -1;
}

int kvs_linux_delete(const char *key) {
    (void)key;
    return -1;
}