#ifndef __fatfs_H
#define __fatfs_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

/* Exported functions --------------------------------------------------------*/
uint8_t FATFS_Init(void);
uint8_t FATFS_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __fatfs_H */
