#ifndef GLOBAL_H_
#define GLOBAL_H_

uint32_t clamp(uint32_t num, uint32_t min, uint32_t max) {const uint32_t t = num < min ? min : num;return t > max ? max : t;}

#endif