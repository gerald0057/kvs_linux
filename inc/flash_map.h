/*
 * flash_map.h for Linux - simulated MCU flash layout
 * Values derived from Kconfig settings.
 */

#ifndef __FLASH_MAP_H__
#define __FLASH_MAP_H__

#include <kvs_config.h>

#define SOC_INNER_FLASH_SIZE       (CONFIG_FLASH_SIZE_KB * 1024)
#define MAIN_FLASH_SIZE            SOC_INNER_FLASH_SIZE

#define USER_ENV_BANK_NUM          CONFIG_USER_ENV_BANK_NUM
#define KVS_ENV_BANK_NUM           CONFIG_KVS_ENV_BANK_NUM

#define FLASH_USER_ENV_SIZE        (USER_ENV_BANK_NUM * (CONFIG_EF_ERASE_MIN_SIZE))
#define FLASH_USER_ENV_ADDRESS     (MAIN_FLASH_SIZE - FLASH_USER_ENV_SIZE)

#define FLASH_KVS_ENV_SIZE         (KVS_ENV_BANK_NUM * (CONFIG_EF_ERASE_MIN_SIZE))
#define FLASH_KVS_ENV_ADDR        (MAIN_FLASH_SIZE - FLASH_USER_ENV_SIZE - FLASH_KVS_ENV_SIZE)

#endif /* __FLASH_MAP_H__ */
