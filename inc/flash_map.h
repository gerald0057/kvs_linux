/*
 * flash_map.h for Linux - simulates 256KB MCU flash layout
 */

#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#define SOC_INNER_FLASH_SIZE       (256 * 1024)   /* 256KB simulated flash */
#define MAIN_FLASH_SIZE            SOC_INNER_FLASH_SIZE

#define BOOTLOADER_SIZE            0x8000          /* 32KB bootloader */
#define FLASH_BOOTLOADER_SIZE      BOOTLOADER_SIZE
#define FLASH_BOOTLOADER_ADDR      0

#define BOOT_ENV_SIZE              (16 * 4096)     /* 64KB boot env */
#define FLASH_BOOT_ENV_SIZE        BOOT_ENV_SIZE
#define FLASH_BOOT_ENV_ADDR        (FLASH_BOOTLOADER_SIZE)

#define BUILD_KVS_ENABLE           1
#define USER_ENV_BANK_NUM          16              /* 16 * 4KB = 64KB */
#define KVS_ENV_BANK_NUM           2               /* 2 * 4KB = 8KB */

#define FLASH_USER_ENV_SIZE        (USER_ENV_BANK_NUM * 4096)
#define FLASH_USER_ENV_ADDR        (MAIN_FLASH_SIZE - FLASH_USER_ENV_SIZE)

#define FLASH_KVS_ENV_SIZE         (KVS_ENV_BANK_NUM * 4096)
#define FLASH_KVS_ENV_ADDR        (MAIN_FLASH_SIZE - FLASH_USER_ENV_SIZE - FLASH_KVS_ENV_SIZE)

#define FLASH_APP_TOTAL_SIZE      (MAIN_FLASH_SIZE - FLASH_BOOTLOADER_SIZE - FLASH_BOOT_ENV_SIZE - FLASH_KVS_ENV_SIZE - FLASH_USER_ENV_SIZE)
#define FLASH_APP_ADDR             (FLASH_BOOT_ENV_ADDR + FLASH_BOOT_ENV_SIZE)

#endif /* __FLASH_MAP_H__ */