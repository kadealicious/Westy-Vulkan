#include"h/mafs.h"

uint32_t mfClampInt(uint32_t num, uint32_t min, uint32_t max)	{const uint32_t t = num < min ? min : num; return t > max ? max : t;}	// Clamp function!  WHY DOESN'T C HAVE THIS BUILT IN
uint32_t mfMaxInt(uint32_t a, uint32_t b)						{return ((a >= b) ? a : b);}