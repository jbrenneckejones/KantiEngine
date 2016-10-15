#ifndef KANTI_INSTRINSICS

//
// TODO(Julian): Convert all of these to platform-efficient versions
// and remove math.h
//

#include "math.h"

inline real64
Epsilon()
{
	local_persist real64 MachineEpsilon = 1;

	while(1.0 + (MachineEpsilon / 2) > 1.0)
	{
		MachineEpsilon = MachineEpsilon / 2;
	}

	return MachineEpsilon;
}

inline int32
SignOf(int32 Value)
{
	int32 Result = (Value >= 0) ? 1 : -1;
	return(Result);
}

inline real32
SignOf(real32 Value)
{
	real32 Result = (Value >= 0) ? 1.0f : -1.0f;
	return(Result);
}

inline real32
SquareRoot(real32 Real32)
{
	real32 Result = sqrtf(Real32);
	return(Result);
}

inline real32
AbsoluteValue(real32 Real32)
{
	real32 Result = (real32)fabs(Real32);
	return(Result);
}

inline uint32
RotateLeft(uint32 Value, int32 Amount)
{
#if COMPILER_MSVC
	uint32 Result = _rotl(Value, Amount);
#else
	// TODO(casey): Actually port this to other compiler platforms!
	Amount &= 31;
	uint32 Result = ((Value << Amount) | (Value >> (32 - Amount)));
#endif

	return(Result);
}

inline uint32
RotateRight(uint32 Value, int32 Amount)
{
#if COMPILER_MSVC
	uint32 Result = _rotr(Value, Amount);
#else
	// TODO(casey): Actually port this to other compiler platforms!
	Amount &= 31;
	uint32 Result = ((Value >> Amount) | (Value << (32 - Amount)));
#endif

	return(Result);
}

inline int32
RoundReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)roundf(Real32);
	return(Result);
}

inline uint32
RoundReal32ToUInt32(real32 Real32)
{
	uint32 Result = (uint32)roundf(Real32);
	return(Result);
}

inline int32
FloorReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)floorf(Real32);
	return(Result);
}

inline int32
CeilReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)ceilf(Real32);
	return(Result);
}

inline int32
TruncateReal32ToInt32(real32 Real32)
{
	int32 Result = (int32)Real32;
	return(Result);
}

inline real32
Sin(real32 Angle)
{
	real32 Result = sinf(Angle);
	return (Result);
}

inline real32
ASin(real32 Angle)
{
	real32 Result = (real32)asin(Angle);
	return (Result);
}

inline real32
Cos(real32 Angle)
{
	real32 Result = cosf(Angle);
	return (Result);
}

inline real32
ACos(real32 Angle)
{
	real32 Result = (real32)acos(Angle);
	return (Result);
}

inline real32
Tan(real32 Angle)
{
	real32 Result = (real32)tan(Angle);
	return (Result);
}

inline real32
ATan2(real32 Y, real32 X)
{
	real32 Result = (real32)atan2f(Y, X);
	return (Result);
}

inline real32
Log(real32 Angle)
{
	real32 Result = (real32)log(Angle);
	return (Result);
}

inline real32
Exp(real32 Value)
{
	real32 Result = (real32)exp(Value);
	return (Result);
}

inline real32
Power(real32 X, real32 Y)
{
	real32 Result = (real32)pow(X, Y);
	return (Result);
}

inline real32
Max(real32 X, real32 Y)
{
	real32 Result = (X > Y) ? X : Y;

	return (Result);
}

inline real32
ToRadians(real32 Degrees)
{
	return (Degrees * 0.01745329251994329576923690768489f);
}

#define KANTI_INSTRINSICS
#endif // !KANTI_INSTRINSICS