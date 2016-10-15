#include <Windows.h>
#include <iostream>
#include <cstdio>

#include "KantiPlatform.h"
#include "KantiIntrinsics.h"
#include "KantiMath.h"
#include "KantiEngine.h"
#include "KantiVulkan/KantiVulkan.h"
#include "KantiVulkan/KantiVulkan.cpp"
#include "KantiPrimitives.h"
#include "KantiMemoryManager.h"
#include "KantiRenderManager.h"
#include "KantiTimeManager.h"

#include "KantiInputManager.h"
#include "KantiCameraManager.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

global_variable bool32 GlobalRunning = false;

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

	if(!ConsoleSetup)
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
	Win32DebugConsole(" In: ");
	Win32DebugConsole(File);
	Win32DebugConsole(" Line: ");
	Win32DebugConsole(LineNumber);
}

// File IO

#define BUFFERSIZE 4096
DWORD BytesTransferred = 0;

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK FileIOCompletionRoutine(
	__in  DWORD dwErrorCode,
	__in  DWORD dwNumberOfBytesTransfered,
	__in  LPOVERLAPPED lpOverlapped )
{
	// _tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
	// _tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
	BytesTransferred = dwNumberOfBytesTransfered;
}

k_internal 
PLATFORM_GET_FILE_CONTENTS(Win32GetFileContents)
{
	k_string Result;

	HANDLE FileHandle; 
	DWORD  BytesRead = 0;
	char   ReadBuffer[BUFFERSIZE] = {0};
	OVERLAPPED Overlapped = {0};

	FileHandle = CreateFile(FileName.Data(),               // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
		NULL);                 // no attr. template

	if (FileHandle == INVALID_HANDLE_VALUE) 
	{ 
		// Win32Error((TEXT("Terminal failure: unable to open file \"%s\" for read.\n"), FileName.Data()), __FILE__, __LINE__);
		return Result; 
	}

	// Read one character less than the buffer size to save room for
	// the terminating NULL character. 

	if( FALSE == ReadFileEx(FileHandle, ReadBuffer, BUFFERSIZE - 1, &Overlapped, FileIOCompletionRoutine) )
	{
		// Win32Error((TEXT("Terminal failure: Unable to read from file.\n GetLastError=%08x\n"), GetLastError()), __FILE__, __LINE__);
		// Win32Error("Terminal failure: Unable to read from file.\n ", __FILE__, __LINE__);
		CloseHandle(FileHandle);
		return Result;
	}
	SleepEx(5000, TRUE);
	BytesRead = BytesTransferred;
	// This is the section of code that assumes the file is ANSI text. 
	// Modify this block for other data types if needed.

	if (BytesRead > 0 && BytesRead <= BUFFERSIZE - 1)
	{
		ReadBuffer[BytesRead] = '\0'; // NULL character

		// _tprintf(TEXT("Data read from %s (%d bytes): \n"), argv[1], BytesRead);
		// printf("%s\n", ReadBuffer);

		Result = k_string(ReadBuffer, BytesRead);
	}
	else if (BytesRead == 0)
	{
		// _tprintf(TEXT("No data read from file %s\n"), argv[1]);
	}
	else
	{
		// printf("\n ** Unexpected value for dwBytesRead ** \n");
	}

	// It is always good practice to close the open file handles even though
	// the app will exit here and clean up open handles anyway.

	CloseHandle(FileHandle);

	return Result;
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

/*


k_internal void
Win32CheckErrorCode(DWORD ErrorCode)
{
	LPVOID MessageBuffer;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&MessageBuffer,
		0, NULL);

	k_string Message((LPTSTR)MessageBuffer);
	Win32DebugOutput(Message);
}


k_internal PLATFORM_GET_FILE_FROM_NAME(Win32GetFileFromName)
{
	platform_file_handle Result = {};

	win32_platform_file_handle Win32File = {};

	TCHAR WorkingDirectory[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, WorkingDirectory);

	Win32File.Win32Handle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

	Result.Platform = (platform_file_handle *)&Win32File;

	return(Result);
}

*/

KantiRenderManager* Renderer;

k_internal void
Win32InitializeGameManagers()
{
	memory_index MemorySize = Megabytes(100);
	LPVOID MemoryAddress = 0;

	void* MemoryBlock = VirtualAlloc(MemoryAddress, MemorySize, MEM_COMMIT, PAGE_READWRITE);

	KantiMemoryManager::SetInstance(new KantiMemoryManager(MemorySize, MemoryBlock));

	KantiDebugManager::PlatformConsole = Win32DebugConsole;
	KantiDebugManager::PlatformError = Win32Error;

	KantiFileManager::PlatformGetFile = Win32GetFileContents;
	KantiFileManager::PlatformSetFile = Win32SetFileContents;

	KantiTimeManager::PlatformPerformanceCounter = Win32GetLastCounter;
	KantiTimeManager::PlatformCycleCount = Win32GetLastCycle;
	KantiTimeManager::PlatformPerformanceFrequency = Win32GetPerformanceFrequency;

	KantiInputManager::InputManager = (KantiInputManager *)MemAlloc(sizeof(KantiInputManager), 8);
	KantiInputManager::InputManager->Mapper = InputMapper();
	KantiInputManager::InputManager->Mapper.AddInputContext("maincontext");

	KantiRandomManager::Initialize(0 , true);

	KantiCameraManager::GetMainCamera()->Type = KantiCameraManager::FPS;
	KantiCameraManager::GetMainCamera()->SetPerspective(90.0f, 1280.0f / 720.0f, 0.001f, 256.0f);
}

k_internal LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{
			break;
		}

		case WM_CLOSE:
		{
			// TODO(Julian): Handle this as a message to the user
			GlobalRunning = false;
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
			GlobalRunning = false;
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

			int32 LastX = KantiInputManager::LastMouseX;
			int32 LastY = KantiInputManager::LastMouseY;
			KantiInputManager::InputManager->Mapper.SetRawAxisValue(RAW_INPUT_AXIS_MOUSE_X, real64(X -  LastX));
			KantiInputManager::InputManager->Mapper.SetRawAxisValue(RAW_INPUT_AXIS_MOUSE_Y, real64(Y -  LastY));

			KantiInputManager::LastMouseX = X;
			KantiInputManager::LastMouseY = Y;

			break;
		}

		case WM_KEYDOWN:
		{
			uint32 VKCode = (uint32)WParam;

			input_button_data Button;

			if(KantiInputManager::ConvertWindowsButton(VKCode, LParam, Button))
			{
				KantiInputManager::InputManager->Mapper.SetRawButtonState(Button);
			}

			break;
		}

		case WM_KEYUP:
		{
			uint32 VKCode = (uint32)WParam;

			input_button_data Button;

			if(KantiInputManager::ConvertWindowsButton(VKCode, LParam, Button))
			{
				KantiInputManager::InputManager->Mapper.SetRawButtonState(Button);
			}

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
			if((VKCode == VK_F4) && AltKeyWasDown)
			{
				GlobalRunning = false;
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
	Win32InitializeGameManagers();

	Quaternion Quat = {};

	Quat.X = 1.0f;
	Quat.Y = 2.0f;
	Quat.Z = 4.0f;

	Matrix4x4 Mat = Matrix4x4();
	Mat = Quaternion::ToMatrix(Quat);

	Quaternion Test0 = Quaternion::FromMatrix(Mat);

	KantiTimeManager::OnInitialize();

	WNDCLASSA WindowClass = {};

	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	// WindowClass.hIcon = ;
	WindowClass.lpszClassName = "KantiEngineClass";
	

	Win32Error("Something", __FILE__, __LINE__);
	Win32DebugConsole("Something");

	k_string FileName("Memes.txt");

	k_string Contents = Win32GetFileContents(FileName);

	k_string FileWrite("something to do with the memes");
	Win32SetFileContents(FileName, FileWrite);

	KList<vertex> Verts;
	KList <uint32> Indices;
	SetCubeVertex(Verts, Indices);

	if(RegisterClassA(&WindowClass))
	{
		HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "KantiEngine",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);

		if(Window)
		{
			GlobalRunning = true;

			renderer_platform Info = {};
			Info.Instance = Instance;
			Info.Window = Window;

			Renderer = (KantiRenderManager *)MemAlloc(sizeof(KantiRenderManager), 8);

			Renderer->InitializeRenderer = VulkanInitialize;
			Renderer->UpdateRenderer = VulkanUpdateAndRender;
			Renderer->InitializeRenderer(Info);

			KantiTimeManager::StartFrame();

			while(GlobalRunning)
			{
				MSG Message;
				while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}

					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				// Get Input
				{

					

					// KantiCameraManager::GetMainCamera()->Rotate(KantiInputManager::MouseDelta.X, Vector3(0, 1, 0));


					KantiCameraManager::GetMainCamera()->Update(KantiTimeManager::GetMSPerFrame());
				}

				// GameUpdateAndRender
				Renderer->UpdateRenderer();

				// Play sound

				char CharBuffer[100];
				sprintf(CharBuffer, "MSPerFrame: %fms/f - FPS: %ff/s - MCPF: %fmc/f\n",
					KantiTimeManager::GetMSPerFrame(), KantiTimeManager::GetFPS(), KantiTimeManager::GetMCPF());
				Win32DebugConsole(CharBuffer);

				/*
				char CharMouseBuffer[100];
				sprintf(CharMouseBuffer, "Mouse Delta X: %f - Mouse Delta Y: %f \n\n",
					KantiInputManager::MouseDelta.X, KantiInputManager::MouseDelta.Y);
				Win32DebugConsole(CharMouseBuffer);

				// KantiInputManager::ResetKeys();
				*/

				
				// TODO(Julian): Move this into a global end frame
				KantiInputManager::InputManager->Mapper.Dispatch();
				KantiInputManager::InputManager->Mapper.Clear();

				KantiTimeManager::EndFrame();
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