#ifndef __CRC_H__
#define __CRC_H__

#include <kvs_config.h>

#if defined(CONFIG_USING_EXTERNAL_CRC32)
unsigned int crc32(unsigned int crc, const void *buf, unsigned int size);
#else
extern unsigned int gx_crc32(unsigned int crc, const void *p, unsigned int size);
static inline unsigned int crc32(unsigned int crc, const void *buf, unsigned int size)
{
    return gx_crc32(crc, buf, size);
}
#endif


#endif
