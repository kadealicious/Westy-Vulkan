#include<stdio.h>
#include"h/mafs.h"



/* Mafs - Created by Kade Samson - https://www.github.com/kadealicious
	This library contains C standard-type math operations that are used 
	all the time.  Why isn't clamp a C standard library function? */



#define CLAMP_MIN_LOGIC (num < min ? min : num)
#define CLAMP_MAX_LOGIC (num > max ? max : num)
#define MIN_LOGIC (a <= b ? a : b)
#define MAX_LOGIC (a >= b ? a : b)


void printb(unsigned char value)
{
    for (int i = sizeof(char) * 7; i >= 0; i--)
    	{printf("%d", (value & (1 << i)) >> i );}
}


int mfClampInt(int num, int min, int max)											{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
int64_t mfClampInt64(int64_t num, int64_t min, int64_t max)							{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
int32_t mfClampInt32(int32_t num, int32_t min, int32_t max)							{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
int16_t mfClampInt16(int16_t num, int16_t min, int16_t max)							{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
int8_t mfClampInt8(int8_t num, int8_t min, int8_t max)								{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}

unsigned int mfClampUint(unsigned int num, unsigned int min, unsigned int max)		{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
uint64_t mfClampUint64(uint64_t num, uint64_t min, uint64_t max)					{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
uint32_t mfClampUint32(uint32_t num, uint32_t min, uint32_t max)					{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
uint16_t mfClampUint16(uint16_t num, uint16_t min, uint16_t max)					{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
uint8_t mfClampUint8(uint8_t num, uint8_t min, uint8_t max)							{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}

float mfClampFloat(float num, float min, float max)									{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
double mfClampDouble(double num, double min, double max)							{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}
long double mfClampLongDouble(long double num, long double min, long double max)	{num = CLAMP_MIN_LOGIC; num = CLAMP_MAX_LOGIC; return num;}



int mfMaxInt(int a, int b)									{return MAX_LOGIC;}
int64_t mfMaxInt64(int64_t a, int64_t b)					{return MAX_LOGIC;}
int32_t mfMaxInt32(int32_t a, int32_t b)					{return MAX_LOGIC;}
int16_t mfMaxInt16(int16_t a, int16_t b)					{return MAX_LOGIC;}
int8_t mfMaxInt8(int8_t a, int8_t b)						{return MAX_LOGIC;}

unsigned int mfMaxUint(unsigned int a, unsigned int b)		{return MAX_LOGIC;}
uint64_t mfMaxUint64(uint64_t a, uint64_t b)				{return MAX_LOGIC;}
uint32_t mfMaxUint32(uint32_t a, uint32_t b)				{return MAX_LOGIC;}
uint16_t mfMaxUint16(uint16_t a, uint16_t b)				{return MAX_LOGIC;}
uint8_t mfMaxUint8(uint8_t a, uint8_t b)					{return MAX_LOGIC;}

float mfMaxFloat(float a, float b)							{return MAX_LOGIC;}
double mfMaxDouble(double a, double b)						{return MAX_LOGIC;}
long double mfMaxLongDouble(long double a, long double b)	{return MAX_LOGIC;}



int mfMinInt(int a, int b)									{return MIN_LOGIC;}
int64_t mfMinInt64(int64_t a, int64_t b)					{return MIN_LOGIC;}
int32_t mfMinInt32(int32_t a, int32_t b)					{return MIN_LOGIC;}
int16_t mfMinInt16(int16_t a, int16_t b)					{return MIN_LOGIC;}
int8_t mfMinInt8(int8_t a, int8_t b)						{return MIN_LOGIC;}

unsigned int mfMinUint(unsigned int a, unsigned int b)		{return MIN_LOGIC;}
uint64_t mfMinUint64(uint64_t a, uint64_t b)				{return MIN_LOGIC;}
uint32_t mfMinUint32(uint32_t a, uint32_t b)				{return MIN_LOGIC;}
uint16_t mfMinUint16(uint16_t a, uint16_t b)				{return MIN_LOGIC;}
uint8_t mfMinUint8(uint8_t a, uint8_t b)					{return MIN_LOGIC;}

float mfMinFloat(float a, float b)							{return MIN_LOGIC;}
double mfMinDouble(double a, double b)						{return MIN_LOGIC;}
long double mfMinLongDouble(long double a, long double b)	{return MIN_LOGIC;}