// String 

struct k_string
{
	uint32 Size;
	char* Buffer;
};

inline uint32
GetPointerCharArraySize(char* Array)
{
	uint32 Count = 0;
	for(char* At = Array; *At; ++At)
	{
		Count++;
	}

	return Count;
}

inline void
CharArrayCopyTo(char** Destination, char* Source, uint32 Size = -1)
{
	if(Size <= -1)
	{
		Size = GetPointerCharArraySize((char *)Source);
	}

	Destination[0] = (char *)MemAlloc(sizeof(char) + Size);

	for(int Index = 0; Index < Size; ++Index)
	{
		Destination[0][Index] = Source[Index];
	}

	Destination[0][Size] = '\0';
}

inline char*
CharArrayGetCopy(const char* Source, uint32 Size = -1)
{
	char* Result = (char *)MemAlloc(sizeof(char) + Size);

	if(Size <= -1)
	{
		Size = GetPointerCharArraySize((char *)Source);
	}

	for(int Index = 0; Index < Size; ++Index)
	{
		Result[Index] = Source[Index];
	}

	Result[Size] = '\0';
}

inline void
CharArrayAppend(char** Destination, char* Addition, uint32 SizeDest = -1, uint32 SizeAdd = -1)
{
	if(SizeDest <= -1)
	{
		SizeDest = GetPointerCharArraySize(Destination[0]);
	}

	if(SizeAdd <= -1)
	{
		SizeAdd = GetPointerCharArraySize((char *)Addition);
	}

	int TotalSize = SizeDest + SizeAdd;

	Destination[0] = (char *)realloc(Destination[0], sizeof(char) + TotalSize);

	int AddIndex = 0;
	for(int Index = SizeDest; Index < TotalSize; ++Index)
	{
		Destination[0][Index] = Addition[AddIndex++];
	}

	Destination[0][TotalSize] = '\0';
}

inline void
CharArrayAppend(char** Destination, const char* Addition, uint32 SizeDest = -1, uint32 SizeAdd = -1)
{
	CharArrayAppend(Destination, (char *)Addition, SizeDest, SizeAdd);
}

inline char*
CharArrayCombine(const char* Left, const char* Right, uint32 SizeLeft = -1, uint32 SizeRight = -1)
{
	if(SizeLeft <= -1)
	{
		SizeLeft = GetPointerCharArraySize((char *)Left);
	}

	if(SizeRight <= -1)
	{
		SizeRight = GetPointerCharArraySize((char *)Right);
	}

	char* Result = (char *)MemAlloc(sizeof(char) + SizeLeft + SizeRight);

	CharArrayCopyTo(&Result, Left, SizeLeft);

	CharArrayAppend(&Result, Right, SizeLeft, SizeRight);

	return (Result);
}

inline k_string
operator+(k_string A, k_string B)
{
	k_string Result = {};
	Result.Buffer = CharArrayCombine(A.Buffer, B.Buffer, A.Size, B.Size);
	Result.Size = A.Size + B.Size;

	return Result;
}

inline void
operator+=(k_string &A, k_string B)
{
	CharArrayAppend(&A.Buffer, B.Buffer, A.Size, B.Size);
	A.Size += B.Size;
}

inline k_string
StringI(char* String, uint32 Length)
{
	k_string Result = {};

	CharArrayCopyTo(&Result.Buffer, String, Length);
	Result.Size = Length;

	return (Result);
}

inline k_string
StringI(char* String)
{
	return StringI(String, GetPointerCharArraySize(String));
}

inline void
StringReplaceAt(char** CharArray, uint32 SizeArr, char Character, uint32 Index)
{
	char* Result = (char *)MemAlloc(sizeof(char) + SizeArr);

	for(int ArrayIndex = 0; ArrayIndex < SizeArr; ++ArrayIndex)
	{
		if(ArrayIndex == Index)
		{
			Result[ArrayIndex] = Character;
		}
		else
		{
			Result[ArrayIndex] = CharArray[0][ArrayIndex];
		}
	}

	Result[SizeArr] = '\0';
	*CharArray = Result;
}

inline void
StringReplaceAt(char** CharArray, char Character, uint32 Index)
{
	StringReplaceAt(CharArray, GetPointerCharArraySize(*CharArray), Character, Index);
}

inline void
StringReplaceAt(k_string& String, char Character, uint32 Index)
{
	StringReplaceAt(&String.Buffer, String.Size, Character, Index);
}