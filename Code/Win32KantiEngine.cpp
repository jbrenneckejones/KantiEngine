#include <Windows.h>
#include <iostream>
#include <cstdio>

#include "KantiEngine.h"
#include "KantiEngine.cpp"

// For DSOUND
#include <initguid.h>
#include <dsound.h>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Unique ID

#include <Objbase.h>

k_internal PLATFORM_GET_UUID(Win32GetUUID)
{
	UUID UID;
	CoCreateGuid(&UID);

	UniqueID Result;
	Result.Data1 = UID.Data1;
	Result.Data2 = UID.Data2;
	Result.Data3 = UID.Data3;

	for (uint32 Index = 0; Index < sizeof(Result.Data4); ++Index)
	{
		Result.Data4[Index] = UID.Data4[Index];
	}

	return (Result);
}

// Time

k_internal
PLATFORM_GET_PERFORMANCE_FREQUENCY(Win32GetPerformanceFrequency)
{
	LARGE_INTEGER PerfCountFrequencyResult;
	QueryPerformanceFrequency(&PerfCountFrequencyResult);
	int64 PerfCountFrequency = PerfCountFrequencyResult.QuadPart;

	return PerfCountFrequency;
}

k_internal PLATFORM_GET_PERFORMANCE_COUNTER(Win32GetLastCounter)
{
	LARGE_INTEGER PerformanceCounter;
	QueryPerformanceCounter(&PerformanceCounter);

	return PerformanceCounter.QuadPart;
}

// Cycles

k_internal PLATFORM_GET_CYCLE_COUNTER(Win32GetLastCycle)
{
	uint64 LastCycleCount = __rdtsc();

	return LastCycleCount;
}

// Debug

k_internal
PLATFORM_GAME_DEBUG_CONSOLE(Win32DebugConsole)
{
	local_persist bool32 ConsoleSetup = false;

	if (!ConsoleSetup)
	{
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		FILE *Stream;
		freopen_s(&Stream, "CONOUT$", "w+", stdout);
		SetConsoleTitle("Kanti Engine");
	}

	std::cout << DebugMessage.Data();
}

k_internal
PLATFORM_GAME_ERROR(Win32Error)
{
	Win32DebugConsole(Message);
	char Buffer[256];
	snprintf(Buffer, sizeof(Buffer), " In: %s Line: %d", File.Data(), LineNumber);
	Win32DebugConsole(Buffer);
}

// Memory

k_internal
PLATFORM_ALLOCATE_MEMORY(Win32AllocateMemory)
{
	return VirtualAlloc((LPVOID)Address, Size, MemoryType, MemoryAccess);
}

// File IO

DWORD G_BytesTransferred;

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped)
{
	G_BytesTransferred = dwNumberOfBytesTransfered;
}

k_internal
PLATFORM_GET_FILE_CONTENTS(Win32GetFileContents)
{
	KString Result;

	GET_FILEEX_INFO_LEVELS FileInfoLevel = GetFileExInfoStandard;
	WIN32_FILE_ATTRIBUTE_DATA FileAttributes;
	bool32 FileFound = GetFileAttributesExA(FileName.Data(), FileInfoLevel, &FileAttributes);

	if (!FileFound)
	{
		return (Result);
	}

	LARGE_INTEGER FileSize;
	FileSize.HighPart = FileAttributes.nFileSizeHigh;
	FileSize.LowPart = FileAttributes.nFileSizeLow;

	uint64 BufferSize = FileSize.QuadPart;

	HANDLE FileHandle;
	DWORD  BytesRead = 0;
	KString   ReadBuffer((uint32)BufferSize);
	OVERLAPPED Overlapped = { 0 };

	FileHandle = CreateFile(FileName.Data(),               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		return (Result);
	}

	if (ReadFileEx(FileHandle, ReadBuffer.Data(), (DWORD)BufferSize - 1, &Overlapped, FileIOCompletionRoutine) == FALSE)
	{
		CloseHandle(FileHandle);
		return (Result);
	}

	SleepEx(5000, TRUE);
	BytesRead = G_BytesTransferred;

	if (BytesRead > 0 && BytesRead <= BufferSize - 1)
	{
		ReadBuffer[BytesRead] = '\0'; // NULL character
		Result = ReadBuffer;
	}

	CloseHandle(FileHandle);

	return (Result);
}

k_internal
PLATFORM_SET_FILE_CONTENTS(Win32SetFileContents)
{
	HANDLE FileHandle;
	DWORD BytesToWrite = Contents.Count();
	DWORD BytesWritten = 0;
	BOOL ErrorFlag = FALSE;

	FileHandle = CreateFile(FileName.Data(),                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (FileHandle == INVALID_HANDLE_VALUE)
	{
		// Win32Error((TEXT("Terminal failure: Unable to open file \"%s\" for write.\n"), FileName.Data()), __FILE__, __LINE__);
		return;
	}

	ErrorFlag = WriteFile(
		FileHandle,           // open file handle
		Contents.Data(),      // start of data to write
		BytesToWrite,  // number of bytes to write
		&BytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure

	if (FALSE == ErrorFlag)
	{
		// Win32Error("Terminal failure: Unable to write to file.\n", __FILE__, __LINE__);
	}
	else
	{
		if (BytesWritten != BytesToWrite)
		{
			// This is an error because a synchronous write that results in
			// success (WriteFile returns TRUE) should write all data as
			// requested. This would not necessarily be the case for
			// asynchronous writes.

			// printf("Error: dwBytesWritten != dwBytesToWrite\n");
		}
		else
		{
			// _tprintf(TEXT("Wrote %d bytes to %s successfully.\n"), BytesWritten, argv[1]);
		}
	}

	CloseHandle(FileHandle);
}

k_internal void SetWin32FunctionCalls(PlatformFunctionHandles& Win32Handles)
{
	GetUUID = Win32GetUUID;
	Win32Handles.GetUUID = Win32GetUUID;
	Win32Handles.ConsoleOutput = Win32DebugConsole;
	Win32Handles.ErrorOutput = Win32Error;
	Win32Handles.GetCycleCounter = Win32GetLastCycle;
	Win32Handles.GetFileContents = Win32GetFileContents;
	Win32Handles.GetPerformanceCounter = Win32GetLastCounter;
	Win32Handles.GetPerformanceFrequency = Win32GetPerformanceFrequency;
	Win32Handles.PlatforMemAllocateMemory = Win32AllocateMemory;
	Win32Handles.SetFileContents = Win32SetFileContents;
}

k_internal LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch (Message)
	{
	case WM_SIZE:
	{
		break;
	}

	case WM_CLOSE:
	{
		// TODO(Julian): Handle this as a message to the user
		KEngine::EngineInstance->IsRunning = false;
		break;
	}

	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;
	}

	case WM_DESTROY:
	{
		// TODO(Julian): Handle this as an error, recreate window
		KEngine::EngineInstance->IsRunning = false;
		break;
	}

	case WM_SYSKEYDOWN:
	{
		break;
	}

	case WM_SYSKEYUP:
	{
		break;
	}

	case WM_MOUSEMOVE:
	{
		int32 X = LOWORD(LParam);
		int32 Y = HIWORD(LParam);

		KEngine::EngineInstance->SetMouseInput(X, Y);

		break;
	}

	case WM_KEYDOWN:
	{
		uint32 VKCode = (uint32)WParam;

		KEngine::EngineInstance->SetKeyboardInput(VKCode, LParam);

		if (VKCode == '1')
		{
			KMeshRenderer* Mesh = new KMeshRenderer();
			KantiFileManager::LoadOBJFile("Hexagon Shape.obj", Mesh->MeshData);
			Mesh->Position = KantiCameraManager::GetMainCamera()->Position;

			KEngine::EngineInstance->KRenderer.CreateMeshBuffer(Mesh);
			KEngine::EngineInstance->Meshes.PushBack(Mesh);
		}

		break;
	}

	case WM_KEYUP:
	{
		uint32 VKCode = (uint32)WParam;

		KEngine::EngineInstance->SetKeyboardInput(VKCode, LParam);

		/*
		if(WasDown != IsDown)
		{
			if(VKCode == 'W')
			{
			}
			else if(VKCode == 'A')
			{
			}
			else if(VKCode == 'S')
			{
			}
			else if(VKCode == 'D')
			{
			}
			else if(VKCode == 'Q')
			{
			}
			else if(VKCode == 'E')
			{
			}
			else if(VKCode == VK_UP)
			{
			}
			else if(VKCode == VK_LEFT)
			{
			}
			else if(VKCode == VK_DOWN)
			{
			}
			else if(VKCode == VK_RIGHT)
			{
			}
			else if(VKCode == VK_ESCAPE)
			{
				if(IsDown)
				{
				}
				if(WasDown)
				{
				}
			}
			else if(VKCode == VK_SPACE)
			{
			}
		}

		*/

		bool32 AltKeyWasDown = (LParam & (1 << 29));
		if ((VKCode == VK_F4) && AltKeyWasDown)
		{
			KEngine::EngineInstance->IsRunning = false;
		}

		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT Paint;
		HDC DeviceContext = BeginPaint(Window, &Paint);
		EndPaint(Window, &Paint);
		break;
	}

	default:
	{
		// OutputDebugStringA("Default\n");
		Result = DefWindowProc(Window, Message, WParam, LParam);
		break;
	}
	}

	return (Result);
}

int CALLBACK
WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, LPSTR CommandLine, int ShowCode)
{
	WNDCLASSA WindowClass = {};

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// WindowClass.hIcon = ;
	WindowClass.lpszClassName = "KantiEngineClass";

	KEngine GameEngine = KEngine();

	if (RegisterClassA(&WindowClass))
	{
		HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "KantiEngine",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, Instance, 0);

		if (Window)
		{
			PlatformFunctionHandles Handles;
			SetWin32FunctionCalls(Handles);
			// Setup function calls to Engine
			GameEngine.InitializeGameManager(Handles);

			renderer_platform WindowInfo;
			WindowInfo.Instance = Instance;
			WindowInfo.Window = Window;
			// Intialize window information
			GameEngine.InitializeWindow(WindowInfo);

			while (GameEngine.IsRunning)
			{
				MSG Message;
				while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						GameEngine.IsRunning = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}

				GameEngine.EngineLoop();
			}
		}
		else
		{
			// TODO(Julian): Logging
		}
	}
	else
	{
		// TODO(Julian): Logging
	}

	return(0);
}