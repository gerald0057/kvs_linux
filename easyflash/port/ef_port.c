/*
 * Linux port for EasyFlash - read from bin file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyflash.h>
#include <stdarg.h>

static FILE *g_flash_file = NULL;
static const char *g_bin_path = NULL;
static long g_flash_offset = 0x3E000;

#define EF_LINUX_ERR(fmt, args...) printf("[EF-LINUX ERROR] " fmt, ##args)
#define EF_LINUX_DBG(fmt, args...) printf("[EF-LINUX DBG] " fmt, ##args)

int ef_linux_init(const char *bin_path) {
    g_bin_path = bin_path;
    g_flash_file = fopen(bin_path, "rb");
    if (g_flash_file == NULL) {
        EF_LINUX_ERR("Failed to open bin file: %s\n", bin_path);
        return -1;
    }
    EF_LINUX_DBG("Opened bin file: %s at offset 0x%lx\n", bin_path, g_flash_offset);
    return 0;
}

void ef_linux_deinit(void) {
    if (g_flash_file != NULL) {
        fclose(g_flash_file);
        g_flash_file = NULL;
    }
}

void ef_linux_set_offset(long offset) {
    g_flash_offset = offset;
}

static const ef_env default_env_set[] = {
    {"boot_times", "0"},
};

EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);
    return EF_NO_ERR;
}

EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    if (g_flash_file == NULL) {
        EF_LINUX_ERR("Flash file not opened\n");
        return EF_READ_ERR;
    }

    uint32_t flash_addr = g_flash_offset + addr;
    
    if (addr >= ENV_AREA_SIZE) {
        EF_LINUX_ERR("Read address out of range: 0x%x\n", addr);
        return EF_READ_ERR;
    }

    if (addr + size > ENV_AREA_SIZE) {
        size = ENV_AREA_SIZE - addr;
    }

    if (fseek(g_flash_file, flash_addr, SEEK_SET) != 0) {
        EF_LINUX_ERR("Seek to 0x%x failed\n", flash_addr);
        return EF_READ_ERR;
    }

    size_t read_size = fread(buf, 1, size, g_flash_file);
    if (read_size != size) {
        EF_LINUX_ERR("Read failed: expected %zu, got %zu\n", size, read_size);
        return EF_READ_ERR;
    }

    return EF_NO_ERR;
}

EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EF_LINUX_DBG("Erase called (no-op): addr=0x%x size=%zu\n", addr, size);
    return EF_NO_ERR;
}

EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EF_LINUX_DBG("Write called (no-op): addr=0x%x size=%zu\n", addr, size);
    return EF_NO_ERR;
}

void ef_port_env_lock(void) {
}

void ef_port_env_unlock(void) {
}

void ef_log_debug(const char *file, long line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void ef_log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void ef_print(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
