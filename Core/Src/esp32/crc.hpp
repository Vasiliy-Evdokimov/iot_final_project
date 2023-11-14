#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t crc8(uint8_t* array, uint8_t size);

#ifdef __cplusplus
}
#endif

#endif