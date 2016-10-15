#ifndef KANTI_RANDOM_MANAGER

#include "KantiPlatform.h"
#include "KantiMath.h"

class KantiRandomManager
{
	private:

	/* initialize state to random bits */
	k_internal const uint32 GeneratorStateCount = 16;

	k_internal uint64 GeneratorStates[GeneratorStateCount];

	/* init should also reset this to 0 */
	k_internal uint32 GeneratorIndex;

	k_internal uint64 GeneratorMaxValue;

	public:

	inline k_internal uint64 RandomMax()
	{
		return GeneratorMaxValue;
	}

	inline k_internal void Initialize(uint64 Seed[GeneratorStateCount], bool32 UsePrecomputedStates = false)
	{
		if(!UsePrecomputedStates)
		{
			for(uint32 Index = 0; Index < GeneratorStateCount; ++Index)
			{
				GeneratorStates[Index] = Seed[Index];
			}
		}

		// NOTE(Julian): We have to generate these so we can get a potential max value
		for(uint32 Index = 0; Index < GeneratorStateCount; ++Index)
		{
			RandomNext();
		}

		uint64 Max = 0;
		for(uint32 Index = 0; Index < GeneratorStateCount; ++Index)
		{
			if(GeneratorStates[Index] > Max)
			{
				Max = GeneratorStates[Index];
			}
		}

		GeneratorMaxValue = Max;
	}

	inline k_internal uint64 RandomNext()
	{
		uint64 A, B, C, D;
		A = GeneratorStates[GeneratorIndex];
		C = GeneratorStates[(GeneratorIndex + 13) & 15];
		B = A ^ C ^(A << 16) ^ (C << 15);
		C = GeneratorStates[(GeneratorIndex + 9) & 15];
		C ^= (C >> 11);
		A = GeneratorStates[GeneratorIndex] = B ^ C;
		D = A ^ ((A << 5) & 0xDA442D24UL);
		GeneratorIndex = (GeneratorIndex + 15) & 15;
		A = GeneratorStates[GeneratorIndex];
		GeneratorStates[GeneratorIndex] = A ^ B ^ D ^(A << 2) ^ (B << 18) ^ (C << 28);

		uint64 Result = GeneratorStates[GeneratorIndex];

		if(Result > GeneratorMaxValue)
		{
			GeneratorMaxValue = Result;
		}

		return (Result);
	}

	inline k_internal uint32 RandomChoice(uint32 ChoiceCount)
	{
		uint32 Result = (RandomNext() % ChoiceCount);

		return(Result);
	}

	inline k_internal real32 RandomRange(real32 Min, real32 Max)
	{
		real32 Numerator = Min + (real32)(RandomNext());
		real32 Divisor = (real32)(GeneratorMaxValue / (Max - Min));
		real32 Result = Numerator / Divisor;

		return (Result);
	}

	inline k_internal int32 RandomRange(int32 Min, int32 Max)
	{
		int32 Numerator = Min + (int32)(RandomNext());
		int32 Divisor = (int32)(GeneratorMaxValue / (Max - Min));
		int32 Result = Numerator / Divisor;

		return (Result);
	}

	inline k_internal real32 RandomRangeUniform(real32 Min, real32 Max)
	{
		return RandomNext() / (GeneratorMaxValue + 1.0f) * (Max - Min) + Min;
	}

	inline k_internal int32 RandomRangeUniform(int32 Min, int32 Max)
	{
		return (int32)(RandomNext() / (GeneratorMaxValue + 1) * (Max - Min) + Min);
	}

	inline k_internal real32 RandomGaussian(real32 Mean, real32 Sigma)
	{
		const real32 Epsi = (real32)Epsilon();
		const real32 PiTwo = 2.0f * Pi32;

		k_internal real32 Z0, Z1;
		k_internal bool32 Generate;
		Generate = !Generate;

		real32 Result = Z1 * Sigma + Mean;

		if(!Generate)
		{
			return (Result);
		}

		real32 U1, U2;
		do
		{
			U1 = RandomNext() * (1.0f / GeneratorMaxValue);
			U2 = RandomNext() * (1.0f / GeneratorMaxValue);
		}
		while ( U1 <= Epsi );

		Z0 = SquareRoot(-2.0f * Log(U1)) * Cos(PiTwo * U2);
		Z1 = SquareRoot(-2.0f * Log(U1)) * Sin(PiTwo * U2);

		Result = Z0 * Sigma + Mean;

		return (Result);
	}

	inline k_internal real32 RandomExponential(real32 A, real32 Rate)
	{
		const real32 RESOLUTION = 1000.0;
		real32 Uniform;

		real32 Result;

		if(Rate == 0)
		{
			Result = NAN;
		}
		else
		{
			do
			{
				Uniform = (RandomRange(0.0f, (real32)RESOLUTION) / (real32)RESOLUTION);
			} while(Uniform == 0);

			Result = A - Rate * Log(Uniform);
		}

		return (Result);
	}

	inline k_internal real32 RandomGamma(real32 A, real32 B, real32 C)
	{
		const real32 RESOLUTION = 1000.0f;
		const real32 T = 4.5f;
		const real32 D = 1 + Log(T);

		real32 Uniform;
		real32 A2, B2, C2, Q, P, Y;
		real32 P1, P2, V, W, Z;

		bool32 Found;

		A2 = (1.0f / SquareRoot(2.0f * C - 1.0f));
		B2 = C - Log(4);
		Q = C + 1 / A2;
		C2 = 1 + C / Exp(1);
		Found = false;

		real32 Result = 0.0f;

		if(C < 1)
		{
			do
			{
				do
				{
					Uniform = RandomRange(0.0f, RESOLUTION) / RESOLUTION;
				} while(Uniform < 0);

				P = C2 * Uniform;

				if(P > 1)
				{
					do
					{
						Uniform = RandomRange(0.0, RESOLUTION) / RESOLUTION;
					} while(Uniform < 0);

					Y = -Log((C2 - P) / C);

					if(Uniform <= Power(Y, C - 1))
					{
						Result = A + B * Y;
						Found = true;
					}
				}
				else
				{
					Y = Power(P, 1 / C);

					if(Uniform <= Exp(-Y))
					{
						Result = A + B * Y;
						Found = true;
					}
				}

			} while(!Found);


		}
		else if(C == 1)
		{
			Result = RandomExponential(A, B);
		}
		else
		{
			do
			{
				do
				{
					P1 = RandomRange(0.0f, RESOLUTION) / RESOLUTION;
				} while(P1 < 0);

				do
				{
					P2 = RandomRange(0.0f, RESOLUTION) / RESOLUTION;
				} while(P1 < 0);

				V = A2 * Log(P1 / (1 - P1));
				Y = C * Exp(V);
				Z = P1 * P1 * P2;
				W = B2 + Q * V - Y;

				if(W + D - T * Z >= 0 || W >= Log(Z))
				{
					Result = A + B * Y;
					Found = true;
				}

			} while(!Found);
		}


		return Result;
	}

	inline k_internal real32 RandomErlang(real32 Mean, uint32 Shape)
	{
		const real32 RESOLUTION = 1000;

		real32 Uniform, Product;

		real32 Result = 0;

		if(Mean <= 0 || Shape < 1)
		{
			Result = NAN;
		}
		else
		{
			Product = 1;
			
			for(uint32 Index = 1; Index < Shape; ++Index)
			{
				do
				{
					Uniform = RandomRange(0.0f, RESOLUTION) / RESOLUTION;
				} while(Uniform < 0);

				Product = Product * Uniform;
			}

			Result = -Mean * Log(Product);
		}

		return Result;
	}

	inline k_internal int32 RandomPoisson(real32 Mean)
	{
		const real32 RESOLUTION = 1000.0f;

		int32 Successes;
		real32 B, L;

		int32 Result = 0;

		Assert(Mean > 0);

		Successes = 0;
		B = 1;

		L = Exp(-Mean);

		while(B > L)
		{
			Successes++;
			B = B * RandomRange(0.0, RESOLUTION) / RESOLUTION;
		}

		Result = Successes - 1;

		return Result;
	}

	inline k_internal real32 RandomChisq(int32 DegreeFreedom)
	{
		real32 Result = 0;

		if(DegreeFreedom < 1)
		{
			Result = NAN;
		}
		else
		{
			Result = RandomGamma(0.0f, 2.0f, 0.5f * DegreeFreedom);
		}

		return Result;
	}

	inline k_internal real32 RandomStudentT(int32 DegreeFreedom)
	{
		real32 Result = 0;

		if(DegreeFreedom < 1)
		{
			Result = NAN;
		}
		else
		{
			Result = RandomRange(0, 1) / SquareRoot(RandomChisq(DegreeFreedom) / DegreeFreedom);
		}

		return Result;
	}

	inline k_internal real32 RandomFisher(int32 V, int32 W)
	{
		real32 Result = 0;

		if(V < 1 || W < 1)
		{
			Result = NAN;
		}
		else
		{
			Result = RandomChisq(V) / V / (RandomChisq(W) / W);
		}

		return Result;
	}
};

uint64 KantiRandomManager::GeneratorMaxValue = 0;

uint32 KantiRandomManager::GeneratorIndex = 0;

uint64 KantiRandomManager::GeneratorStates[] =
{
	0x4f0143b, 0x3402005, 0x26f2b01, 0x22796b6, 0x57343bb, 0x2d9954e, 0x06f9425, 0x1789180,
	0x57d8fab, 0x5365d9c, 0x0e9ec55, 0x2a623e0, 0x366e05d, 0x3759f45, 0x1b4d151, 0x35a5411
};

#define KANTI_RANDOM_MANAGER
#endif
