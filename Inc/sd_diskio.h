#ifndef __SD_DISKIO_H
#define __SD_DISKIO_H

/* Defines -------------------------------------------------------------------*/
#define BUS_4BITS       1                       /* Use 4B Wide Bus */
#define BLOCK_SIZE      512                     /* Block Size in Bytes */

/* SD card status codes */
#define SD_PRESENT          ((uint8_t)0x01)
#define SD_NOT_PRESENT      ((uint8_t)0x00)

/* Functions -----------------------------------------------------------------*/
uint8_t SD_isDetected(void);

#endif /* __SD_DISKIO_H */