#include <stdio.h>
#include <easyflash.h>
#include "easyflash/inc/ef_linux.h"

int main() {
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

    printf("ENV loaded. All keys:\n");
    ef_print_env();

    const char *test_keys[] = {
        "/srf/pair_info",
        NULL
    };

    printf("\n--- Test reading keys ---\n");
    for (int i = 0; test_keys[i] != NULL; i++) {
        uint8_t buf[256];
        size_t saved_len = 0;
        
        printf("Trying to get key: %s\n", test_keys[i]);
        
        size_t size = ef_get_env_blob(test_keys[i], buf, sizeof(buf), &saved_len);
        printf("  ef_get_env_blob returned size=%zu, saved_len=%zu\n", size, saved_len);
        
        if (size > 0) {
            printf("Key '%s': size=%zu, value (hex): ", test_keys[i], size);
            for (size_t j = 0; j < size && j < saved_len; j++) {
                printf("%02x", buf[j]);
            }
            printf("\n");
        } else {
            printf("Key '%s': (not found)\n", test_keys[i]);
        }
    }
    return 0;
}