#include "kvs_linux.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define KVS_BIN_FILE "kvs_data.bin"

int main() {
    // KVS first valid entry at offset 0x3E010
    long kvs_offset = 0x3E010;
    
    printf("Initializing KVS from %s at offset 0x%lx...\n", KVS_BIN_FILE, kvs_offset);
    if (kvs_linux_init_offset(KVS_BIN_FILE, kvs_offset) != 0) {
        fprintf(stderr, "Failed to initialize KVS.\n");
        return -1;
    }
    printf("KVS initialized.\n");

    FILE *fp = kvs_linux_get_file();
    
    // First let's dump the raw data to see what's there
    printf("\n--- Raw KVS data dump (first 0x100 bytes at 0x3E000) ---\n");
    unsigned char buf[256];
    fseek(fp, kvs_offset, SEEK_SET);
    fread(buf, 1, 256, fp);
    for (int i = 0; i < 256; i += 16) {
        printf("%04x: ", i);
        for (int j = 0; j < 16; j++) {
            printf("%02x ", buf[i+j]);
        }
        printf(" |");
        for (int j = 0; j < 16; j++) {
            char c = buf[i+j];
            printf("%c", (c >= 32 && c < 127) ? c : '.');
        }
        printf("|\n");
    }

    // Now test reading
    char read_buf[1024];
    const char *test_keys[] = {
        "/srf/pair_info",
        "/srf/wifi_info", 
        "/boot_times",
        NULL
    };

    printf("\n--- Testing KVS read ---\n");
    for (int i = 0; test_keys[i] != NULL; i++) {
        size_t value_size = kvs_linux_get_value_size(test_keys[i]);
        printf("Key '%s': size=%zu\n", test_keys[i], value_size);
        if (value_size > 0 && value_size < sizeof(read_buf)) {
            int ret = kvs_linux_read(test_keys[i], read_buf, sizeof(read_buf));
            if (ret == 0) {
                printf("  Value (hex): ");
                for (size_t j = 0; j < value_size; j++) {
                    printf("%02x ", (unsigned char)read_buf[j]);
                }
                printf("\n");
            }
        }
    }

    kvs_linux_deinit();
    return 0;
}