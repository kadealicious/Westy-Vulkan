#include"h/mafs.h"

#define MAX_LOGIC (a >= b ? a : b)
#define MIN_LOGIC (a <= b ? a : b)

uint32_t mfClampUint32(uint32_t num, uint32_t min, uint32_t max)
{
	const uint32_t t = (num < min ? min : num);
	return (t > max ? max : t);
}

unsigned int mfMaxUint(unsigned int a, unsigned int b)
	{return MAX_LOGIC;}
uint64_t mfMaxUint64(uint64_t a, uint64_t b)
	{return MAX_LOGIC;}
uint32_t mfMaxUint32(uint32_t a, uint32_t b)
	{return MAX_LOGIC;}
uint16_t mfMaxUint16(uint16_t a, uint16_t b)
	{return MAX_LOGIC;}
uint8_t mfMaxUint8(uint8_t a, uint8_t b)
	{return MAX_LOGIC;}

uint32_t mfMinUint32(uint32_t a, uint32_t b)
	{return MIN_LOGIC;}