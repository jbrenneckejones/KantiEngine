#ifndef KANTI_ENGINE

#include "KantiPlatform.h"
#include "KantiMath.h"
#include "KantiList.h"

// Time

#define PLATFORM_GET_PERFORMANCE_FREQUENCY(name) int64 name()
typedef PLATFORM_GET_PERFORMANCE_FREQUENCY(platform_get_performance_frequency);

#define PLATFORM_GET_PERFORMANCE_COUNTER(name) int64 name()
typedef PLATFORM_GET_PERFORMANCE_COUNTER(platform_get_performance_counter);

#define PLATFORM_GET_CYCLE_COUNTER(name) uint64 name()
typedef PLATFORM_GET_CYCLE_COUNTER(platform_get_cycle_count);

// Files

#define PLATFORM_GET_FILE_CONTENTS(name) k_string name(k_string FileName)
typedef PLATFORM_GET_FILE_CONTENTS(platform_get_file_contents);

#define PLATFORM_SET_FILE_CONTENTS(name) void name(k_string FileName, k_string Contents)
typedef PLATFORM_SET_FILE_CONTENTS(platform_set_file_contents);

// TODO(Julian): More file editing

// Memory

#define PLATFORM_ALLOCATE_MEMORY(name) void* name(memory_index Size)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(void *Memory)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory);

// Debug

#define PLATFORM_GAME_ERROR(name) void name(k_string Message, k_string File, uint32 LineNumber)
typedef PLATFORM_GAME_ERROR(platform_game_error);

#define PLATFORM_GAME_DEBUG_CONSOLE(name) void name(k_string DebugMessage)
typedef PLATFORM_GAME_DEBUG_CONSOLE(platform_debug_console);

// 3D Rendering

struct renderer_platform
{
	void* Window;
	void* Instance;
};

struct vertex
{
	Vector3 Position;
	Vector3 Color;
};

struct matrix_data
{
	Matrix4x4 Projection;
	Matrix4x4 Model;
	Matrix4x4 View;
};

struct platform_buffer_data
{
	void* UniformData;

	void* VertexBuffer;
	void* IndexBuffer;

	uint32 IndexCount;
};

struct render_data
{
	KList<vertex> Vertices;
	KList<uint32> Indices;
};

struct k_object
{
	Vector3 Position;
	Vector3 Rotation;
	Vector3 Scale;

	Vector3 Color;

	matrix_data UniformBuffer;
	platform_buffer_data* BufferData;

	render_data MeshData;
};

#define PLATFORM_GAME_RENDER_INITIALIZE(name) void name(renderer_platform& Platform)
typedef PLATFORM_GAME_RENDER_INITIALIZE(platform_game_render_initialize);

#define PLATFORM_GAME_UPDATE_AND_RENDER(name) void name()
typedef PLATFORM_GAME_UPDATE_AND_RENDER(platform_game_update_and_render);

#define KANTI_ENGINE
#endif

