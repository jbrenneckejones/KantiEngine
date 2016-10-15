#ifndef KANTI_MATH

#include "KantiIntrinsics.h"

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

		for(const T* Index = InitList.begin(); Index != InitList.end(); ++Index)
		{
			PushBack(*Index);
		}
	}

	inline KList(const T* InitArray, uint32 InitCount)
	{
		SetArray(InitArray, InitCount);
	}

	inline KList(T* InitArray, uint32 InitCount)
	{
		SetArray(InitArray, InitCount);
	}

	template<uint32 N>
	inline KList(T(&InitArray)[N])
	{
		SetArray(*InitArray, N);
	}

	template<uint32 N>
	inline KList(const T(&InitArray)[N])
	{
		SetArray(*InitArray, N);
	}

	inline KList(uint32 Size)
	{
		Initialize(Size);
	}

	inline void ZeroArray()
	{
		for(uint32 Index = 0; Index < ElementMax; ++Index)
		{
			Array[Index] = {};
		}
	}

	inline void Resize(uint32 Size)
	{
		if(!Array)
		{
			Initialize(0);
		}

		ElementMax = Size;
		MemorySize = sizeof(T) * ElementMax;
		Array = (T *)MemRealloc(Array, MemorySize, MemorySize, 8);

		ElementCount = Size;
	}

	inline void PushBack(T Element)
	{
		if(ElementCount >= ElementMax)
		{
			Grow(1);
		}

		Array[ElementCount++] = Element;
	}

	template<uint32 N>
	inline void PushBack(T(&ElementList)[N])
	{
		uint32 ElementListCount = N;

		uint32 LastCount = ElementCount;

		uint32 NewCount = LastCount + ElementListCount;

		if(NewCount > ElementMax)
		{
			Grow(ElementListCount);
		}

		uint32 ElementListIndex = 0;
		for(uint32 Index = LastCount; Index < NewCount; ++Index)
		{
			PushBack(ElementList[ElementListIndex++]);
		}
	}

	template<uint32 N>
	inline void PushBack(const T(&ElementList)[N])
	{
		uint32 ElementListCount = N;

		uint32 LastCount = ElementCount;

		uint32 NewCount = LastCount + ElementListCount;

		if(NewCount > ElementMax)
		{
			Grow(ElementListCount);
		}

		uint32 ElementListIndex = 0;
		for(uint32 Index = LastCount; Index < NewCount; ++Index)
		{
			PushBack(ElementList[ElementListIndex++]);
		}
	}

	inline void PushBack(T* ElementList, uint32 Size)
	{
		Grow(Size);

		for(T* Element = ElementList; *Element; ++Element)
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

	inline T& operator[](uint32 Index)
	{
		Assert(Index < ElementMax);

		return Array[Index];
	}

	const T& operator[](uint32 Index) const
	{
		Assert(Index < ElementMax);

		return Array[Index];
	}

	inline bool32 operator==(KList<T> OtherList)
	{
		if(Array == OtherList.Array)
		{
			return true;
		}

		return false;
	}

	inline T* Data()
	{
		return Array;
	}

	private:

	inline void Initialize(uint32 InitCount)
	{
		if(InitCount <= 0)
		{
			Array = (T *)MemAlloc(sizeof(T), 8);

			MemorySize = sizeof(T);

			ElementMax = 1;
		}
		else
		{
			uint32 Slack = InitCount;
			Array = (T *)MemAlloc(sizeof(T) * Slack, 8);

			ElementCount = InitCount;

			ElementMax = Slack;

			MemorySize = sizeof(T) * ElementMax;
		}
	}

	inline void SetArray(T* InitArray, uint32 InitCount)
	{
		Initialize(InitCount);

		MemCopy(Array, InitArray, InitCount);
	}

	inline void SetArray(const T& InitArray, uint32 InitCount)
	{
		Initialize(InitCount);

		MemCopy(Array, (void *)&InitArray, InitCount);
	}

	inline void Grow(uint32 Growth, uint32 Slack = 10)
	{
		ElementMax += Growth + Slack;

		memory_index NewSize = sizeof(T) * ElementMax;

		if(!Array)
		{
			Array = (T *)MemAlloc(NewSize, 8);
		}
		else
		{
			Array = (T *)MemRealloc(Array, MemorySize, NewSize, 8);
		}

		MemorySize = NewSize;
	}

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

//
// NOTE(Julian): Vector2 operations
//

class Vector2
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

	inline Vector2()
	{
		this->X = 0;
		this->Y = 0;
	};

	inline Vector2(real32 Value)
	{
		this->X = Value;
		this->Y = Value;
	};

	inline Vector2(int32 X, int32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline Vector2(uint32 X, uint32 Y)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
	}

	inline Vector2(real32 X, real32 Y)
	{
		this->X = X;
		this->Y = Y;
	}

	inline Vector2 operator*(real32 A)
	{
		Vector2 Result = { this->X * A, this->Y * A };

		return (Result);
	}

	inline Vector2 operator*(Vector2 A)
	{
		Vector2 Result = { this->X * A.X, this->Y * A.Y };

		return (Result);
	}

	inline friend Vector2 operator*(real32 A, Vector2 B)
	{
		Vector2 Result = { A * B.X, A * B.Y };

		return(Result);
	}

	inline Vector2 operator*=(real32 A)
	{
		this->X *= A;
		this->Y *= A;
	}

	inline Vector2 operator-()
	{
		Vector2 Result = { -this->X, -this->Y };

		return(Result);
	}

	inline Vector2 operator+(Vector2 A)
	{
		Vector2 Result = { this->X + A.X, this->Y + A.Y };

		return(Result);
	}

	inline Vector2 operator+=(Vector2 A)
	{
		this->X += A.X;
		this->Y += A.Y;
	}

	inline Vector2 operator-(Vector2 A)
	{
		Vector2 Result = { this->X - A.X, this->Y - A.Y };

		return(Result);
	}

	inline Vector2 operator-=(Vector2 A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
	}

	inline Vector2 operator/(real32 A)
	{
		Vector2 Result = { this->X / A, this->Y / A };

		return (Result);
	}

	inline Vector2 Perp()
	{
		Vector2 Result = { -this->Y, this->X };

		return (Result);
	}

	inline Vector2 Hadamard(Vector2 A)
	{
		Vector2 Result = { this->X * A.X, this->Y * A.Y };

		return(Result);
	}

	inline k_internal real32 Inner(Vector2 A, Vector2 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y;

		return(Result);
	}

	inline real32 Inner(Vector2 A)
	{
		real32 Result = Inner(*this, A);

		return(Result);
	}

	inline k_internal real32 LengthSq(Vector2 A)
	{
		real32 Result = Inner(A, A);

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = LengthSq(*this);

		return(Result);
	}

	inline k_internal real32 Length(Vector2 A)
	{

		real32 Result = SquareRoot(LengthSq(A));

		return(Result);
		
	}

	inline real32 Length()
	{
		real32 Result = Length(*this);

		return(Result);
	}

	inline Vector2 Normalize()
	{
		Vector2 Result = Normalize(*this);

		return(Result);
	}

	k_internal inline Vector2 Normalize(Vector2 A)
	{
		Vector2 Result = A / Length(A);

		return(Result);
	}

	inline Vector2 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		Vector2 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max) };

		return(Result);
	}

	k_internal inline Vector2 Arm2(real32 Angle)
	{
		Vector2 Result = { Cos(Angle), Sin(Angle) };

		return(Result);
	}
};

//
// NOTE(Julian): Vector3 operations
//

class Vector3
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
			Vector2 XY;
			real32 Ignored2_;
		};

		struct
		{
			real32 Ignored3_;
			Vector2 YZ;
		};

		struct
		{
			Vector2 UV;
			real32 Ignored2_;
		};

		struct
		{
			real32 Ignored3_;
			Vector2 VW;
		};

		real32 E[3];
	};

	inline Vector3()
	{
		this->XY = 0;
		this->Z = 0;
	};

	inline Vector3(real32 Value)
	{
		this->XY = Value;
		this->Z = Value;
	};

	inline Vector3(real32 X, real32 Y, real32 Z)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
	}

	inline Vector3(int32 X, int32 Y, int32 Z)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
		this->Z = (real32)Z;
	}

	inline Vector3(uint32 X, uint32 Y, uint32 Z)
	{
		this->X = (real32)X;
		this->Y = (real32)Y;
		this->Z = (real32)Z;
	}

	inline Vector3(Vector2 XY, real32 Z)
	{
		this->X = XY.X;
		this->Y = XY.Y;
		this->Z = Z;
	}

	inline k_internal Vector3 Up()
	{
		Vector3 Result = { 0.0f, 1.0f, 0.0f };

		return (Result);
	}

	inline k_internal Vector3 Left()
	{
		Vector3 Result = { -1.0f, 0.0f, 0.0f };

		return (Result);
	}

	inline k_internal Vector3 Forward()
	{
		Vector3 Result = { 0.0f, 0.0f, 1.0f };

		return (Result);
	}

	inline Vector3 operator*(real32 A)
	{
		Vector3 Result = { this->X * A, this->Y * A, this->Z * A };

		return(Result);
	}

	inline Vector3 operator*(Vector3 A)
	{
		Vector3 Result = { this->X * A.X, this->Y * A.Y, this->Z * A.Z };

		return(Result);
	}

	inline friend Vector3 operator*(real32 A, Vector3 B)
	{
		Vector3 Result = { A * B.X, A * B.Y, A * B.Z };

		return(Result);
	}

	inline Vector3 operator*=(real32 A)
	{
		this->X *= A;
		this->Y *= A;
		this->Z *= A;
	}

	inline Vector3 operator-()
	{
		Vector3 Result = { -this->X, -this->Y, -this->Z };

		return(Result);
	}

	inline Vector3 operator+(Vector3 A)
	{
		Vector3 Result = { this->X + A.X, this->Y + A.Y, this->Z + A.Z };

		return(Result);
	}

	inline Vector3& operator+=(Vector3 A)
	{
		this->X += A.X;
		this->Y += A.Y;
		this->Z += A.Z;

		return *this;
	}

	inline Vector3 operator-(Vector3 A)
	{
		Vector3 Result = { this->X - A.X, this->Y - A.Y, this->Z - A.Z };

		return(Result);
	}

	inline Vector3& operator-=(Vector3 A)
	{
		this->X -= A.X;
		this->Y -= A.Y;
		this->Z -= A.Z;

		return *this;
	}

	inline friend Vector3 operator/(Vector3 A, real32 B)
	{
		Vector3 Result = {};

		Result.X = A.X / B;
		Result.Y = A.Y / B;
		Result.Z = A.X / B;

		return (Result);
	}

	inline Vector3 Hadamard(Vector3 A)
	{
		Vector3 Result = { this->X * A.X, this->Y * A.Y, this->Z * A.Z };

		return(Result);
	}

	k_internal inline Vector3 Hadamard(Vector3 A, Vector3 B)
	{
		Vector3 Result = { A.X * B.X, A.Y * B.Y, A.Z * B.Z };

		return(Result);
	}

	inline real32 Inner(Vector3 A)
	{
		real32 Result = this->X * A.X + this->Y * A.Y + this->Z * A.Z;

		return(Result);
	}

	k_internal inline real32 Inner(Vector3 A, Vector3 B)
	{
		real32 Result = A.X * B.X + A.Y * B.Y + A.Z * B.Z;

		return(Result);
	}

	inline real32 LengthSq()
	{
		real32 Result = Inner(*this);

		return(Result);
	}

	k_internal inline real32 LengthSq(Vector3 A)
	{
		real32 Result = Vector3::Inner(A, A);

		return(Result);
	}

	inline real32 SqRoot()
	{
		real32 Result = SquareRoot(Inner(*this));

		return(Result);
	}

	k_internal inline real32 SqRoot(Vector3 A)
	{
		real32 Result = SquareRoot(Vector3::Inner(A, A));

		return(Result);
	}

	inline real32 InverseSqRoot()
	{
		real32 Result = 1.0f / SqRoot();

		return (Result);
	}

	k_internal inline real32 InverseSqRoot(Vector3 A)
	{
		real32 Result = 1.0f / SqRoot(A);

		return (Result);
	}

	inline real32 Length()
	{
		real32 Result = SquareRoot(LengthSq());

		return(Result);
	}

	k_internal inline real32 Length(Vector3 A)
	{
		real32 Result = SquareRoot(Vector3::LengthSq(A));

		return(Result);
	}

	inline Vector3 Normalize()
	{
		Vector3 Result = Normalize(*this);

		return(Result);
	}

	k_internal inline Vector3 Normalize(Vector3 A)
	{
		Vector3 Result = A / SqRoot(A);

		return(Result);
	}

	inline Vector3 NOZ()
	{
		Vector3 Result = { 0, 0, 0 };

		real32 LenSq = LengthSq();
		if(LenSq > Square(0.0001f))
		{
			Result = *this * (1.0f / SquareRoot(LenSq));
		}

		return(Result);
	}

	k_internal inline Vector3 NOZ(Vector3 A)
	{
		Vector3 Result = { 0, 0, 0 };

		real32 LenSq = Vector3::LengthSq(A);
		if(LenSq > Square(0.0001f))
		{
			Result = A * (1.0f / SquareRoot(LenSq));
		}

		return(Result);
	}

	inline Vector3 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		Vector3 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max) , Clamp(Min, this->Z, Max) };

		return(Result);
	}

	k_internal inline Vector3 Clamped(Vector3 A, real32 Min = 0.0f, real32 Max = 1.0f)
	{
		Vector3 Result = { Clamp(Min, A.X, Max), Clamp(Min, A.Y, Max) , Clamp(Min, A.Z, Max) };

		return(Result);
	}

	inline Vector3 Lerp(real32 Time, Vector3 A)
	{
		Vector3 Result = (1.0f - Time) * (*this) + Time * A;

		return(Result);
	}

	k_internal inline Vector3 Lerp(Vector3 A, real32 Time, Vector3 B)
	{
		Vector3 Result = (1.0f - Time) * A + Time * B;

		return(Result);
	}

	inline Vector3 Cross(Vector3 A)
	{
		Vector3 Result =
		{
			this->Y * A.Z - this->Y * A.Z,
			this->Y * A.Z - this->Y * A.Z,
			this->X * A.Y - this->X * A.Y
		};

		return (Result);
	}

	k_internal inline Vector3 Cross(Vector3 A, Vector3 B)
	{
		Vector3 Result =
		{
			A.Y * B.Z - B.Y * A.Z,
			A.Z * B.X - B.Z * A.X,
			A.X * B.Y - B.X * A.Y
		};

		return (Result);
	}

	inline real32 Dot(Vector3 A)
	{
		real32 Result = (this->X * A.X) + (this->Y * A.Y) + (this->Z + A.Z);

		return (Result);
	}

	k_internal inline real32 Dot(Vector3 A, Vector3 B)
	{
		real32 Result = (A.X * B.X) + (A.Y * B.Y) + (A.Z + B.Z);

		return (Result);
	}

	k_internal inline Vector3 Cosine(Vector3 A)
	{
		Vector3 Result = { Cos(A.X), Cos(A.Y), Cos(A.Z) };

		return (Result);
	}

	k_internal inline Vector3 Sine(Vector3 A)
	{
		Vector3 Result = { Sin(A.X), Sin(A.Y), Sin(A.Z) };

		return (Result);
	}
};

//
// NOTE(Julian): Vector4 operations
//

class Vector4
{
	public:

	union
	{
		struct
		{
			union
			{
				Vector3 XYZ;
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
				Vector3 RGB;

				struct
				{
					real32 R, G, B;
				};
			};

			real32 A;
		};

		struct
		{
			Vector2 XY;
			real32 Ignored0_;
			real32 Ignored1_;
		};

		struct
		{
			real32 Ignored2_;
			Vector2 YZ;
			real32 Ignored3_;
		};

		struct
		{
			real32 Ignored4_;
			real32 Ignored5_;
			Vector2 ZW;
		};

		real32 E[4];
	};

	inline Vector4()
	{
		this->XYZ = 0;
		this->W = 0;
	}

	inline Vector4(real32 Value)
	{
		this->XYZ = Value;
		this->W = Value;
	}

	inline Vector4(real32 X, real32 Y, real32 Z, real32 W)
	{
		this->X = X;
		this->Y = Y;
		this->Z = Z;
		this->W = W;
	}

	inline Vector4(Vector3 XYZ, real32 W)
	{
		this->XYZ = XYZ;
		this->W = W;
	}

	inline Vector4 operator*(real32 Value)
	{
		Vector4 Result = { this->X * Value, this->Y * Value, this->Z * Value, this->W * Value };

		return(Result);
	}

	inline Vector4 operator*(Vector4 Value)
	{
		Vector4 Result = { this->X * Value.X, this->Y * Value.Y, this->Z * Value.Z, this->W * Value.W };

		return(Result);
	}

	inline friend Vector4 operator*(real32 Value, Vector4 Vector)
	{
		Vector4 Result = { Value * Vector.X, Value * Vector.Y, Value * Vector.Z, Value * Vector.W };

		return(Result);
	}

	inline Vector4 operator*=(real32 Value)
	{
		this->X *= Value;
		this->Y *= Value;
		this->Z *= Value;
		this->W *= Value;
	}

	inline Vector4 operator-()
	{
		Vector4 Result = { -this->X, -this->Y, -this->Z, -this->W };

		return(Result);
	}

	inline Vector4 operator+(Vector4 Value)
	{
		Vector4 Result = { this->X + Value.X, this->Y + Value.Y, this->Z + Value.Z, this->W + Value.W };

		return(Result);
	}

	inline Vector4 operator+=(Vector4 Value)
	{
		this->X += Value.X;
		this->Y += Value.Y;
		this->Z += Value.Z;
		this->W += Value.W;
	}

	inline Vector4 operator-(Vector4 Value)
	{
		Vector4 Result = { this->X - Value.X, this->Y - Value.Y, this->Z - Value.Z, this->W - Value.W };

		return(Result);
	}

	inline Vector4 operator-=(Vector4 Value)
	{
		this->X -= Value.X;
		this->Y -= Value.Y;
		this->Z -= Value.Z;
		this->W -= Value.W;
	}

	inline Vector4 Hadamard(Vector4 Value)
	{
		Vector4 Result = { this->X * Value.X, this->Y * Value.Y, this->Z * Value.Z, this->W * Value.W };

		return(Result);
	}

	inline real32 Inner(Vector4 Value)
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

	inline Vector4 Clamped(real32 Min = 0.0f, real32 Max = 1.0f)
	{
		Vector4 Result = { Clamp(Min, this->X, Max), Clamp(Min, this->Y, Max), Clamp(Min, this->Z, Max), Clamp(Min, this->W, Max) };

		return(Result);
	}

	Vector4 Lerp(real32 Time, Vector4 Value)
	{
		Vector4 Result = (1.0f - Time) * (*this) + Time * Value;

		return(Result);
	}
};

///
/// Matrices
///

// NOTE(Julian): Using a left handed coordinate system
class Matrix4x4
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

		Vector4 E[4];

		real32 M[16];
	};

	Matrix4x4()
	{
		this->E[0] = { 1, 0, 0, 0 };
		this->E[1] = { 0, 1, 0, 0 };
		this->E[2] = { 0, 0, 1, 0 };
		this->E[3] = { 0, 0, 0, 1 };
	}

	Matrix4x4(real32 Value)
	{
		this->E[0] = { Value };
		this->E[1] = { Value };
		this->E[2] = { Value };
		this->E[3] = { Value };
	}

	Matrix4x4(Vector4 X, Vector4 Y, Vector4 Z, Vector4 W)
	{
		this->E[0] = X;
		this->E[1] = Y;
		this->E[2] = Z;
		this->E[3] = W;
	}

	inline Matrix4x4 operator*(real32 A)
	{
		Matrix4x4 Result;

		Result.E[0] = this->E[0] * A;
		Result.E[0] = this->E[1] * A;
		Result.E[0] = this->E[2] * A;
		Result.E[0] = this->E[3] * A;

		return(Result);
	}

	inline Matrix4x4 operator*(Matrix4x4 A)
	{
		Matrix4x4 Result = { 0.0f };

		Result.E[0] = this->E[0] * A.E[0].E[0] + this->E[1] * A.E[0].E[1] + this->E[2] * A.E[0].E[2] + this->E[3] * A.E[0].E[3];
		Result.E[1] = this->E[0] * A.E[1].E[0] + this->E[1] * A.E[1].E[1] + this->E[2] * A.E[1].E[2] + this->E[3] * A.E[1].E[3];
		Result.E[2] = this->E[0] * A.E[2].E[0] + this->E[1] * A.E[2].E[1] + this->E[2] * A.E[2].E[2] + this->E[3] * A.E[2].E[3];
		Result.E[3] = this->E[0] * A.E[3].E[0] + this->E[1] * A.E[3].E[1] + this->E[2] * A.E[3].E[2] + this->E[3] * A.E[3].E[3];

		return(Result);
	}

	inline friend Matrix4x4 operator*(real32 A, Matrix4x4 B)
	{
		Matrix4x4 Result = A * B;

		return(Result);
	}

	inline Matrix4x4 operator*(Vector3 A)
	{
		Matrix4x4 Result = *this * A;

		return(Result);
	}

	inline Matrix4x4 operator*(Vector4 A)
	{
		Matrix4x4 Result =
		{
			this->E[0].E[0] * A.E[0] + this->E[0].E[1] * A.E[1] + this->E[0].E[2] * A.E[2] + this->E[0].E[3] * A.E[3],
			this->E[1].E[0] * A.E[0] + this->E[1].E[1] * A.E[1] + this->E[1].E[2] * A.E[2] + this->E[1].E[3] * A.E[3],
			this->E[2].E[0] * A.E[0] + this->E[2].E[1] * A.E[1] + this->E[2].E[2] * A.E[2] + this->E[2].E[3] * A.E[3],
			this->E[3].E[0] * A.E[0] + this->E[3].E[1] * A.E[1] + this->E[3].E[2] * A.E[2] + this->E[3].E[3] * A.E[3]
		};

		return(Result);
	}

	inline Matrix4x4 operator*=(real32 A)
	{
		this->E[0] *= A;
		this->E[1] *= A;
		this->E[2] *= A;
		this->E[3] *= A;
	}

	inline Matrix4x4 operator-()
	{
		Matrix4x4 Result;

		Result.E[0] = -this->E[0];
		Result.E[1] = -this->E[1];
		Result.E[2] = -this->E[2];
		Result.E[3] = -this->E[3];

		return(Result);
	}

	inline Matrix4x4 operator+(Matrix4x4 A)
	{
		Matrix4x4 Result;

		Result.E[0] = this->E[0] + A.E[0];
		Result.E[1] = this->E[1] + A.E[1];
		Result.E[2] = this->E[2] + A.E[2];
		Result.E[3] = this->E[3] + A.E[3];

		return(Result);
	}

	inline Matrix4x4 operator+=(Matrix4x4 A)
	{
		this->E[0] += A.E[0];
		this->E[1] += A.E[1];
		this->E[2] += A.E[2];
		this->E[3] += A.E[3];
	}

	inline Matrix4x4 operator-(Matrix4x4 A)
	{
		Matrix4x4 Result;

		Result.E[0] = this->E[0] - A.E[0];
		Result.E[1] = this->E[1] - A.E[1];
		Result.E[2] = this->E[2] - A.E[2];
		Result.E[3] = this->E[3] - A.E[3];

		return(Result);
	}

	inline Matrix4x4 operator-=(Matrix4x4 A)
	{
		this->E[0] -= A.E[0];
		this->E[1] -= A.E[1];
		this->E[2] -= A.E[2];
		this->E[3] -= A.E[3];
	}

	k_internal Matrix4x4 Identity(real32 A = 1.0f)
	{
		Matrix4x4 Result = {};

		Result.E[0] = { A, 0, 0, 0 };
		Result.E[1] = { 0, A, 0, 0 };
		Result.E[2] = { 0, 0, A, 0 };
		Result.E[3] = { 0, 0, 0, A };

		return (Result);
	}

	k_internal inline Matrix4x4 Perspective(real32 FOVY, real32 AspectRatio, real32 ZNear, real32 ZFar)
	{
		Assert(AbsoluteValue(AspectRatio - (real32)Epsilon()) > 0.0f);

		real32 TanHalfOfFOV = Tan(FOVY / 2.0f);

		Matrix4x4 Result = { 0 };
		Result.E[0].E[0] = 1.0f / (AspectRatio * TanHalfOfFOV);
		Result.E[1].E[1] = 1.0f / (TanHalfOfFOV);
		Result.E[2].E[3] = 1.0f;

		Result.E[2].E[2] = ZFar / (ZFar - ZNear);
		Result.E[3].E[2] = -(ZFar * ZNear) / (ZFar - ZNear);

		return (Result);
	}

	k_internal inline Matrix4x4 LookAt(Vector3 Eye, Vector3 Center, Vector3 Up)
	{
		Vector3 Front = Vector3::Normalize(Center - Eye);
		Vector3 See = Vector3::Normalize(Vector3::Cross(Up, Front));
		Vector3 Upward = Vector3::Cross(Front, See);

		Matrix4x4 Result = Matrix4x4::Identity(1.0f);

		Result.E[0].E[0] = See.X;
		Result.E[1].E[0] = See.Y;
		Result.E[2].E[0] = See.Z;
		Result.E[0].E[1] = Upward.X;
		Result.E[1].E[1] = Upward.Y;
		Result.E[2].E[1] = Upward.Z;
		Result.E[0].E[2] = Front.X;
		Result.E[1].E[2] = Front.Y;
		Result.E[2].E[2] = Front.Z;
		Result.E[3].E[0] = -Vector3::Dot(See, Eye);
		Result.E[3].E[1] = -Vector3::Dot(Upward, Eye);
		Result.E[3].E[2] = -Vector3::Dot(Front, Eye);

		return (Result);
	}

	k_internal inline Matrix4x4 FPSView(Vector3 Eye, real32 Pitch, real32 Yaw)
	{
		// If the pitch and yaw angles are in degrees,
		// they need to be converted to radians. Here
		// I assume the values are already converted to radians.
		real32 CosPitch = Cos(Pitch);
		real32 SinPitch = Sin(Pitch);
		real32 CosYaw = Cos(Yaw);
		real32 SinYaw = Sin(Yaw);

		Vector3 XAxis = { CosYaw, 0.0f, -SinYaw };
		Vector3 YAxis = { SinYaw * SinPitch, CosPitch, CosYaw * SinPitch };
		Vector3 ZAxis = { SinYaw * CosPitch, -SinPitch, CosPitch * CosYaw };

		// Create a 4x4 view matrix from the right, up, forward and eye position vectors
		Matrix4x4 ViewMatrix =
		{
			Vector4(XAxis.X,            YAxis.X,            ZAxis.X,      0.0f),
			Vector4(XAxis.Y,            YAxis.Y,            ZAxis.Y,      0.0f),
			Vector4(XAxis.Z,           YAxis.Z,            -ZAxis.Z,   0.0f),
			Vector4(-Vector3::Dot(XAxis, Eye), -Vector3::Dot(YAxis, Eye), -Vector3::Dot(ZAxis, Eye), 1)
		};

		return ViewMatrix;
	}

	inline void Rotate(real32 Angle, Vector3 Rotation)
	{
		real32 const ConstAngle = Angle;
		real32 const Cosine = Cos(ConstAngle);
		real32 const Sine = Sin(ConstAngle);

		Vector3 Axis = Rotation.Normalize();
		Vector3 TempVector = ((1.0f - Cosine) * Axis);

		Matrix4x4 Rotate = Matrix4x4(0.0f);
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

	k_internal inline Matrix4x4 Rotate(Matrix4x4 Matrix, real32 Angle, Vector3 Rotation)
	{
		real32 const ConstAngle = Angle;
		real32 const Cosine = Cos(ConstAngle);
		real32 const Sine = Sin(ConstAngle);

		Vector3 Axis = Rotation.Normalize();
		Vector3 TempVector = ((1.0f - Cosine) * Axis);

		Matrix4x4 Rotate = Matrix4x4(0.0f);
		Rotate.E[0].X = Cosine + TempVector.X * Axis.X;
		Rotate.E[0].Y = 0.0f + TempVector.X * Axis.Y + Sine * Axis.Z;
		Rotate.E[0].Z = 0.0f + TempVector.X * Axis.Z - Sine * Axis.Y;

		Rotate.E[1].X = 0.0f + TempVector.Y * Axis.X - Sine * Axis.Z;
		Rotate.E[1].Y = Cosine + TempVector.Y * Axis.Y;
		Rotate.E[1].Z = 0.0f + TempVector.Y * Axis.Z + Sine * Axis.X;

		Rotate.E[2].X = 0.0f + TempVector.Z * Axis.X + Sine * Axis.Y;
		Rotate.E[2].Y = 0.0f + TempVector.Z * Axis.Y - Sine * Axis.X;
		Rotate.E[2].Z = Cosine + TempVector.Z * Axis.Z;

		Matrix4x4 Result = Matrix4x4(0.0f);
		Result.E[0] = Matrix.E[0] * Rotate.E[0].X + Matrix.E[1] * Rotate.E[0].Y + Matrix.E[2] * Rotate.E[0].Z;
		Result.E[1] = Matrix.E[0] * Rotate.E[1].X + Matrix.E[1] * Rotate.E[1].Y + Matrix.E[2] * Rotate.E[1].Z;
		Result.E[2] = Matrix.E[0] * Rotate.E[2].X + Matrix.E[1] * Rotate.E[2].Y + Matrix.E[2] * Rotate.E[2].Z;
		Result.E[3] = Matrix.E[3];
		return (Result);
	}

	inline void Translate(Vector3 Position)
	{
		this->E[3] = this->E[0] * Position.E[0] + this->E[1] * Position.E[1] + this->E[2] * Position.E[2] + this->E[3];
	}

	k_internal inline Matrix4x4 Translate(Matrix4x4 Matrix, Vector3 Position)
	{
		Matrix.E[3] = Matrix.E[0] * Position.E[0] + Matrix.E[1] * Position.E[1] + Matrix.E[2] * Position.E[2] + Matrix.E[3];

		return (Matrix);
	}

	inline void Scale(Vector3 Scale)
	{
		this->E[0].E[0] *= Scale.E[0];
		this->E[1].E[1] *= Scale.E[1];
		this->E[2].E[2] *= Scale.E[2];
	}

	k_internal inline Matrix4x4 Scale(Matrix4x4 Matrix, Vector3 Scale)
	{
		Matrix.E[0] *= Scale.E[0];
		Matrix.E[1] *= Scale.E[1];
		Matrix.E[2] *= Scale.E[2];
	}
};

// NOTE(Julian): Quaternions

class Quaternion
{
	public:

	union
	{
		struct
		{
			Vector4 Euler;
		};

		struct
		{
			real32 X, Y, Z, W;
		};

		real32 E[4];
	};

	inline Quaternion()
	{
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
		W = 1.0f;
	}

	inline Quaternion(Vector3 EulerAngle)
	{
		Vector3 C = Vector3::Cosine(EulerAngle * 0.5f);
		Vector3 S = Vector3::Sine(EulerAngle * 0.5f);

		this->W = C.X * C.Y * C.Z + S.X * S.Y * S.Z;
		this->X = S.X * C.Y * C.Z - C.X * S.Y * S.Z;
		this->Y = C.X * S.Y * C.Z + S.X * C.Y * S.Z;
		this->Z = C.X * C.Y * S.Z - S.X * S.Y * C.Z;
	}

	inline Quaternion(real32 X, real32 Y, real32 Z, real32 W)
	{
		this->W = X;
		this->X = Y;
		this->Y = Z;
		this->Z = W;
	}

	inline Quaternion operator+(const Quaternion& Quat)
	{
		return Quat;
	}

	inline Quaternion operator-()
	{
		Quaternion Result = { -this->X, -this->Y, -this->Z, -this->W };;

		return (Result);
	}

	inline Quaternion operator-(const Quaternion& Quat)
	{
		Quaternion Result = { this->X - Quat.X, this->Y - Quat.Y, this->Z - Quat.Z, this->W - Quat.W };

		return (Result);
	}

	inline Quaternion operator*(real32 A)
	{
		Quaternion Result = { this->X * A, this->Y * A, this->Z * A, this->W * A };

		return (Result);
	}

	inline Vector3 operator*(Vector3 Direction)
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
		Vector3 Result;

		Result.X = (1.0f - (num5 + num6)) * Direction.X + (num7 - num12) * Direction.Y + (num8 + num11) * Direction.Z;
		Result.Y = (num7 + num12) * Direction.X + (1.0f - (num4 + num6)) * Direction.Y + (num9 - num10) * Direction.Z;
		Result.Z = (num8 - num11) * Direction.X + (num9 + num10) * Direction.Y + (1.0f - (num4 + num5)) * Direction.Z;

		return (Result);
	}

	inline friend Quaternion operator*(real32 A, Quaternion B)
	{
		Quaternion Result = B * A;

		return (Result);
	}

	inline friend Quaternion operator/(Quaternion A, real32 B)
	{
		Quaternion Result = { A.X / B, A.Y / B, A.Z / B, A.W / B };

		return (Result);
	}

	inline k_internal friend Quaternion operator*(Quaternion Left, Vector4 Right)
	{
		Quaternion Result = {};

		Result.W = (Left.W * Right.W) - (Left.X * Right.X) - (Left.Y * Right.Y) - (Left.Z * Right.Z);

		Result.X = (Left.W * Right.X) + (Left.X * Right.W) - (Left.Y * Right.Z) - (Left.Z * Right.Y);

		Result.Y = (Left.W * Right.Y) - (Left.Y * Right.W) - (Left.Z * Right.X) - (Left.X * Right.Z);

		Result.Z = (Left.W * Right.Z) - (Left.Z * Right.W) - (Left.X * Right.Y) - (Left.Y * Right.X);

		return(Result);
	}

	inline Quaternion operator*(Quaternion Quat)
	{
		Quaternion Result = {};

		Result.W = (this->W * Quat.W) - (this->X * Quat.X) - (this->Y * Quat.Y) - (this->Z * Quat.Z);

		Result.X = (this->W * Quat.X) + (this->X * Quat.W) - (this->Y * Quat.Z) - (this->Z * Quat.Y);

		Result.Y = (this->W * Quat.Y) - (this->Y * Quat.W) - (this->Z * Quat.X) - (this->X * Quat.Z);

		Result.Z = (this->W * Quat.Z) - (this->Z * Quat.W) - (this->X * Quat.Y) - (this->Y * Quat.X);

		return(Result);
	}

	inline void operator*=(Quaternion Quat)
	{
		*this = *this * Quat;
	}

	inline k_internal real32 Squared(Quaternion Quat)
	{
		real32 Result = Square(Quat.X) + Square(Quat.Y) + Square(Quat.Z) + Square(Quat.W);

		return (Result);
	}

	inline k_internal Quaternion Normalize(Quaternion Quat)
	{
		real32 Magnitude = Squared(Quat);

		Quaternion Result = { Quat.X / Magnitude, Quat.Y / Magnitude, Quat.Z / Magnitude, Quat.W / Magnitude };

		return (Result);
	}

	inline Quaternion Normalize()
	{
		Quaternion Result = Normalize(*this);

		return (Result);
	}

	inline k_internal Quaternion Conjugate(Quaternion Quat)
	{
		Quaternion Result = { -Quat.X, -Quat.Y, -Quat.Z, Quat.W };

		return (Result);
	}

	inline Quaternion Conjugate()
	{
		Quaternion Result = Conjugate(*this);

		return (Result);
	}

	inline k_internal Quaternion FromAxisAngle(Vector3 Axis, real32 Angle)
	{
		Quaternion Result = {};

		Result.X = Axis.X * Sin(Angle / 2.0f);
		Result.Y = Axis.Y * Sin(Angle / 2.0f);
		Result.Z = Axis.Z * Sin(Angle / 2.0f);
		Result.W = Cos(Angle / 2.0f);

		return (Result);
	}

	inline k_internal Vector3 RotateThing(Quaternion A, Vector3 B)
	{
		Quaternion QuatVector = (B.X, B.Y, B.Z, 0.0f);

		Quaternion Combined = A * QuatVector * A.Conjugate();

		Vector3 Result = (Combined.X, Combined.Y, Combined.Z);

		return (Result);
	}

	inline k_internal Quaternion RotateTo(Vector3 A, Vector3 B)
	{
		Quaternion Result = {};

		Vector3 Cross = A.Cross(B);

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
			Vector3 XAxis(1, 0, 0);
			Vector3 YAxis(0, 1, 0);

			Cross = A.Cross(AbsoluteValue(A.Dot(XAxis)) < 1.0f ? XAxis : YAxis);
			SquareMagnitude = CosTheta = 0.0f;
		}

		Result = Quaternion(Cross.X, Cross.Y, Cross.Z, SquareMagnitude + CosTheta);

		return (Result);
	}

	inline k_internal Quaternion Rotate(Quaternion A, real32 Angle, Vector3 View)
	{
		Vector3 TempVec = View;

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

		Quaternion Result = A * Quaternion(TempVec.X * Sine, TempVec.Y * Sine, TempVec.Z * Sine, Cos(AngleRad * 0.5f));

		return (Result);
		//return gtc::quaternion::cross(q, tquat<T, P>(cos(AngleRad * T(0.5)), Tmp.x * fSin, Tmp.y * fSin, Tmp.z * fSin));
	}

	inline void Rotate(real32 Angle, Vector3 View)
	{
		*this = Rotate(*this, Angle, View);
	}

	inline k_internal real32 Dot(Quaternion A, Quaternion B)
	{
		Vector4 TempVec(A.X * B.X, A.Y * B.Y, A.Z * B.Z, A.W * B.W);

		real32 Result = (TempVec.X + TempVec.Y) + (TempVec.Z + TempVec.W);

		return (Result);
	}

	inline k_internal Quaternion MixQuat(Quaternion A, real32 Value, Quaternion B)
	{
		real32 CosTheta = Quaternion::Dot(A, B);

		Quaternion Result = {};

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
	inline k_internal Matrix4x4 ToMatrix(Quaternion Quat)
	{

		// NOTE if inlined, then gcc 4.2 and 4.4 get rid of the temporary (not gcc 4.3 !!)
		// if not inlined then the cost of the return by value is huge ~ +35%,
		// however, not inlining this function is an order of magnitude slower, so
		// it has to be inlined, and so the return by value is not an issue
		Matrix4x4 Result = {};

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
	}	inline Matrix4x4 ToMatrix()
	{
		Matrix4x4 Result = ToMatrix(*this);
		return (Result);
	}	inline k_internal Quaternion FromMatrix(Matrix4x4 Matrix)	{		Quaternion Result = {};
		Result.W = SquareRoot(Max(0.0f, 1.0f + Matrix.E[0].E[0] + Matrix.E[1].E[1] + Matrix.E[2].E[2])) / 2.0f;
		Result.X = SquareRoot(Max(0.0f, 1.0f + Matrix.E[0].E[0] - Matrix.E[1].E[1] - Matrix.E[2].E[2])) / 2.0f;
		Result.Y = SquareRoot(Max(0.0f, 1.0f - Matrix.E[0].E[0] + Matrix.E[1].E[1] - Matrix.E[2].E[2])) / 2.0f;
		Result.Z = SquareRoot(Max(0.0f, 1.0f - Matrix.E[0].E[0] - Matrix.E[1].E[1] + Matrix.E[2].E[2])) / 2.0f;
		Result.X *= SignOf(Result.X * (Matrix.E[2].E[1] - Matrix.E[1].E[2]));
		Result.Y *= SignOf(Result.Y * (Matrix.E[0].E[2] - Matrix.E[2].E[0]));
		Result.Z *= SignOf(Result.Z * (Matrix.E[1].E[0] - Matrix.E[0].E[1]));

		return (Result);	}	inline k_internal Quaternion Lerp(Quaternion A, real32 Value, Quaternion B)
	{
		// Lerp is only defined in [0, 1]
		Assert(Value >= 0.0f);
		Assert(Value <= 1.0f);

		Quaternion Result = A * (1.0f - Value) + (B * Value);

		return (Result);
	}	inline k_internal Quaternion Slerp(Quaternion A, real32 Value, Quaternion B)
	{
		Quaternion Current = B;

		real32 CosTheta = Quaternion::Dot(A, B);

		Quaternion Result = {};

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
	}	inline Quaternion Slerp(real32 Value, Quaternion B)
	{
		Quaternion Result = Slerp(*this, Value, B);

		return(Result);
	}	inline k_internal real32 Roll(Quaternion Quat)
	{
		real32 A = 2.0f * (Quat.X * Quat.Y + Quat.W * Quat.Z);
		real32 B = Square(Quat.W) + Square(Quat.X) - Square(Quat.Y) - Square(Quat.Z);

		real32 Result = ATan2(A, B);

		return (Result);
	}

	inline k_internal real32 Pitch(Quaternion Quat)
	{
		real32 A = 2.0f * (Quat.Y * Quat.Z + Quat.W * Quat.X);
		real32 B = Square(Quat.W) - Square(Quat.X) - Square(Quat.Y) + Square(Quat.Z);

		real32 Result = ATan2(A, B);

		return (Result);
	}

	inline k_internal real32 Yaw(Quaternion Quat)
	{
		real32 A = -2.0f * (Quat.X * Quat.Z - Quat.W * Quat.Y);

		real32 Result = ASin(Clamp(-1.0f, A, 1.0f));

		return (Result);
	}	inline k_internal Vector3 ToEuler(Quaternion Quat)	{		Vector3 Result = {};		Result.X = Pitch(Quat);		Result.Y = Yaw(Quat);		Result.Z = Roll(Quat);		return (Result);	}	inline Vector3 ToEuler()	{		Vector3 Result = {};		Result.X = Pitch(*this);		Result.Y = Yaw(*this);		Result.Z = Roll(*this);		return (Result);	}
};

#define KANTI_MATH
#endif // !KANTI_MATH