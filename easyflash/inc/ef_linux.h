/*
 * EasyFlash Linux port header
 */

#ifndef EF_LINUX_H_
#define EF_LINUX_H_

#include <easyflash.h>

int ef_linux_init(const char *bin_path);
void ef_linux_deinit(void);
void ef_linux_set_offset(long offset);

#endif /* EF_LINUX_H_ */
