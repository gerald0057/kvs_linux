#ifndef __FLASH_MAP_H
#define __FLASH_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAIN_FLASH_SIZE             (256 * 1024)

#define USER_ENV_BANK_NUM           (16) // BANK SIZE 4KB (0 - 15)
#define KVS_ENV_BANK_NUM            (2) // BANK SIZE 4KB

#define FLASH_USER_ENV_SIZE         (USER_ENV_BANK_NUM * 4096)
#define FLASH_USER_ENV_ADDR         (MAIN_FLASH_SIZE - FLASH_USER_ENV_SIZE)

#define FLASH_KVS_ENV_SIZE          (KVS_ENV_BANK_NUM * 4096)
#define FLASH_KVS_ENV_ADDR          (MAIN_FLASH_SIZE - FLASH_KVS_ENV_SIZE - FLASH_USER_ENV_SIZE)

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_MAP_H */
