#ifndef __FATFS_H
#define __FATFS_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

uint8_t FATFS_Init(void);
uint8_t FATFS_DeInit(void);

#ifdef __cplusplus
}
#endif
#endif /* __FATFS_H */
