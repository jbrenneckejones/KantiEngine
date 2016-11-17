#ifndef KANTI_PLATFORM

// #ifdef __cplusplus
// extern "C" {
// extern "C" {
// #endif


#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <float.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float real32;
typedef double real64;

typedef int32 bool32;

typedef size_t memory_index;

typedef intptr_t intptr;
typedef uintptr_t uintptr;

template <class T>
class KList;

// TODO(Julian): Implement this ourselves
#include <map>
#include <set>
#include <list>

template <typename T, typename ContainerType>
class KDictionary;

class KString;

class KVector2;

class KVector3;

class KVector4;

class KMatrix4x4;

class KQuaternion;

#define Real32Maximum FLT_MAX
#define Real32Minimum -FLT_MAX

#ifndef k_internal
#define k_internal static
#endif
#define local_persist static
#define global_variable static

#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#if KANTI_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif



#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
// TODO(Julian): swap, min, max ... macros???

#define AlignPow2(Value, Alignment) ((Value + ((Alignment) - 1)) & ~((Alignment) - 1))
#define Align4(Value) ((Value + 3) & ~3)
#define Align8(Value) ((Value + 7) & ~7)
#define Align16(Value) ((Value + 15) & ~15)

inline uint32
SafeTruncateUInt64(uint64 Value)
{
	// TODO(Julian): Defines for maximum values
	Assert(Value <= 0xFFFFFFFF);
	uint32 Result = (uint32)Value;
	return(Result);
}

inline uint16
SafeTruncateToU16(uint32 Value)
{
	// TODO(Julian): Defines for maximum values
	Assert(Value <= 0xFFFF);
	uint16 Result = (uint16)Value;
	return(Result);
}

#define KANTI_PLATFORM
#endif
