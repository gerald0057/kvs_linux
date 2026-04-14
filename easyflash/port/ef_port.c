/*
 * Linux port for EasyFlash - loads bin to RAM and simulates flash read
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyflash.h>
#include <stdarg.h>

#define ef_port_err(fmt, args...) printf("[EF-LINUX ERROR] " fmt, ##args)
#define ef_port_debug(fmt, args...) printf("[EF-LINUX DBG] " fmt, ##args)

static uint8_t *g_flash_ram = NULL;
static size_t g_flash_size = 0;

static int ef_linux_init(void)
{
    FILE *fp = fopen("ef.disk", "rb");
    if (fp == NULL)
    {
        ef_port_err("Failed to open rf disk\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    g_flash_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    g_flash_ram = (uint8_t *)malloc(g_flash_size);
    if (g_flash_ram == NULL)
    {
        ef_port_err("Failed to allocate %zu bytes for flash simulation\n", g_flash_size);
        fclose(fp);
        return -1;
    }

    size_t read_size = fread(g_flash_ram, 1, g_flash_size, fp);
    fclose(fp);

    if (read_size != g_flash_size)
    {
        ef_port_err("Failed to read full bin file: expected %zu, got %zu\n", g_flash_size, read_size);
        free(g_flash_ram);
        g_flash_ram = NULL;
        return -1;
    }

    ef_port_debug("Loaded %zu bytes into RAM for flash simulation\n", g_flash_size);
    return 0;
}

static const ef_env default_env_set[] = {
    {"boot_times", "0", 2},
};

EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size)
{
    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);
    ef_port_debug("start %08X end %08X\n", EF_START_ADDR, EF_START_ADDR + ENV_AREA_SIZE);
    return ef_linux_init() == 0 ? EF_NO_ERR : EF_ENV_INIT_FAILED;
}

EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size)
{
    ef_port_debug("Read called: addr=0x%x size=%zu\n", addr, size);

    if (g_flash_ram == NULL)
    {
        ef_port_err("Flash RAM not initialized\n");
        return EF_READ_ERR;
    }

    if ((addr < EF_START_ADDR) || (addr >= (EF_START_ADDR + ENV_AREA_SIZE)))
    {
        return EF_READ_ERR;
    }

    if ((addr + size) > (EF_START_ADDR + ENV_AREA_SIZE))
    {
        size = EF_START_ADDR + ENV_AREA_SIZE - addr;
    }

    memcpy(buf, g_flash_ram + addr, size);

    return EF_NO_ERR;
}

EfErrCode ef_port_erase(uint32_t addr, size_t size)
{
    ef_port_debug("Erase called: addr=0x%x size=%zu\n", addr, size);

    memset(g_flash_ram + addr, 0xFF, size);

    return EF_NO_ERR;
}

EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size)
{
    ef_port_debug("Write called: addr=0x%x size=%zu\n", addr, size);

    if ((addr < EF_START_ADDR) || (addr >= (EF_START_ADDR + ENV_AREA_SIZE)))
    {
        ef_port_debug("write error addr = 0x%x\n", addr);
        return EF_WRITE_ERR;
    }

    if ((addr + size) > (EF_START_ADDR + ENV_AREA_SIZE))
    {
        size = EF_START_ADDR + ENV_AREA_SIZE - addr;
    }

    memcpy(g_flash_ram + addr, buf, size);

    return EF_WRITE_ERR;
}

void ef_port_env_lock(void)
{
}

void ef_port_env_unlock(void)
{
}

void ef_log_debug(const char *file, long line, const char *format, ...)
{
    (void)(file);
    (void)(line);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void ef_log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void ef_print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
