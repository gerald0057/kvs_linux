/*
 * Linux port for EasyFlash - read from bin file
 */

#ifndef EF_PORT_H_
#define EF_PORT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define EF_USING_ENV
#define EF_ENV_VER_NUM  0
#define EF_ERASE_MIN_SIZE         (0x1000)
#define EF_WRITE_GRAN             (1)

/* Linux 模拟 flash 地址 */
#define EF_START_ADDR             0
#define ENV_AREA_SIZE             (8 * 1024)  /* 8KB for KVS */

#endif /* EF_PORT_H_ */
