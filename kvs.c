#include <stdio.h>
#include <stdlib.h>
#include <easyflash.h>
#include <drv_common.h>
#include "kvs.h"

int kvs_init(void)
{
    if(easyflash_init() == EF_NO_ERR)
        return 0;

    return -1;
}

int kvs_write(const char *key, const void *buf, size_t size)
{
    int ret = -1;

    if(key == NULL || buf == NULL || size == 0)
        return -1;

    char *tmp_key = strdup(key);
    if(tmp_key == NULL) {
        return -1;
    }

    char *tmp_buf = NULL;
    if(CHECK_FLASH_XIP_ADDR(buf)) {
        tmp_buf = (char *)calloc(size, 1);
        if(tmp_buf == NULL) {
            free(tmp_key);
            return -1;
        }

        memcpy(tmp_buf, buf, size);
        buf = tmp_buf;
    }

    if(ef_set_env_blob(tmp_key, buf, size) == EF_NO_ERR) {
        ret = 0;
    }

    free(tmp_key);
    if(tmp_buf) {
        free(tmp_buf);
    }

    return ret;
}

int kvs_read(const char *key, void *buf, size_t size)
{
    int ret = 0;
    size_t required_size = 0;

    if(key == NULL || buf == NULL || size == 0)
        return -1;

    char *tmp_key = strdup(key);
    if(tmp_key == NULL) {
        return -1;
    }

    ef_get_env_blob(tmp_key, NULL, 0, &required_size);
    if(required_size == 0) {
        goto _return;
    }

    ret = ef_get_env_blob(tmp_key, buf, size, &required_size);

_return:
    free(tmp_key);

    return ret;
}

size_t kvs_get_value_size(const char *key)
{
    size_t required_size = 0;

    if(key == NULL)
        return 0;

    char *tmp_key = strdup(key);
    if(tmp_key == NULL) {
        return 0;
    }

    ef_get_env_blob(tmp_key, NULL, 0, &required_size);

    free(tmp_key);

    return required_size;
}

int kvs_delete(const char *key)
{
    int ret = -1;

    if(key == NULL)
        return -1;

    char *tmp_key = strdup(key);
    if(tmp_key == NULL) {
        return -1;
    }

    if(ef_del_env(tmp_key) == EF_NO_ERR) {
        ret = 0;
    }

    free(tmp_key);

    return ret;
}

