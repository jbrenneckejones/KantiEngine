#ifndef KANTI_ENGINE

#define ENGINE_VERSION 001

#include "KantiPlatform.h"

k_internal void*
MemCopy(void* Destination, void* Source, memory_index Size);

k_internal void
MemMove(void** Destination, void** Source, memory_index Size, uint8 Alignment = 4);

k_internal void*
MemAlloc(memory_index MemorySize, uint8 Alignment = 4);

k_internal void*
MemRealloc(void* Pointer, memory_index SourceSize, memory_index NewSize, uint8 Alignment = 4);

k_internal void
MemDealloc(void* Pointer);

#include "KantiMath.h"

// Time

#define PLATFORM_GET_PERFORMANCE_FREQUENCY(name) int64 name()
typedef PLATFORM_GET_PERFORMANCE_FREQUENCY(platform_get_performance_frequency);

#define PLATFORM_GET_PERFORMANCE_COUNTER(name) int64 name()
typedef PLATFORM_GET_PERFORMANCE_COUNTER(platform_get_performance_counter);

#define PLATFORM_GET_CYCLE_COUNTER(name) uint64 name()
typedef PLATFORM_GET_CYCLE_COUNTER(platform_get_cycle_count);

// Files

#define PLATFORM_GET_FILE_CONTENTS(name) KString name(KString FileName)
typedef PLATFORM_GET_FILE_CONTENTS(platform_get_file_contents);

#define PLATFORM_SET_FILE_CONTENTS(name) void name(KString FileName, KString Contents)
typedef PLATFORM_SET_FILE_CONTENTS(platform_set_file_contents);

// TODO(Julian): More file editing

// Memory

#define PLATFORM_ALLOCATE_MEMORY(name) void* name(int64 Address, memory_index Size, int32 MemoryType, int32 MemoryAccess)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void *Memory)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory);

// Debug

#define PLATFORM_GAME_ERROR(name) void name(KString Message, KString File, uint32 LineNumber)
typedef PLATFORM_GAME_ERROR(platform_game_error);

#define PLATFORM_GAME_DEBUG_CONSOLE(name) void name(KString DebugMessage)
typedef PLATFORM_GAME_DEBUG_CONSOLE(platform_debug_console);

struct renderer_platform
{
	void* Window;
	void* Instance;
};

struct KVertex
{
	KVector3 Position;
	KVector2 UV;
	KVector3 Color;
	KVector3 Normal;

	bool32 operator==(const KVertex& Comparer) const 
	{
		return  Position == Comparer.Position &&
				Color == Comparer.Color &&
				UV == Comparer.UV &&
				Normal == Comparer.Normal;
	}
};

namespace std 
{
	template<> struct hash<KVector2>
	{
		memory_index operator()(const KVector2& Vector) const
		{
			return ((hash<real32>()(Vector.X) ^ (hash<real32>()(Vector.Y) << 1)) >> 1);
		}
	};

	template<> struct hash<KVector3>
	{
		memory_index operator()(const KVector3& Vector) const
		{
			return ((hash<real32>()(Vector.X) ^ (hash<real32>()(Vector.Y) << 1)) >> 1) ^ (hash<real32>()(Vector.Z) << 1);
		}
	};

	template<> struct hash<KVertex>
	{
		memory_index operator()(const KVertex& Vertex) const
		{
			return ((hash<KVector3>()(Vertex.Position) ^ (hash<KVector3>()(Vertex.Color) << 1)) >> 1) ^ (hash<KVector2>()(Vertex.UV) << 1);
		}
	};
}


struct KMatrixData
{
	KMatrix4x4 Model;
	KMatrix4x4 View;
	KMatrix4x4 Projection;	
};

struct KMeshData
{
	KList<KVertex> Vertices;
	KList<uint32> Indices;
};

struct KImageData
{
	void* Data;
	int32 Width;
	int32 Height;
	int32 Channels;
	memory_index Size;
};

#define PLATFORM_GAME_RENDER_INITIALIZE(name) void name(renderer_platform& Platform)
typedef PLATFORM_GAME_RENDER_INITIALIZE(platform_game_render_initialize);

#define PLATFORM_GAME_UPDATE_AND_RENDER(name) void name()
typedef PLATFORM_GAME_UPDATE_AND_RENDER(platform_game_update_and_render);

#define PLATFORM_CREATE_MESH_BUFFER(name) void name(class KMeshRenderer* MeshRenderer)
typedef PLATFORM_CREATE_MESH_BUFFER(platform_create_mesh_buffer);

#include "KantiManagers/KantiConversionManager.h"
#include "KantiManagers/KantiMemoryManager.h"
#include "KantiManagers/KantiDebugManager.h"

// TODO(Julian): Maybe remove this
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobj/tiny_obj_loader.h"
#include "KantiManagers/KantiFileManager.h"

// OpenGL
#ifdef OPENGL

#define GLEW_STATIC
#include <gl/glew.h>
#include <glfw3.h>

#endif

#include "KantiManagers/KantiRenderManager.h"
#include "KantiManagers/KantiTimeManager.h"
#include "KantiManagers/KantiInputManager.h"
#include "KantiManagers/KantiCameraManager.h"

#include "KantiManagers/KantiRandomManager.h"
#include "KantiManagers/KantiSoundManager.h"

struct PlatformFunctionHandles
{
	// Unique ID
	platform_get_uuid* GetUUID;

	// Memory
	platform_allocate_memory* PlatforMemAllocateMemory;

	// Debug
	platform_debug_console* ConsoleOutput;
	platform_game_error* ErrorOutput;

	// File
	platform_get_file_contents* GetFileContents;
	platform_set_file_contents* SetFileContents;

	// Timing
	platform_get_performance_counter* GetPerformanceCounter;
	platform_get_performance_frequency* GetPerformanceFrequency;
	platform_get_cycle_count* GetCycleCounter;
};

class KEngine
{
public:

	bool32 IsRunning;
	k_internal KEngine* EngineInstance;

	KantiMemoryManager* KMemory;
	KantiInputManager KInput;
	KantiSoundManager KSound;
	KantiRenderManager KRenderer;

	// Temp
	KList<KMeshRenderer*> Meshes;

public:

	KEngine()
	{
		if (!EngineInstance)
		{
			EngineInstance = this;
		}
	}

	void InitializeGameManager(PlatformFunctionHandles PlatformHandles)
	{
		KantiMemoryManager::PlatformMemoryAllocate = PlatformHandles.PlatforMemAllocateMemory;

		memory_index MemorySize = Megabytes(100);
		int64 MemoryAddress = 0;

		// #define MEM_COMMIT                  0x1000 
		// #define PAGE_READWRITE         0x04
		void* MemoryBlock = KantiMemoryManager::PlatformMemoryAllocate(MemoryAddress, MemorySize, 0x1000, 0x04);
		KMemory = new KantiMemoryManager(MemorySize, MemoryBlock);

		KInput.Mapper.AddInputContext("maincontext");

		KantiDebugManager::PlatformConsole = PlatformHandles.ConsoleOutput;
		KantiDebugManager::PlatformError = PlatformHandles.ErrorOutput;

		KantiFileManager::PlatformGetFile = PlatformHandles.GetFileContents;
		KantiFileManager::PlatformSetFile = PlatformHandles.SetFileContents;

		KantiTimeManager::PlatformPerformanceCounter = PlatformHandles.GetPerformanceCounter;
		KantiTimeManager::PlatformCycleCount = PlatformHandles.GetCycleCounter;
		KantiTimeManager::PlatformPerformanceFrequency = PlatformHandles.GetPerformanceFrequency;

		KantiRandomManager::Initialize(0, true);

		KantiCameraManager::GetMainCamera()->Type = KantiCameraManager::FPS;
		KantiCameraManager::GetMainCamera()->SetPerspective(90.0f, 1280.0f / 720.0f, 0.001f, 256.0f);
		KInput.Mapper.PushContext("maincontext");
		KInput.Mapper.AddCallback(KantiCameraManager::GetMainCamera()->Input, 0);
	}

	void InitializeWindow(renderer_platform RenderInfo)
	{
		KQuaternion Quat = {};

		Quat.X = 1.0f;
		Quat.Y = 2.0f;
		Quat.Z = 4.0f;

		KMatrix4x4 Mat = KMatrix4x4();
		Mat = KQuaternion::ToMatrix(Quat);

		KQuaternion Test0 = KQuaternion::FromMatrix(Mat);

		KantiTimeManager::OnInitialize();

		/*
		Win32Error("Something", __FILE__, __LINE__);
		Win32DebugConsole("Something");

		k_string FileName("Memes.txt");

		k_string Contents = Win32GetFileContents(FileName);

		k_string FileWrite("something to do with the memes");
		Win32SetFileContents(FileName, FileWrite);

		KList<vertex> Verts;
		KList <uint32> Indices;
		SetCubeVertex(Verts, Indices);

		KantiSoundManager Sound = KantiSoundManager();
		*/

		// Link to OpenGL or Vulkan
		KRenderer.Initialize();
		KRenderer.PlatformInitializeRenderer(RenderInfo);

		KantiTimeManager::StartFrame();

		IsRunning = true;
	}

	void EngineLoop();

	void SetMouseInput(int32 X, int32 Y)
	{
		int32 LastX = KInput.LastMouseX;
		int32 LastY = KInput.LastMouseY;
		KInput.Mapper.SetRawAxisValue(RAW_INPUT_AXIS_MOUSE_X, real64(X - LastX));
		KInput.Mapper.SetRawAxisValue(RAW_INPUT_AXIS_MOUSE_Y, real64(Y - LastY));

		KInput.LastMouseX = X;
		KInput.LastMouseY = Y;
	}

	void SetKeyboardInput(uint32 VKCode, int64 LParam)
	{
		input_button_data Button;

		if (KInput.ConvertWindowsButton(VKCode, LParam, Button))
		{
			KInput.Mapper.SetRawButtonState(Button);
		}
	}
	
};

KEngine* KEngine::EngineInstance = nullptr;

#include "KantiGameObjects.h"

#define KANTI_ENGINE
#endif

