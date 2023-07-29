#ifndef MAFS_H_
#define MAFS_H_

#include<stdint.h>


int mfClampInt(int num, int min, int max);
int64_t mfClampInt64(int64_t num, int64_t min, int64_t max);
int32_t mfClampInt32(int32_t num, int32_t min, int32_t max);
int16_t mfClampInt16(int16_t num, int16_t min, int16_t max);
int8_t mfClampInt8(int8_t num, int8_t min, int8_t max);

unsigned int mfClampUint(unsigned int num, unsigned int min, unsigned int max);
uint64_t mfClampUint64(uint64_t num, uint64_t min, uint64_t max);
uint32_t mfClampUint32(uint32_t num, uint32_t min, uint32_t max);
uint16_t mfClampUint16(uint16_t num, uint16_t min, uint16_t max);
uint8_t mfClampUint8(uint8_t num, uint8_t min, uint8_t max);

float mfClampFloat(float num, float min, float max);
double mfClampDouble(double num, double min, double max);
long double mfClampLongDouble(long double num, long double min, long double max);


int mfMaxInt(int a, int b);
int64_t mfMaxInt64(int64_t a, int64_t b);
int32_t mfMaxInt32(int32_t a, int32_t b);
int16_t mfMaxInt16(int16_t a, int16_t b);
int8_t mfMaxInt8(int8_t a, int8_t b);

unsigned int mfMaxUint(unsigned int a, unsigned int b);
uint64_t mfMaxUint64(uint64_t a, uint64_t b);
uint32_t mfMaxUint32(uint32_t a, uint32_t b);
uint16_t mfMaxUint16(uint16_t a, uint16_t b);
uint8_t mfMaxUint8(uint8_t a, uint8_t b);

float mfMaxFloat(float a, float b);
double mfMaxDouble(double a, double b);
long double mfMaxLongDouble(long double a, long double b);


int mfMinInt(int a, int b);
int64_t mfMinInt64(int64_t a, int64_t b);
int32_t mfMinInt32(int32_t a, int32_t b);
int16_t mfMinInt16(int16_t a, int16_t b);
int8_t mfMinInt8(int8_t a, int8_t b);

unsigned int mfMinUint(unsigned int a, unsigned int b);
uint64_t mfMinUint64(uint64_t a, uint64_t b);
uint32_t mfMinUint32(uint32_t a, uint32_t b);
uint16_t mfMinUint16(uint16_t a, uint16_t b);
uint8_t mfMinUint8(uint8_t a, uint8_t b);

float mfMinFloat(float a, float b);
double mfMinDouble(double a, double b);
long double mfMinLongDouble(long double a, long double b);


#endif