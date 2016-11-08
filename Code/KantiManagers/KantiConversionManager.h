#ifndef KANTI_CONVERTER

//
// Conversion operations
//

inline real32
ToFloat(char* String)
{
	float Result = 0.0f, Factorial = 1.0f;

	if (*String == '-') 
	{
		String++;
		Factorial = -1.0f;
	}

	for (uint32 Point = 0; *String; String++) 
	{
		if (*String == '.') 
		{
			Point = 1;
			continue;
		}

		uint32 Decimal = *String - '0';

		if (Decimal >= 0 && Decimal <= 9) 
		{
			if (Point)
			{
				Factorial /= 10.0f;
			}

			Result = Result * 10.0f + (real32)Decimal;
		}
	}

	return Result * Factorial;
}

inline int32
ToInt(char* String, uint32 Size)
{
	int32 Result = 0, Factorial = 1;

	if (*String == '-')
	{
		String++;
		Factorial = -1;
	}

	uint32 Place = Size - 1;
	for (uint32 Index = 0; Index < Size; ++Index)
	{
		Result += (String[Index] - '0') * (Power(10, Place));
		Place--;
	}

	return (Result);
}

inline int32
ToInt(char* String)
{
	int32 Result;
	uint32 Size = 0;

	for (char* Character = String; *Character; ++Character)
	{
		Size++;
	}

	Result = ToInt(String, Size);

	return (Result);
}


#define KANTI_CONVERTER
#endif