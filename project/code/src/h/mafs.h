#ifndef MAFS_H_
#define MAFS_H_

#include<stdint.h>


uint32_t mfClampUint32(uint32_t num, uint32_t min, uint32_t max);

unsigned int mfMaxUint(unsigned int a, unsigned int b);
uint64_t mfMaxUint64(uint64_t a, uint64_t b);
uint32_t mfMaxUint32(uint32_t a, uint32_t b);
uint16_t mfMaxUint16(uint16_t a, uint16_t b);
uint8_t mfMaxUint8(uint8_t a, uint8_t b);

uint32_t mfMinUint32(uint32_t a, uint32_t b);

#endif