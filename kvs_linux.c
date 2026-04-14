#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <easyflash.h>
#include "easyflash/inc/ef_linux.h"

#define DEFAULT_DISK_FILE "ef.disk"
#define DEFAULT_CACHE_DIR "cache"

static void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  --load-disk <disk_file>    Specify disk file to load (default: %s)\n", DEFAULT_DISK_FILE);
    printf("  --list-keys                List all KVS keys\n");
    printf("  -r <key>                   Read key value\n");
    printf("  -w <key> <value>           Write key value\n");
    printf("  -h                         Show this help\n");
    printf("\nExample:\n");
    printf("  %s --load-disk flash_dump.bin --list-keys\n", prog);
    printf("  %s -r boot_times\n", prog);
}

static int ensure_disk_file(const char *disk_path) {
    char dest_path[256];
    snprintf(dest_path, sizeof(dest_path), "%s/%s", DEFAULT_CACHE_DIR, DEFAULT_DISK_FILE);
    
    if (access(DEFAULT_CACHE_DIR, F_OK) != 0) {
        if (mkdir(DEFAULT_CACHE_DIR, 0755) != 0) {
            fprintf(stderr, "Failed to create cache directory: %s\n", DEFAULT_CACHE_DIR);
            return -1;
        }
    }

    if (strcmp(disk_path, dest_path) != 0) {
        FILE *src = fopen(disk_path, "rb");
        if (src == NULL) {
            fprintf(stderr, "Failed to open disk file: %s\n", disk_path);
            return -1;
        }

        FILE *dst = fopen(dest_path, "wb");
        if (dst == NULL) {
            fprintf(stderr, "Failed to create cache file: %s\n", dest_path);
            fclose(src);
            return -1;
        }

        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
            fwrite(buf, 1, n, dst);
        }

        fclose(src);
        fclose(dst);
        printf("Loaded %s\n", disk_path);
    }

    return 0;
}

static int list_keys(void) {
    EfErrCode ret = easyflash_init();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "EasyFlash init failed: %d\n", ret);
        return -1;
    }

    ret = ef_load_env();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "Load ENV failed: %d\n", ret);
        return -1;
    }

    printf("All KVS keys:\n");
    ef_print_env();
    return 0;
}

static int read_key(const char *key) {
    EfErrCode ret = easyflash_init();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "EasyFlash init failed: %d\n", ret);
        return -1;
    }

    ret = ef_load_env();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "Load ENV failed: %d\n", ret);
        return -1;
    }

    uint8_t buf[4096];
    size_t saved_len = 0;
    size_t size = ef_get_env_blob(key, buf, sizeof(buf), &saved_len);

    if (size > 0) {
        printf("Key '%s' = ", key);
        for (size_t i = 0; i < size && i < saved_len; i++) {
            printf("%02x", buf[i]);
        }
        printf("\n");
        return 0;
    } else {
        printf("Key '%s' not found\n", key);
        return -1;
    }
}

static int write_key(const char *key, const char *value) {
    EfErrCode ret = easyflash_init();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "EasyFlash init failed: %d\n", ret);
        return -1;
    }

    ret = ef_load_env();
    if (ret != EF_NO_ERR) {
        fprintf(stderr, "Load ENV failed: %d\n", ret);
        return -1;
    }

    ret = ef_set_env(key, value);
    if (ret == EF_NO_ERR) {
        printf("Key '%s' = '%s' written successfully\n", key, value);
        return 0;
    } else {
        fprintf(stderr, "Failed to write key: %d\n", ret);
        return -1;
    }
}

int main(int argc, char *argv[]) {
    const char *disk_file = NULL;
    int list_keys_flag = 0;
    char *read_key_arg = NULL;
    char *write_key_arg = NULL;
    char *write_value_arg = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--load-disk") == 0 && i + 1 < argc) {
            disk_file = argv[++i];
        } else if (strcmp(argv[i], "--list-keys") == 0) {
            list_keys_flag = 1;
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            read_key_arg = argv[++i];
        } else if (strcmp(argv[i], "-w") == 0 && i + 2 < argc) {
            write_key_arg = argv[++i];
            write_value_arg = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    if (disk_file == NULL && access(DEFAULT_CACHE_DIR "/" DEFAULT_DISK_FILE, F_OK) != 0) {
        fprintf(stderr, "No disk file (.bin) specified, please use --load-disk to specify one at the first use\n");
        print_usage(argv[0]);
        return -1;
    }

    if (disk_file != NULL) {
        if (ensure_disk_file(disk_file) != 0) {
            return -1;
        }
    }

    if (list_keys_flag) {
        return list_keys();
    } else if (read_key_arg != NULL) {
        return read_key(read_key_arg);
    } else if (write_key_arg != NULL && write_value_arg != NULL) {
        return write_key(write_key_arg, write_value_arg);
    } else {
        print_usage(argv[0]);
        return 0;
    }
}