#ifndef KANTI_MATH

// TODO(Julian): Convert all of these to platform-efficient versions
// and remove math.h
#include "math.h"

inline real64
Epsilon()
{
	local_persist real64 MachineEpsilon = 1;

	while (1.0 + (MachineEpsilon / 2) > 1.0)
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

inline int32
Power(int32 X, int32 Y)
{
	int32 Result = (int32)pow(X, Y);
	return (Result);
}

global_variable real32 const DEGREES_CONSTANT = Pi32 / 180.0f;

inline real32
ToRadians(real32 Degrees)
{
	real32 Result = Degrees * DEGREES_CONSTANT;

	return (Result);
}

global_variable real32 const RADIANS_CONSTANT = 180.0f / Pi32;

inline real32
ToDegrees(real32 Radians)
{
	real32 Result = Radians * RADIANS_CONSTANT;

	return (Result);
}

//
// Scalar operations
//

inline real32
Square(real32 A)
{
	real32 Result = A * A;

	return(Result);
}

inline real32
Sin01(real32 Time)
{
	real32 Result = Sin(Pi32 * Time);

	return(Result);
}

inline real32
Triangle01(real32 Time)
{
	real32 Result = 2.0f * Time;
	if(Result > 1.0f)
	{
		Result = 2.0f - Result;
	}

	return(Result);
}

inline real32
Lerp(real32 A, real32 Time, real32 B)
{
	real32 Result = (1.0f - Time) * A + Time * B;

	return(Result);
}

inline real32
Mix(real32 A, real32 Time, real32 B)
{
	real32 Result = (A)+Time * (B - A);

	return(Result);
}

inline real32
Clamp(real32 Min, real32 Value, real32 Max)
{
	real32 Result = Value;

	if(Result < Min)
	{
		Result = Min;
	}
	else if(Result > Max)
	{
		Result = Max;
	}

	return(Result);
}

inline real32
Clamp01(real32 Value)
{
	real32 Result = Clamp(0.0f, Value, 1.0f);

	return(Result);
}

inline real32
Clamp01MapToRange(real32 Min, real32 Time, real32 Max)
{
	real32 Result = 0.0f;

	real32 Range = Max - Min;
	if(Range != 0.0f)
	{
		Result = Clamp01((Time - Min) / Range);
	}

	return(Result);
}

inline real32
ClampAboveZero(real32 Value)
{
	real32 Result = (Value < 0) ? 0.0f : Value;
	return(Result);
}

inline real32
SafeRatioN(real32 Numerator, real32 Divisor, real32 N)
{
	real32 Result = N;

	if(Divisor != 0.0f)
	{
		Result = Numerator / Divisor;
	}

	return(Result);
}

inline real32
SafeRatio0(real32 Numerator, real32 Divisor)
{
	real32 Result = SafeRatioN(Numerator, Divisor, 0.0f);

	return(Result);
}

inline real32
SafeRatio1(real32 Numerator, real32 Divisor)
{
	real32 Result = SafeRatioN(Numerator, Divisor, 1.0f);

	return(Result);
}

inline real32
Max(real32 X, real32 Y)
{
	real32 Result = (X > Y) ? X : Y;

	return (Result);
}

uint32
Min(uint32 X, uint32 Y)
{
	uint32 Result = (X < Y) ? X : Y;

	return (Result);
}

bool32
IsFloatEqual(real32 A, real32 B, real32 Threshold = 1.0f / 8192.0f)
{
	return fabsf(A - B) < Threshold;
}

//
// NOTE(Julian): KList operations
//

// TODO(Julian): Maybe remove this
#include <initializer_list>

template <class T>
class KList
{
public:

	inline KList()
	{
		// Initialize(0);
	}

	inline KList(std::initializer_list<T> InitList)
	{
		Initialize(0);

		for (const T* Index = InitList.begin(); Index != InitList.end(); ++Index)
		{
			PushBack(*Index);
		}
	}

	inline KList(T* InitArray)
	{
		SetArray(InitArray, GetTypeCount(InitArray));
	}

	inline KList(const T* InitArray)
	{
		SetArray(InitArray, GetTypeCount(InitArray));
	}

	inline KList(T* InitArray, uint32 InitCount)
	{
		SetArray(InitArray, InitCount);
	}

	inline KList(const T* InitArray, uint32 InitCount)
	{
		SetArray(InitArray, InitCount);
	}

	template<uint32 Count>
	inline KList(T(&InitArray)[Count])
	{
		SetArray(*InitArray, Count);
	}

	template<uint32 Count>
	inline KList(const T(&InitArray)[Count])
	{
		SetArray(*InitArray, Count);
	}

	inline KList(uint32 Size)
	{
		Initialize(Size);
	}

	inline void ZeroArray()
	{
		for (uint32 Index = 0; Index < ElementMax; ++Index)
		{
			Array[Index] = {};
		}
	}

	inline void Clear()
	{
		for (uint32 Index = 0; Index < ElementMax; ++Index)
		{
			Array[Index] = nullptr;
		}
	}

	inline void Resize(uint32 Size)
	{
		if (!Array)
		{
			Initialize(0);
		}

		ElementMax = Size;
		MemorySize = sizeof(T) * ElementMax;
		Array = (T *)MemRealloc(Array, MemorySize, MemorySize, 8);

		ElementCount = Size;
	}

	inline void Set(uint32 Index, T Element)
	{
		if (!Array)
		{
			return;
		}

		Array[Index] = Element;
	}

	inline void PushBack(T Element)
	{
		if (ElementCount >= ElementMax)
		{
			Grow(GrowthAmount);
		}

		Array[ElementCount++] = Element;
	}

	template<uint32 Count>
	inline void PushBack(T(&ElementList)[Count])
	{
		uint32 ElementListCount = Count;

		uint32 LastCount = ElementCount;

		uint32 NewCount = LastCount + ElementListCount;

		if (NewCount > ElementMax)
		{
			Grow(ElementListCount);
		}

		uint32 ElementListIndex = 0;
		for (uint32 Index = LastCount; Index < NewCount; ++Index)
		{
			PushBack(ElementList[ElementListIndex++]);
		}
	}

	template<uint32 Count>
	inline void PushBack(const T(&ElementList)[Count])
	{
		uint32 ElementListCount = Count;

		uint32 LastCount = ElementCount;

		uint32 NewCount = LastCount + ElementListCount;

		if (NewCount > ElementMax)
		{
			Grow(ElementListCount);
		}

		uint32 ElementListIndex = 0;
		for (uint32 Index = LastCount; Index < NewCount; ++Index)
		{
			PushBack(ElementList[ElementListIndex++]);
		}
	}

	inline void PushBack(T* ElementList, uint32 Size)
	{
		Grow(Size);

		for (T* Element = ElementList; *Element; ++Element)
		{
			PushBack(*Element);
		}

		ElementCount = ElementCount + Size;
	}

	inline T& At(uint32 Index)
	{
		Assert(Index < ElementMax);

		return Array[Index];
	}

	inline uint32 Count()
	{
		return ElementCount;
	}

	inline uint32 MaxSize()
	{
		return ElementMax;
	}

	inline memory_index Memory()
	{
		return MemorySize;
	}

	inline KList<T> operator + (KList<T> Other)
	{
		KList<T> Result;
		Result.PushBack(Array, ElementCount);

		Result.PushBack(Other.Array, Other.Count());

		return (Result);
	}

	inline void operator += (KList<T> Other)
	{
		PushBack(Other.Array, Other.Count());
	}

	inline T& operator[](uint32 Index)
	{
		Assert(Index < ElementMax);

		return Array[Index];
	}

	inline const T& operator[](uint32 Index) const
	{
		Assert(Index < ElementMax);

		return Array[Index];
	}

	inline bool32 operator ==(KList<T> OtherList)
	{
		if (Array == OtherList.Array)
		{
			return true;
		}

		return false;
	}

	inline T* Data()
	{
		return Array;
	}

	inline T* begin()
	{
		if (Array)
		{
			return Array[0];
		}

		return nullptr;
	}

	inline T* end()
	{
		if (ElementMax > 0)
		{
			return Array[ElementMax - 1];
		}

		return begin();
	}

protected:

	inline uint32 GetTypeCount(T* InitArray)
	{
		uint32 Result = 0;
		for (T* Iterator = InitArray; *Iterator; ++Iterator)
		{
			Result++;
		}

		return Result;
	}

	inline uint32 GetTypeCount(const T* InitArray)
	{
		uint32 Result = 0;
		for (const T* Iterator = InitArray; *Iterator; ++Iterator)
		{
			Result++;
		}

		return Result;
	}

	inline virtual void Initialize(uint32 InitCount)
	{
		if (InitCount <= 0)
		{
			Array = (T *)MemAlloc(sizeof(T), 8);

			MemorySize = sizeof(T);

			ElementMax = 1;
		}
		else
		{
			uint32 Slack = InitCount + GrowthAmount;
			Array = (T *)MemAlloc(sizeof(T) * Slack, 8);

			ElementCount = InitCount;

			ElementMax = Slack;

			MemorySize = sizeof(T) * ElementMax;
		}
	}

	inline virtual void SetArray(T* InitArray, uint32 InitCount)
	{
		Initialize(InitCount);

		MemCopy(Array, InitArray, InitCount);
	}

	inline virtual void SetArray(const T* InitArray, uint32 InitCount)
	{
		Initialize(InitCount);

		MemCopy(Array, (void *)InitArray, InitCount);
	}

	inline virtual void SetArray(const T& InitArray, uint32 InitCount)
	{
		Initialize(InitCount);

		MemCopy(Array, (void *)&InitArray, InitCount);
	}

	inline void Grow(uint32 Growth, uint32 Slack = 10)
	{
		ElementMax += Growth + Slack;

		memory_index NewSize = sizeof(T) * ElementMax;

		if (!Array)
		{
			Array = (T *)MemAlloc(NewSize, 8);
		}
		else
		{
			Array = (T *)MemRealloc(Array, MemorySize, NewSize, 8);
		}

		MemorySize = NewSize;
	}

	uint32 GrowthAmount = 5;

	memory_index MemorySize = 0;

	uint32 ElementMax = 0;

	uint32 ElementCount = 0;

	T* Array = nullptr;
};


// TODO(Julian): Turn this into a hashmap and implement it
template <typename TKey, typename TValue>
class KDictionary
{

};

// KString

class KString : public KList<char>
{
public:

public:

	inline KString() : KList<char>()
	{
		CheckString();
	}

	inline KString(std::initializer_list<char> InitList) : KList<char>(InitList)
	{
		CheckString();
	}

	inline KString(char* InitArray) : KList<char>(InitArray)
	{
		CheckString();
	}

	inline KString(const char* InitArray) : KList<char>(InitArray)
	{
		CheckString();
	}

	inline KString(const char* InitArray, uint32 InitCount) : KList<char>(InitArray, InitCount)
	{
		CheckString();
	}

	inline KString(char* InitArray, uint32 InitCount) : KList<char>(InitArray, InitCount)
	{
		CheckString();
	}

	inline KString(char* InitArray, uint32 FirstIndex, uint32 Length) : KList<char>(InitArray, Length)
	{
		MemCopy(Data(), (void *)InitArray[FirstIndex], Length);
		CheckString();
	}

	inline KString(const char* InitArray, uint32 FirstIndex, uint32 Length) : KList<char>(InitArray, Length)
	{
		MemCopy(Data(), (void *)InitArray[FirstIndex], Length);
		CheckString();
	}

	template<uint32 Count>
	inline KString(char(&InitArray)[Count]) : KList<char>(InitArray, Count)
	{
		CheckString();
	}

	template<uint32 Count>
	inline KString(const char(&InitArray)[Count]) : KList<char>(InitArray, Count)
	{
		CheckString();
	}

	inline KString(uint32 Size) : KList<char>(Size)
	{
		CheckString();
	}

protected:

	inline void CheckString()
	{
		if (ElementCount == 0)
		{
			return;
		}

		if (Array[ElementCount] != '\0')
		{
			Set(ElementCount, '\0');
		}
	}

	inline virtual void Initialize(uint32 InitCount) override
	{
		KList<char>::Initialize(InitCount);

		CheckString();
	}

	inline virtual void SetArray(char* InitArray, uint32 InitCount) override
	{
		KList<char>::SetArray(InitArray, InitCount);

		CheckString();
	}

	inline virtual void SetArray(const char* InitArray, uint32 InitCount) override
	{
		KList<char>::SetArray(InitArray, InitCount);

		CheckString();
	}

	inline virtual void SetArray(const char& InitArray, uint32 InitCount) override
	{
		KList<char>::SetArray(InitArray, InitCount);

		CheckString();
	}

};

//
// NOTE(Julian): Vector2 operations
//

class KVector2
{
public:

	union
	{
		struct
		{
			real32 X, Y;
		};

		struct
		{
			real32 U, V;
		};

		real32 E[2];
	};

	const k_internal KVector2 Zero;
	const k_internal KVector2 One;
	const k_internal KVector2 Up;
	const k_internal KVector2 Down;
	const k_internal KVector2 Left;
	const k_internal KVector2 Right;


	inline KVector2()
	{
		this->X = 0;
		this->Y = 0;
	};

	inline KVector2(real32 Value)
	{
		this->X = Value;
		this->Y = Value;
	};

	inline KVector2(int32 X, int32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline KVector2(uint32 X, uint32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline KVector2(real32 X, real32 Y)
	{
		this->X = X;
		this->Y = Y;
	}

	inline KVector2 operator*(real32 A)
	{
		KVector2 Result = { this->X * A, this->Y * A };

		return (Result);
	}

	inline KVector2 operator*(KVector2 A)
	{
		KVector2 Result = { this->X * A.X, this->Y * A.Y };

		return (Result);
	}

	inline KVector2 operator*(const KVector2& A) const
	{
		KVector2 Result = { this->X * A.X, this->Y * A.Y };

		return (Result);
	}

	inline friend KVector2 operator*(real32 A, KVector2 B)
	{
		KVector2 Result = { A * B.X, A * B.Y };

		return(Result);
	}

	inline KVector2 operator*=(real32 A)
	{
		this->X *= A;
		this->Y *= A;
	}

	inline KVector2 operator-()
	{
		KVector2 Result = { -this->X, -this->Y };

		return(Result);
	}

	inline const KVector2& operator-() const
	{
		KVector2 Result = { -this->X, -this->Y };

		return(Result);
	}

	inline KVector2 operator+(KVector2 A)
	{
		KVector2 Result = {};

		Result = { this->X + A.X, this->Y + A.Y };

		return(Result);
	}

	inline void operator+=(KVector2 A)
	{
		this->X += A.X;
		this->Y += A.Y;
	}

	inline KVector2 operator-(KVector2 A)
	{
		KVector2 Result = { this->X - A.X, this->Y - A.Y };

		return(Result);
	}

	inline void operator-=(KVector2 A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
	}

	inline bool32 operator==(KVector2 A)
	{
		bool32 Result = X == A.X && Y == A.Y;

		return (Result);
	}

	inline bool32 operator==(const KVector2& A) const
	{
		bool32 Result = X == A.X && Y == A.Y;

		return (Result);
	}

	inline bool32 operator!=(KVector2 A)
	{
		bool32 Result = !(*this == A);

		return (Result);
	}

	inline bool32 operator!=(const KVector2& A) const
	{
		bool32 Result = !(*this == A);

		return (Result);
	}

	inline bool32 operator <(KVector2 A)
	{
		bool32 Result = this->X < A.X || (this->X == A.X && this->Y < A.Y);

		return (Result);
	}

	inline bool32 operator >(KVector2 A)
	{
		bool32 Result = this->X > A.X || (this->X == A.X && this->Y > A.Y);

		return (Result);
	}

	inline KVector2 operator/(real32 A)
	{
		KVector2 Result = { this->X / A, this->Y / A };

		return (Result);
	}

	inline KVector2 Perp()
	{
		KVector2 Result = { -this->Y, this->X };

		return (Result);
	}

	inline KVector2 Hadamard(KVector2 A)
	{
		KVector2 Result = { this->X * A.X, this->Y * A.Y };

		return(Result);
	}

	inline k_internal real32 Inner(KVector2 A, KVector2 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y;

		return(Result);
	}

	inline real32 Inner(KVector2 A)
	{
		real32 Result = Inner(*this, A);

		return(Result);
	}

	inline k_internal real32 LengthSq(KVector2 A)
	{
		real32 Result = Inner(A, A);

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = LengthSq(*this);

		return(Result);
	}

	inline k_internal real32 Length(KVector2 A)
	{

		real32 Result = SquareRoot(LengthSq(A));

		return(Result);

	}

	inline real32 Length()
	{
		real32 Result = Length(*this);

		return(Result);
	}

	inline KVector2 Normalize()
	{
		KVector2 Result = Normalize(*this);

		return(Result);
	}

	k_internal inline KVector2 Normalize(KVector2 A)
	{
		// Vector2 Result = A / Length(A);
		KVector2 Result = A;

		real32 Length = Result.X * Result.X + Result.Y * Result.Y;
		if (Length > 0.0f)
		{
			Length = SquareRoot(Length);
			real32 InverseLength = 1.0f / Length;
			Result.X *= InverseLength;
			Result.Y *= InverseLength;
		}

		return(Result);
	}

	inline KVector2 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		KVector2 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max) };

		return(Result);
	}

	k_internal inline KVector2 Arm2(real32 Angle)
	{
		KVector2 Result = { Cos(Angle), Sin(Angle) };

		return(Result);
	}

	k_internal inline real32 Distance(KVector2 A, KVector2 B)
	{
		real32 Result = SquareRoot(Square(A.X - B.X) + Square(A.Y - B.Y));

		return (Result);
	}

	inline real32 Distance(KVector2 A)
	{
		real32 Result = Distance(*this, A);

		return (Result);
	}

	inline real32 Distance(const KVector2& A) const
	{
		real32 Result = Distance(*this, A);

		return (Result);
	}

	inline k_internal KVector2 Min(KVector2 A, KVector2 B)
	{
		return A.Length() < B.Length() ? A : B;
	}

	inline k_internal KVector2 Max(KVector2 A, KVector2 B)
	{
		return A.Length() > B.Length() ? A : B;
	}

	inline k_internal real32 DotProduct(KVector2 A, KVector2 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y;

		return (Result);
	}

	inline k_internal KVector2 RotateVector(KVector2 A, real32 Degrees)
	{
		real32 Radians = ToRadians(Degrees);
		real32 Sine = Sin(Radians);
		real32 Cosine = Cos(Radians);

		KVector2 Result;
		Result.X = A.X * Cosine - A.Y * Sine;
		Result.Y = A.X * Sine + A.Y * Cosine;

		return (Result);
	}

	inline k_internal KVector2 Rotate90Degrees(KVector2 A)
	{
		KVector2 Result;

		Result.X = -A.Y;
		Result.Y = A.X;

		return (Result);
	}

	inline k_internal KVector2 UnitVector(KVector2 A)
	{
		real32 Length = A.Length();
		KVector2 Result = A;

		if (0.0f < Length)
		{
			return Result / Length;
		}

		return (Result);
	}

	inline k_internal bool32 IsVectorParallel(KVector2 A, KVector2 B)
	{
		KVector2 na = Rotate90Degrees(A);

		return IsFloatEqual(0.0f, DotProduct(na, B));
	}

	inline k_internal real32 Angle(KVector2 A, KVector2 B)
	{
		real32 DeltaY = B.Y - A.Y;
		real32	DeltaX = B.X - A.X;

		real32 AngleInDegrees = ATan2(DeltaY, DeltaX) * RADIANS_CONSTANT;

		return AngleInDegrees;

		return ATan2(B.Y, B.X) - ATan2(A.Y, A.X);

		real32 Dot = DotProduct(A, B);						// # dot product
		real32 Determinant = A.X * B.Y - A.Y * B.X;		   // # determinant
		real32 Result = ATan2(Determinant, Dot);		   // # atan2(y, x) or atan2(sin, cos)

		return (Result);
	}

	inline k_internal real32 EnclosedAngle(KVector2 A, KVector2 B)
	{
		KVector2 UnitA = KVector2::UnitVector(A);
		KVector2 UnitB = KVector2::UnitVector(B);

		real32 DotProduct = KVector2::DotProduct(UnitA, UnitB);

		real32 Result = ToDegrees(ACos(DotProduct));

		return (Result);
	}
};

const KVector2 KVector2::Zero = KVector2(0.0f, 0.0f);
const KVector2 KVector2::One = KVector2(1.0f, 1.0f);
const KVector2 KVector2::Up = KVector2(0.0f, -1.0f);
const KVector2 KVector2::Down = KVector2(0.0f, 1.0f);
const KVector2 KVector2::Left = KVector2(-1.0f, 0.0f);
const KVector2 KVector2::Right = KVector2(1.0f, 0.0f);

//
// NOTE(Julian): Vector3 operations
//

class KVector3
{
	public:

	union
	{
		struct
		{
			real32 X, Y, Z;
		};

		struct
		{
			real32 U, V, W;
		};

		struct
		{
			KVector2 XY;
			real32 Ignored2_;
		};

		struct
		{
			real32 Ignored3_;
			KVector2 YZ;
		};

		struct
		{
			KVector2 UV;
			real32 Ignored2_;
		};

		struct
		{
			real32 Ignored3_;
			KVector2 VW;
		};

		real32 E[3];
	};

	const k_internal KVector3 Zero;

	const k_internal KVector3 One;

	const k_internal KVector3 Up;

	const k_internal KVector3 Left;

	const k_internal KVector3 Forward;

	inline KVector3()
	{
		this->XY = 0;
		this->Z = 0;
	};

	inline KVector3(real32 Value)
	{
		this->XY = Value;
		this->Z = Value;
	};

	inline KVector3(real32 X, real32 Y, real32 Z)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
	}

	inline KVector3(int32 X, int32 Y, int32 Z)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
		this->Z = (real32)Z;
	}

	inline KVector3(uint32 X, uint32 Y, uint32 Z)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
		this->Z = (real32)Z;
	}

	inline KVector3(KVector2 XY, real32 Z)
	{
		this->X = XY.X;
		this->Y = XY.Y;
		this->Z = Z;
	}

	inline KVector3 operator*(real32 A)
	{
		KVector3 Result = { this->X * A, this->Y * A, this->Z * A };

		return(Result);
	}

	inline KVector3 operator*(KVector3 A)
	{
		KVector3 Result = { this->X * A.X, this->Y * A.Y, this->Z * A.Z };

		return(Result);
	}

	inline friend KVector3 operator*(real32 A, KVector3 B)
	{
		KVector3 Result = { A * B.X, A * B.Y, A * B.Z };

		return(Result);
	}

	inline KVector3 operator*=(real32 A)
	{
		this->X *= A;
		this->Y *= A;
		this->Z *= A;
	}

	inline KVector3 operator-()
	{
		KVector3 Result = { -this->X, -this->Y, -this->Z };

		return(Result);
	}

	inline KVector3 operator+(KVector3 A)
	{
		KVector3 Result = { this->X + A.X, this->Y + A.Y, this->Z + A.Z };

		return(Result);
	}

	inline KVector3& operator+=(KVector3 A)
	{
		this->X += A.X;
		this->Y += A.Y;
		this->Z += A.Z;

		return *this;
	}

	inline KVector3 operator-(KVector3 A)
	{
		KVector3 Result = { this->X - A.X, this->Y - A.Y, this->Z - A.Z };

		return(Result);
	}

	inline KVector3& operator-=(KVector3 A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
		this->Z -= A.Z;

		return *this;
	}

	inline friend KVector3 operator/(KVector3 A, real32 B)
	{
		KVector3 Result = {};

		Result.X = A.X / B;
		Result.Y = A.Y / B;
		Result.Z = A.X / B;

		return (Result);
	}

	inline KVector3 Hadamard(KVector3 A)
	{
		KVector3 Result = { this->X * A.X, this->Y * A.Y, this->Z * A.Z };

		return(Result);
	}

	k_internal inline KVector3 Hadamard(KVector3 A, KVector3 B)
	{
		KVector3 Result = { A.X * B.X, A.Y * B.Y, A.Z * B.Z };

		return(Result);
	}

	inline real32 Inner(KVector3 A)
	{
		real32 Result = this->X * A.X + this->Y * A.Y + this->Z * A.Z;

		return(Result);
	}

	k_internal inline real32 Inner(KVector3 A, KVector3 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = Inner(*this);

		return(Result);
	}

	k_internal inline real32 LengthSq(KVector3 A)
	{
		real32 Result = KVector3::Inner(A, A);

		return(Result);
	}

	inline real32 SqRoot()
	{
		real32 Result = SquareRoot(Inner(*this));

		return(Result);
	}

	k_internal inline real32 SqRoot(KVector3 A)
	{
		real32 Result = SquareRoot(KVector3::Inner(A, A));

		return(Result);
	}

	inline real32 InverseSqRoot()
	{
		real32 Result = 1.0f / SqRoot();

		return (Result);
	}

	k_internal inline real32 InverseSqRoot(KVector3 A)
	{
		real32 Result = 1.0f / SqRoot(A);

		return (Result);
	}

	inline real32 Length()
	{
		real32 Result = SquareRoot(LengthSq());

		return(Result);
	}

	k_internal inline real32 Length(KVector3 A)
	{
		real32 Result = SquareRoot(KVector3::LengthSq(A));

		return(Result);
	}

	inline KVector3 Normalize()
	{
		KVector3 Result = Normalize(*this);

		return(Result);
	}

	k_internal inline KVector3 Normalize(KVector3 A)
	{
		KVector3 Result = A / SqRoot(A);

		return(Result);
	}

	inline KVector3 NOZ()
	{
		KVector3 Result = { 0, 0, 0 };

		real32 LenSq = LengthSq();
		if(LenSq > Square(0.0001f))
		{
			Result = *this * (1.0f / SquareRoot(LenSq));
		}

		return(Result);
	}

	k_internal inline KVector3 NOZ(KVector3 A)
	{
		KVector3 Result = { 0, 0, 0 };

		real32 LenSq = KVector3::LengthSq(A);
		if(LenSq > Square(0.0001f))
		{
			Result = A * (1.0f / SquareRoot(LenSq));
		}

		return(Result);
	}

	inline KVector3 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		KVector3 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max) , Clamp(Min, this->Z, Max) };

		return(Result);
	}

	k_internal inline KVector3 Clamped(KVector3 A, real32 Min = 0.0f, real32 Max = 1.0f)
	{
		KVector3 Result = { Clamp(Min, A.X, Max), Clamp(Min, A.Y, Max) , Clamp(Min, A.Z, Max) };

		return(Result);
	}

	inline KVector3 Lerp(real32 Time, KVector3 A)
	{
		KVector3 Result = (1.0f - Time) * (*this) + Time * A;

		return(Result);
	}

	k_internal inline KVector3 Lerp(KVector3 A, real32 Time, KVector3 B)
	{
		KVector3 Result = (1.0f - Time) * A + Time * B;

		return(Result);
	}

	inline KVector3 Cross(KVector3 A)
	{
		KVector3 Result =
		{
			this->Y * A.Z - this->Y * A.Z,
			this->Y * A.Z - this->Y * A.Z,
			this->X * A.Y - this->X * A.Y
		};

		return (Result);
	}

	k_internal inline KVector3 Cross(KVector3 A, KVector3 B)
	{
		KVector3 Result =
		{
			A.Y * B.Z - B.Y * A.Z,
			A.Z * B.X - B.Z * A.X,
			A.X * B.Y - B.X * A.Y
		};

		return (Result);
	}

	inline real32 Dot(KVector3 A)
	{
		real32 Result = (this->X * A.X) + (this->Y * A.Y) + (this->Z + A.Z);

		return (Result);
	}

	k_internal inline real32 Dot(KVector3 A, KVector3 B)
	{
		real32 Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z + B.Z);

		return (Result);
	}

	k_internal inline KVector3 Cosine(KVector3 A)
	{
		KVector3 Result = { Cos(A.X), Cos(A.Y), Cos(A.Z) };

		return (Result);
	}

	k_internal inline KVector3 Sine(KVector3 A)
	{
		KVector3 Result = { Sin(A.X), Sin(A.Y), Sin(A.Z) };

		return (Result);
	}
};

const KVector3 KVector3::Zero = { 0.0f, 0.0f, 0.0f };

const KVector3 KVector3::One = { 1.0f, 1.0f, 1.0f };

const KVector3 KVector3::Up = { 0.0f, 1.0f, 0.0f };

const KVector3 KVector3::Left = { -1.0f, 0.0f, 0.0f };

const KVector3 KVector3::Forward = { 0.0f, 0.0f, 1.0f };

//
// NOTE(Julian): Vector4 operations
//

class KVector4
{
	public:

	union
	{
		struct
		{
			union
			{
				KVector3 XYZ;
				struct
				{
					real32 X, Y, Z;
				};
			};

			real32 W;
		};

		struct
		{
			union
			{
				KVector3 RGB;

				struct
				{
					real32 R, G, B;
				};
			};

			real32 A;
		};

		struct
		{
			KVector2 XY;
			real32 Ignored0_;
			real32 Ignored1_;
		};

		struct
		{
			real32 Ignored2_;
			KVector2 YZ;
			real32 Ignored3_;
		};

		struct
		{
			real32 Ignored4_;
			real32 Ignored5_;
			KVector2 ZW;
		};

		real32 E[4];
	};

	inline KVector4()
	{
		this->XYZ = 0;
		this->W = 0;
	}

	inline KVector4(real32 Value)
	{
		this->XYZ = Value;
		this->W = Value;
	}

	inline KVector4(real32 X, real32 Y, real32 Z, real32 W)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->W = W;
	}

	inline KVector4(KVector3 XYZ, real32 W)
	{
		this->XYZ = XYZ;
		this->W = W;
	}

	inline KVector4 operator*(real32 Value)
	{
		KVector4 Result = { this->X * Value, this->Y * Value, this->Z * Value, this->W * Value };

		return(Result);
	}

	inline KVector4 operator*(KVector4 Value)
	{
		KVector4 Result = { this->X * Value.X, this->Y * Value.Y, this->Z * Value.Z, this->W * Value.W };

		return(Result);
	}

	inline friend KVector4 operator*(real32 Value, KVector4 Vector)
	{
		KVector4 Result = { Value * Vector.X, Value * Vector.Y, Value * Vector.Z, Value * Vector.W };

		return(Result);
	}

	inline KVector4 operator*=(real32 Value)
	{
		this->X *= Value;
		this->Y *= Value;
		this->Z *= Value;
		this->W *= Value;
	}

	inline KVector4 operator-()
	{
		KVector4 Result = { -this->X, -this->Y, -this->Z, -this->W };

		return(Result);
	}

	inline KVector4 operator+(KVector4 Value)
	{
		KVector4 Result = { this->X + Value.X, this->Y + Value.Y, this->Z + Value.Z, this->W + Value.W };

		return(Result);
	}

	inline KVector4 operator+=(KVector4 Value)
	{
		this->X += Value.X;
		this->Y += Value.Y;
		this->Z += Value.Z;
		this->W += Value.W;
	}

	inline KVector4 operator-(KVector4 Value)
	{
		KVector4 Result = { this->X - Value.X, this->Y - Value.Y, this->Z - Value.Z, this->W - Value.W };

		return(Result);
	}

	inline KVector4 operator-=(KVector4 Value)
	{
		this->X -= Value.X;
		this->Y -= Value.Y;
		this->Z -= Value.Z;
		this->W -= Value.W;
	}

	inline KVector4 Hadamard(KVector4 Value)
	{
		KVector4 Result = { this->X * Value.X, this->Y * Value.Y, this->Z * Value.Z, this->W * Value.W };

		return(Result);
	}

	inline real32 Inner(KVector4 Value)
	{
		real32 Result = this->X * Value.X + this->Y * Value.Y + this->Z * Value.Z + this->W * Value.W;

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = Inner(*this);

		return(Result);
	}

	inline real32 Length()
	{
		real32 Result = SquareRoot(LengthSq());

		return(Result);
	}

	inline KVector4 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		KVector4 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max), Clamp(Min, this->Z, Max), Clamp(Min, this->W, Max) };

		return(Result);
	}

	KVector4 Lerp(real32 Time, KVector4 Value)
	{
		KVector4 Result = (1.0f - Time) * (*this) + Time * Value;

		return(Result);
	}
};

///
/// Matrices
///

// NOTE(Julian): Using a left handed coordinate system
class KMatrix4x4
{
	public:

	union
	{
		// Column major
		struct
		{
			real32 Row0[4];
			real32 Row1[4];
			real32 Row2[4];
			real32 Row3[4];
		};

		KVector4 E[4];

		real32 M[16];
	};

	KMatrix4x4()
	{
		this->E[0] = { 1, 0, 0, 0 };
		this->E[1] = { 0, 1, 0, 0 };
		this->E[2] = { 0, 0, 1, 0 };
		this->E[3] = { 0, 0, 0, 1 };
	}

	KMatrix4x4(real32 Value)
	{
		this->E[0] = { Value };
		this->E[1] = { Value };
		this->E[2] = { Value };
		this->E[3] = { Value };
	}

	KMatrix4x4(KVector4 X, KVector4 Y, KVector4 Z, KVector4 W)
	{
		this->E[0] = X;
		this->E[1] = Y;
		this->E[2] = Z;
		this->E[3] = W;
	}

	inline KMatrix4x4 operator*(real32 A)
	{
		KMatrix4x4 Result;

		Result.E[0] = this->E[0] * A;
		Result.E[0] = this->E[1] * A;
		Result.E[0] = this->E[2] * A;
		Result.E[0] = this->E[3] * A;

		return(Result);
	}

	inline KMatrix4x4 operator*(KMatrix4x4 A)
	{
		KMatrix4x4 Result = { 0.0f };

		Result.E[0] = this->E[0] * A.E[0].E[0] + this->E[1] * A.E[0].E[1] + this->E[2] * A.E[0].E[2] + this->E[3] * A.E[0].E[3];
		Result.E[1] = this->E[0] * A.E[1].E[0] + this->E[1] * A.E[1].E[1] + this->E[2] * A.E[1].E[2] + this->E[3] * A.E[1].E[3];
		Result.E[2] = this->E[0] * A.E[2].E[0] + this->E[1] * A.E[2].E[1] + this->E[2] * A.E[2].E[2] + this->E[3] * A.E[2].E[3];
		Result.E[3] = this->E[0] * A.E[3].E[0] + this->E[1] * A.E[3].E[1] + this->E[2] * A.E[3].E[2] + this->E[3] * A.E[3].E[3];

		return(Result);
	}

	inline friend KMatrix4x4 operator*(real32 A, KMatrix4x4 B)
	{
		KMatrix4x4 Result = A * B;

		return(Result);
	}

	inline KMatrix4x4 operator*(KVector3 A)
	{
		KMatrix4x4 Result = *this * A;

		return(Result);
	}

	inline KMatrix4x4 operator*(KVector4 A)
	{
		KMatrix4x4 Result =
		{
			this->E[0].E[0] * A.E[0] + this->E[0].E[1] * A.E[1] + this->E[0].E[2] * A.E[2] + this->E[0].E[3] * A.E[3],
			this->E[1].E[0] * A.E[0] + this->E[1].E[1] * A.E[1] + this->E[1].E[2] * A.E[2] + this->E[1].E[3] * A.E[3],
			this->E[2].E[0] * A.E[0] + this->E[2].E[1] * A.E[1] + this->E[2].E[2] * A.E[2] + this->E[2].E[3] * A.E[3],
			this->E[3].E[0] * A.E[0] + this->E[3].E[1] * A.E[1] + this->E[3].E[2] * A.E[2] + this->E[3].E[3] * A.E[3]
		};

		return(Result);
	}

	inline KMatrix4x4 operator*=(real32 A)
	{
		this->E[0] *= A;
		this->E[1] *= A;
		this->E[2] *= A;
		this->E[3] *= A;
	}

	inline KMatrix4x4 operator-()
	{
		KMatrix4x4 Result;

		Result.E[0] = -this->E[0];
		Result.E[1] = -this->E[1];
		Result.E[2] = -this->E[2];
		Result.E[3] = -this->E[3];

		return(Result);
	}

	inline KMatrix4x4 operator+(KMatrix4x4 A)
	{
		KMatrix4x4 Result;

		Result.E[0] = this->E[0] + A.E[0];
		Result.E[1] = this->E[1] + A.E[1];
		Result.E[2] = this->E[2] + A.E[2];
		Result.E[3] = this->E[3] + A.E[3];

		return(Result);
	}

	inline KMatrix4x4 operator+=(KMatrix4x4 A)
	{
		this->E[0] += A.E[0];
		this->E[1] += A.E[1];
		this->E[2] += A.E[2];
		this->E[3] += A.E[3];
	}

	inline KMatrix4x4 operator-(KMatrix4x4 A)
	{
		KMatrix4x4 Result;

		Result.E[0] = this->E[0] - A.E[0];
		Result.E[1] = this->E[1] - A.E[1];
		Result.E[2] = this->E[2] - A.E[2];
		Result.E[3] = this->E[3] - A.E[3];

		return(Result);
	}

	inline KMatrix4x4 operator-=(KMatrix4x4 A)
	{
		this->E[0] -= A.E[0];
		this->E[1] -= A.E[1];
		this->E[2] -= A.E[2];
		this->E[3] -= A.E[3];
	}

	k_internal KMatrix4x4 Identity(real32 A = 1.0f)
	{
		KMatrix4x4 Result = {};

		Result.E[0] = { A, 0, 0, 0 };
		Result.E[1] = { 0, A, 0, 0 };
		Result.E[2] = { 0, 0, A, 0 };
		Result.E[3] = { 0, 0, 0, A };

		return (Result);
	}

	k_internal inline KMatrix4x4 Perspective(real32 FOVY, real32 AspectRatio, real32 ZNear, real32 ZFar)
	{
		Assert(AbsoluteValue(AspectRatio - (real32)Epsilon()) > 0.0f);

		real32 TanHalfOfFOV = Tan(FOVY / 2.0f);

		KMatrix4x4 Result = { 0 };
		Result.E[0].E[0] = 1.0f / (AspectRatio * TanHalfOfFOV);
		Result.E[1].E[1] = 1.0f / (TanHalfOfFOV);
		Result.E[2].E[3] = 1.0f;

		Result.E[2].E[2] = ZFar / (ZFar - ZNear);
		Result.E[3].E[2] = -(ZFar * ZNear) / (ZFar - ZNear);

		return (Result);
	}

	k_internal inline KMatrix4x4 LookAt(KVector3 Eye, KVector3 Center, KVector3 Up)
	{
		KVector3 Front = KVector3::Normalize(Center - Eye);
		KVector3 See = KVector3::Normalize(KVector3::Cross(Up, Front));
		KVector3 Upward = KVector3::Cross(Front, See);

		KMatrix4x4 Result = KMatrix4x4::Identity(1.0f);

		Result.E[0].E[0] = See.X;
		Result.E[1].E[0] = See.Y;
		Result.E[2].E[0] = See.Z;
		Result.E[0].E[1] = Upward.X;
		Result.E[1].E[1] = Upward.Y;
		Result.E[2].E[1] = Upward.Z;
		Result.E[0].E[2] = Front.X;
		Result.E[1].E[2] = Front.Y;
		Result.E[2].E[2] = Front.Z;
		Result.E[3].E[0] = -KVector3::Dot(See, Eye);
		Result.E[3].E[1] = -KVector3::Dot(Upward, Eye);
		Result.E[3].E[2] = -KVector3::Dot(Front, Eye);

		return (Result);
	}

	k_internal inline KMatrix4x4 FPSView(KVector3 Eye, real32 Pitch, real32 Yaw)
	{
		// If the pitch and yaw angles are in degrees,
		// they need to be converted to radians. Here
		// I assume the values are already converted to radians.
		real32 CosPitch = Cos(Pitch);
		real32 SinPitch = Sin(Pitch);
		real32 CosYaw = Cos(Yaw);
		real32 SinYaw = Sin(Yaw);

		KVector3 XAxis = { CosYaw, 0.0f, -SinYaw };
		KVector3 YAxis = { SinYaw * SinPitch, CosPitch, CosYaw * SinPitch };
		KVector3 ZAxis = { SinYaw * CosPitch, -SinPitch, CosPitch * CosYaw };

		// Create a 4x4 view matrix from the right, up, forward and eye position vectors
		KMatrix4x4 ViewMatrix =
		{
			KVector4(XAxis.X,            YAxis.X,            ZAxis.X,      0.0f),
			KVector4(XAxis.Y,            YAxis.Y,            ZAxis.Y,      0.0f),
			KVector4(XAxis.Z,           YAxis.Z,            -ZAxis.Z,   0.0f),
			KVector4(-KVector3::Dot(XAxis, Eye), -KVector3::Dot(YAxis, Eye), -KVector3::Dot(ZAxis, Eye), 1)
		};

		return ViewMatrix;
	}

	inline void Rotate(real32 Angle, KVector3 Rotation)
	{
		real32 const ConstAngle = Angle;
		real32 const Cosine = Cos(ConstAngle);
		real32 const Sine = Sin(ConstAngle);

		KVector3 Axis = Rotation.Normalize();
		KVector3 TempVector = ((1.0f - Cosine) * Axis);

		KMatrix4x4 Rotate = KMatrix4x4(0.0f);
		Rotate.E[0].X = Cosine + TempVector.X * Axis.X;
		Rotate.E[0].Y = 0.0f + TempVector.X * Axis.Y + Sine * Axis.Z;
		Rotate.E[0].Z = 0.0f + TempVector.X * Axis.Z - Sine * Axis.Y;

		Rotate.E[1].X = 0.0f + TempVector.Y * Axis.X - Sine * Axis.Z;
		Rotate.E[1].Y = Cosine + TempVector.Y * Axis.Y;
		Rotate.E[1].Z = 0.0f + TempVector.Y * Axis.Z + Sine * Axis.X;

		Rotate.E[2].X = 0.0f + TempVector.Z * Axis.X + Sine * Axis.Y;
		Rotate.E[2].Y = 0.0f + TempVector.Z * Axis.Y - Sine * Axis.X;
		Rotate.E[2].Z = Cosine + TempVector.Z * Axis.Z;

		this->E[0] = this->E[0] * Rotate.E[0].X + this->E[1] * Rotate.E[0].Y + this->E[2] * Rotate.E[0].Z;
		this->E[1] = this->E[0] * Rotate.E[1].X + this->E[1] * Rotate.E[1].Y + this->E[2] * Rotate.E[1].Z;
		this->E[2] = this->E[0] * Rotate.E[2].X + this->E[1] * Rotate.E[2].Y + this->E[2] * Rotate.E[2].Z;
		this->E[3] = this->E[3];
	}

	k_internal inline KMatrix4x4 Rotate(KMatrix4x4 Matrix, real32 Angle, KVector3 Rotation)
	{
		real32 const ConstAngle = Angle;
		real32 const Cosine = Cos(ConstAngle);
		real32 const Sine = Sin(ConstAngle);

		KVector3 Axis = Rotation.Normalize();
		KVector3 TempVector = ((1.0f - Cosine) * Axis);

		KMatrix4x4 Rotate = KMatrix4x4(0.0f);
		Rotate.E[0].X = Cosine + TempVector.X * Axis.X;
		Rotate.E[0].Y = 0.0f + TempVector.X * Axis.Y + Sine * Axis.Z;
		Rotate.E[0].Z = 0.0f + TempVector.X * Axis.Z - Sine * Axis.Y;

		Rotate.E[1].X = 0.0f + TempVector.Y * Axis.X - Sine * Axis.Z;
		Rotate.E[1].Y = Cosine + TempVector.Y * Axis.Y;
		Rotate.E[1].Z = 0.0f + TempVector.Y * Axis.Z + Sine * Axis.X;

		Rotate.E[2].X = 0.0f + TempVector.Z * Axis.X + Sine * Axis.Y;
		Rotate.E[2].Y = 0.0f + TempVector.Z * Axis.Y - Sine * Axis.X;
		Rotate.E[2].Z = Cosine + TempVector.Z * Axis.Z;

		KMatrix4x4 Result = KMatrix4x4(0.0f);
		Result.E[0] = Matrix.E[0] * Rotate.E[0].X + Matrix.E[1] * Rotate.E[0].Y + Matrix.E[2] * Rotate.E[0].Z;
		Result.E[1] = Matrix.E[0] * Rotate.E[1].X + Matrix.E[1] * Rotate.E[1].Y + Matrix.E[2] * Rotate.E[1].Z;
		Result.E[2] = Matrix.E[0] * Rotate.E[2].X + Matrix.E[1] * Rotate.E[2].Y + Matrix.E[2] * Rotate.E[2].Z;
		Result.E[3] = Matrix.E[3];
		return (Result);
	}

	inline void Translate(KVector3 Position)
	{
		this->E[3] = this->E[0] * Position.E[0] + this->E[1] * Position.E[1] + this->E[2] * Position.E[2] + this->E[3];
	}

	k_internal inline KMatrix4x4 Translate(KMatrix4x4 Matrix, KVector3 Position)
	{
		Matrix.E[3] = Matrix.E[0] * Position.E[0] + Matrix.E[1] * Position.E[1] + Matrix.E[2] * Position.E[2] + Matrix.E[3];

		return (Matrix);
	}

	inline void Scale(KVector3 Scale)
	{
		this->E[0].E[0] *= Scale.E[0];
		this->E[1].E[1] *= Scale.E[1];
		this->E[2].E[2] *= Scale.E[2];
	}

	k_internal inline KMatrix4x4 Scale(KMatrix4x4 Matrix, KVector3 Scale)
	{
		Matrix.E[0] *= Scale.E[0];
		Matrix.E[1] *= Scale.E[1];
		Matrix.E[2] *= Scale.E[2];
	}
};

// NOTE(Julian): Quaternions

class KQuaternion
{
	public:

	union
	{
		struct
		{
			KVector4 Euler;
		};

		struct
		{
			real32 X, Y, Z, W;
		};

		real32 E[4];
	};

	inline KQuaternion()
	{
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
		W = 1.0f;
	}

	inline KQuaternion(KVector3 EulerAngle)
	{
		KVector3 C = KVector3::Cosine(EulerAngle * 0.5f);
		KVector3 S = KVector3::Sine(EulerAngle * 0.5f);

		this->W = C.X * C.Y * C.Z + S.X * S.Y * S.Z;
		this->X = S.X * C.Y * C.Z - C.X * S.Y * S.Z;
		this->Y = C.X * S.Y * C.Z + S.X * C.Y * S.Z;
		this->Z = C.X * C.Y * S.Z - S.X * S.Y * C.Z;
	}

	inline KQuaternion(real32 X, real32 Y, real32 Z, real32 W)
	{
		this->W = X;
		this->X = Y;
		this->Y = Z;
		this->Z = W;
	}

	inline KQuaternion operator+(const KQuaternion& Quat)
	{
		return Quat;
	}

	inline KQuaternion operator-()
	{
		KQuaternion Result = { -this->X, -this->Y, -this->Z, -this->W };;

		return (Result);
	}

	inline KQuaternion operator-(const KQuaternion& Quat)
	{
		KQuaternion Result = { this->X - Quat.X, this->Y - Quat.Y, this->Z - Quat.Z, this->W - Quat.W };

		return (Result);
	}

	inline KQuaternion operator*(real32 A)
	{
		KQuaternion Result = { this->X * A, this->Y * A, this->Z * A, this->W * A };

		return (Result);
	}

	inline KVector3 operator*(KVector3 Direction)
	{
		real32 num =   this->X * 2.0f;
		real32 num2 =  this->Y * 2.0f;
		real32 num3 =  this->Z * 2.0f;
		real32 num4 =  this->X * num;
		real32 num5 =  this->Y * num2;
		real32 num6 =  this->Z * num3;
		real32 num7 =  this->X * num2;
		real32 num8 =  this->X * num3;
		real32 num9 =  this->Y * num3;
		real32 num10 = this->W * num;
		real32 num11 = this->W * num2;
		real32 num12 = this->W * num3;
		KVector3 Result;

		Result.X = (1.0f - (num5 + num6)) * Direction.X + (num7 - num12) * Direction.Y + (num8 + num11) * Direction.Z;
		Result.Y = (num7 + num12) * Direction.X + (1.0f - (num4 + num6)) * Direction.Y + (num9 - num10) * Direction.Z;
		Result.Z = (num8 - num11) * Direction.X + (num9 + num10) * Direction.Y + (1.0f - (num4 + num5)) * Direction.Z;

		return (Result);
	}

	inline friend KQuaternion operator*(real32 A, KQuaternion B)
	{
		KQuaternion Result = B * A;

		return (Result);
	}

	inline friend KQuaternion operator/(KQuaternion A, real32 B)
	{
		KQuaternion Result = { A.X / B, A.Y / B, A.Z / B, A.W / B };

		return (Result);
	}

	inline k_internal friend KQuaternion operator*(KQuaternion Left, KVector4 Right)
	{
		KQuaternion Result = {};

		Result.W = (Left.W * Right.W) - (Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z);

		Result.X = (Left.W * Right.X) + (Left.X * Right.W) - (Left.Y * Right.Z) - (Left.Z * Right.Y);

		Result.Y = (Left.W * Right.Y) - (Left.Y * Right.W) - (Left.Z * Right.X) - (Left.X * Right.Z);

		Result.Z = (Left.W * Right.Z) - (Left.Z * Right.W) - (Left.X * Right.Y) - (Left.Y * Right.X);

		return(Result);
	}

	inline KQuaternion operator*(KQuaternion Quat)
	{
		KQuaternion Result = {};

		Result.W = (this->W * Quat.W) - (this->X * Quat.X) - (this->Y * Quat.Y) - (this->Z * Quat.Z);

		Result.X = (this->W * Quat.X) + (this->X * Quat.W) - (this->Y * Quat.Z) - (this->Z * Quat.Y);

		Result.Y = (this->W * Quat.Y) - (this->Y * Quat.W) - (this->Z * Quat.X) - (this->X * Quat.Z);

		Result.Z = (this->W * Quat.Z) - (this->Z * Quat.W) - (this->X * Quat.Y) - (this->Y * Quat.X);

		return(Result);
	}

	inline void operator*=(KQuaternion Quat)
	{
		*this = *this * Quat;
	}

	inline k_internal real32 Squared(KQuaternion Quat)
	{
		real32 Result = Square(Quat.X) + Square(Quat.Y) + Square(Quat.Z) + Square(Quat.W);

		return (Result);
	}

	inline k_internal KQuaternion Normalize(KQuaternion Quat)
	{
		real32 Magnitude = Squared(Quat);

		KQuaternion Result = { Quat.X / Magnitude, Quat.Y / Magnitude, Quat.Z / Magnitude, Quat.W / Magnitude };

		return (Result);
	}

	inline KQuaternion Normalize()
	{
		KQuaternion Result = Normalize(*this);

		return (Result);
	}

	inline k_internal KQuaternion Conjugate(KQuaternion Quat)
	{
		KQuaternion Result = { -Quat.X, -Quat.Y, -Quat.Z, Quat.W };

		return (Result);
	}

	inline KQuaternion Conjugate()
	{
		KQuaternion Result = Conjugate(*this);

		return (Result);
	}

	inline k_internal KQuaternion FromAxisAngle(KVector3 Axis, real32 Angle)
	{
		KQuaternion Result = {};

		Result.X = Axis.X * Sin(Angle / 2.0f);
		Result.Y = Axis.Y * Sin(Angle / 2.0f);
		Result.Z = Axis.Z * Sin(Angle / 2.0f);
		Result.W = Cos(Angle / 2.0f);

		return (Result);
	}

	inline k_internal KVector3 RotateThing(KQuaternion A, KVector3 B)
	{
		KQuaternion QuatVector = (B.X, B.Y, B.Z, 0.0f);

		KQuaternion Combined = A * QuatVector * A.Conjugate();

		KVector3 Result = (Combined.X, Combined.Y, Combined.Z);

		return (Result);
	}

	inline k_internal KQuaternion RotateTo(KVector3 A, KVector3 B)
	{
		KQuaternion Result = {};

		KVector3 Cross = A.Cross(B);

		// The product of the square of magnitudes and the cosine of the angle between from and to.
		real32 CosTheta = A.Dot(B);

		// Return identity if the vectors are the same direction.
		if(CosTheta >= 1.0f)
		{
			return (Result);
		}

		// The product of the square of the magnitudes
		real32 SquareMagnitude = SquareRoot(A.Dot(A) * B.Dot(B));

		// Return identity in the degenerate case.
		if(SquareMagnitude <= 0)
		{
			return (Result);
		}

		// Special handling for vectors facing opposite directions.
		if(CosTheta / SquareMagnitude <= -1.0f)
		{
			KVector3 XAxis(1, 0, 0);
			KVector3 YAxis(0, 1, 0);

			Cross = A.Cross(AbsoluteValue(A.Dot(XAxis)) < 1.0f ? XAxis : YAxis);
			SquareMagnitude = CosTheta = 0.0f;
		}

		Result = KQuaternion(Cross.X, Cross.Y, Cross.Z, SquareMagnitude + CosTheta);

		return (Result);
	}

	inline k_internal KQuaternion Rotate(KQuaternion A, real32 Angle, KVector3 View)
	{
		KVector3 TempVec = View;

		// Axis of rotation must be normalised
		real32 Length = TempVec.Length();

		if(AbsoluteValue(Length - 1.0f) > 0.001f)
		{
			real32 OneOverLength = 1.0f / Length;

			TempVec.X *= OneOverLength;
			TempVec.Y *= OneOverLength;
			TempVec.Z *= OneOverLength;
		}

		real32 const AngleRad(Angle);
		real32 const Sine = Sin(AngleRad * 0.5f);

		KQuaternion Result = A * KQuaternion(TempVec.X * Sine, TempVec.Y * Sine, TempVec.Z * Sine, Cos(AngleRad * 0.5f));

		return (Result);
		//return gtc::quaternion::cross(q, tquat<T, P>(cos(AngleRad * T(0.5)), Tmp.x * fSin, Tmp.y * fSin, Tmp.z * fSin));
	}

	inline void Rotate(real32 Angle, KVector3 View)
	{
		*this = Rotate(*this, Angle, View);
	}

	inline k_internal real32 Dot(KQuaternion A, KQuaternion B)
	{
		KVector4 TempVec(A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W);

		real32 Result = (TempVec.X + TempVec.Y) + (TempVec.Z + TempVec.W);

		return (Result);
	}

	inline k_internal KQuaternion MixQuat(KQuaternion A, real32 Value, KQuaternion B)
	{
		real32 CosTheta = KQuaternion::Dot(A, B);

		KQuaternion Result = {};

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if(CosTheta > 1.0f - Epsilon())
		{
			Result.X = Mix(A.W, Value, B.W);
			Result.Z = Mix(A.Y, Value, B.Y);
			Result.W = Mix(A.Z, Value, B.Z);
			Result.Y = Mix(A.X, Value, B.X);

			// Linear interpolation
			return (Result);
		}
		else
		{
			// Essential Mathematics, page 467
			real32 Angle = ACos(CosTheta);
			return (Sin((1.0f - Value) * Angle) * A + Sin(Value * Angle) * B) / Sin(Angle);
		}
	}

	/* Construct rotation matrix from (possibly non-unit) quaternion.
	* Assumes matrix is used to multiply column vector on the left:
	* vnew = mat vold. Works correctly for right-handed coordinate system
	* and right-handed rotations. */
	inline k_internal KMatrix4x4 ToMatrix(KQuaternion Quat)
	{

		// NOTE if inlined, then gcc 4.2 and 4.4 get rid of the temporary (not gcc 4.3 !!)
		// if not inlined then the cost of the return by value is huge ~ +35%,
		// however, not inlining this function is an order of magnitude slower, so
		// it has to be inlined, and so the return by value is not an issue
		KMatrix4x4 Result = {};

		const real32 tx = 2.0f * Quat.X;
		const real32 ty = 2.0f * Quat.Y;
		const real32 tz = 2.0f * Quat.Z;
		const real32 twx = tx *	 Quat.W;
		const real32 twy = ty *	 Quat.W;
		const real32 twz = tz *	 Quat.W;
		const real32 txx = tx *	 Quat.X;
		const real32 txy = ty *	 Quat.X;
		const real32 txz = tz *	 Quat.X;
		const real32 tyy = ty *	 Quat.Y;
		const real32 tyz = tz *	 Quat.Y;
		const real32 tzz = tz *	 Quat.Z;

		Result.E[0].E[0] = 1.0f - (tyy + tzz);
		Result.E[0].E[1] = txy - twz;
		Result.E[0].E[2] = txz + twy;
		Result.E[1].E[0] = txy + twz;
		Result.E[1].E[1] = 1.0f - (txx + tzz);
		Result.E[1].E[2] = tyz - twx;
		Result.E[2].E[0] = txz - twy;
		Result.E[2].E[1] = tyz + twx;
		Result.E[2].E[2] = 1.0f - (txx + tyy);

		return Result;
	}

	inline KMatrix4x4 ToMatrix()
	{
		KMatrix4x4 Result = ToMatrix(*this);

		return (Result);
	}

	inline k_internal KQuaternion FromMatrix(KMatrix4x4 Matrix)
	{
		KQuaternion Result = {};
		Result.W = SquareRoot(Max(0.0f, 1.0f + Matrix.E[0].E[0] + Matrix.E[1].E[1] + Matrix.E[2].E[2])) / 2.0f;
		Result.X = SquareRoot(Max(0.0f, 1.0f + Matrix.E[0].E[0] - Matrix.E[1].E[1] - Matrix.E[2].E[2])) / 2.0f;
		Result.Y = SquareRoot(Max(0.0f, 1.0f - Matrix.E[0].E[0] + Matrix.E[1].E[1] - Matrix.E[2].E[2])) / 2.0f;
		Result.Z = SquareRoot(Max(0.0f, 1.0f - Matrix.E[0].E[0] - Matrix.E[1].E[1] + Matrix.E[2].E[2])) / 2.0f;
		Result.X *= SignOf(Result.X * (Matrix.E[2].E[1] - Matrix.E[1].E[2]));
		Result.Y *= SignOf(Result.Y * (Matrix.E[0].E[2] - Matrix.E[2].E[0]));
		Result.Z *= SignOf(Result.Z * (Matrix.E[1].E[0] - Matrix.E[0].E[1]));

		return (Result);
	}

	inline k_internal KQuaternion Lerp(KQuaternion A, real32 Value, KQuaternion B)
	{
		// Lerp is only defined in [0, 1]
		Assert(Value >= 0.0f);
		Assert(Value <= 1.0f);

		KQuaternion Result = A * (1.0f - Value) + (B * Value);

		return (Result);
	}

	inline k_internal KQuaternion Slerp(KQuaternion A, real32 Value, KQuaternion B)
	{
		KQuaternion Current = B;

		real32 CosTheta = KQuaternion::Dot(A, B);

		KQuaternion Result = {};

		// If cosTheta < 0, the interpolation will take the long way around the sphere.
		// To fix this, one quat must be negated.
		if(CosTheta < 0.0f)
		{
			Current = -B;
			CosTheta = -CosTheta;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if(CosTheta > 1.0f - Epsilon())
		{
			Result.X = Mix(A.X, Value, Current.X);
			Result.Y = Mix(A.Y, Value, Current.Y);
			Result.Z = Mix(A.Z, Value, Current.Z);
			Result.W = Mix(A.W, Value, Current.W);

			// Linear interpolation
			return (Result);
		}
		else
		{
			// Essential Mathematics, page 467
			real32 Angle = ACos(CosTheta);

			Result = (Sin((1.0f - Value) * Angle) * A + Sin(Value * Angle) * Current) / Sin(Angle);

			return (Result);
		}
	}

	inline KQuaternion Slerp(real32 Value, KQuaternion B)
	{
		KQuaternion Result = Slerp(*this, Value, B);

		return(Result);
	}

	inline k_internal real32 Roll(KQuaternion Quat)
	{
		real32 A = 2.0f * (Quat.X * Quat.Y + Quat.W * Quat.Z);
		real32 B = Square(Quat.W) + Square(Quat.X) - Square(Quat.Y) - Square(Quat.Z);

		real32 Result = ATan2(A, B);

		return (Result);
	}

	inline k_internal real32 Pitch(KQuaternion Quat)
	{
		real32 A = 2.0f * (Quat.Y * Quat.Z + Quat.W * Quat.X);
		real32 B = Square(Quat.W) - Square(Quat.X) - Square(Quat.Y) + Square(Quat.Z);

		real32 Result = ATan2(A, B);

		return (Result);
	}

	inline k_internal real32 Yaw(KQuaternion Quat)
	{
		real32 A = -2.0f * (Quat.X * Quat.Z - Quat.W * Quat.Y);

		real32 Result = ASin(Clamp(-1.0f, A, 1.0f));

		return (Result);
	}

	inline k_internal KVector3 ToEuler(KQuaternion Quat)
	{
		KVector3 Result = {};

		Result.X = Pitch(Quat);
		Result.Y = Yaw(Quat);
		Result.Z = Roll(Quat);

		return (Result);
	}

	inline KVector3 ToEuler()
	{
		KVector3 Result = {};

		Result.X = Pitch(*this);
		Result.Y = Yaw(*this);
		Result.Z = Roll(*this);

		return (Result);
	}

};

struct UniqueID
{
	uint64  Data1;
	uint32	Data2;
	uint32	Data3;
	uint8	Data4[8];

	inline bool32 operator ==(const UniqueID& Comparer)
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool32 operator ==(const UniqueID& Comparer) const
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool32 operator ==(UniqueID& Comparer)
	{
		return  Data1 == Comparer.Data1 &&
			Data2 == Comparer.Data2 &&
			Data3 == Comparer.Data3 &&
			Data4 == Comparer.Data4;
	}

	inline bool operator <(const UniqueID& Comparer) const
	{

		bool Result = memcmp(this, &Comparer, sizeof(Comparer)) < 0;

		return (Result);
	}

	/*
	inline bool32 operator <(const UniqueID& Comparer) const
	{
	return  Data1 < Comparer.Data1 &&
	Data2 < Comparer.Data2 &&
	Data3 < Comparer.Data3 &&
	Data4 < Comparer.Data4;
	}
	*/


	inline bool32 operator <(UniqueID& Comparer)
	{
		bool Result = memcmp(this, &Comparer, sizeof(Comparer)) < 0;

		return (Result);
	}

	inline KString ToString()
	{
		return "UniqueID"; // k_string((uint32)Data1); // + "-" + k_string(Data2) + "-" + k_string(Data3);
	}
};

#define PLATFORM_GET_UUID(name) UniqueID name()
typedef PLATFORM_GET_UUID(platform_get_uuid);

global_variable platform_get_uuid* GetUUID;

#define KANTI_MATH
#endif // !KANTI_MATH