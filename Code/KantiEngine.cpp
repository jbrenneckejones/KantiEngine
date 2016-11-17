#include "KantiEngine.h"


k_internal void*
MemCopy(void* Destination, void* Source, memory_index Size)
{
	return KEngine::EngineInstance->KMemory->MemoryCopy(Destination, Source, Size);
}

k_internal void
MemMove(void** Destination, void** Source, memory_index Size, uint8 Alignment)
{
	KEngine::EngineInstance->KMemory->MemoryMove(Destination, Source, Size, Alignment);
}

k_internal void*
MemAlloc(memory_index MemorySize, uint8 Alignment)
{
	return KEngine::EngineInstance->KMemory->Allocate(MemorySize, Alignment);
}

k_internal void*
MemRealloc(void* Source, memory_index SourceSize, memory_index NewSize, uint8 Alignment)
{
	return KEngine::EngineInstance->KMemory->ReAllocate(Source, SourceSize, NewSize, Alignment);
}

k_internal void
MemDealloc(void* Source)
{
	KEngine::EngineInstance->KMemory->Deallocate(Source);
}

k_internal void
MemSet(void* Source, int32 FillByte, memory_index BytesToFill)
{
	KEngine::EngineInstance->KMemory->MemorySet(Source, FillByte, BytesToFill);
}

// Vulkan
#ifdef VULKAN

#include "vulkan/vulkan.h"
#include "KantiVulkan/KantiVulkan.h"

extern "C" PLATFORM_GAME_RENDER_INITIALIZE(VulkanInitialize)
{
	Vulkan = new VulkanRenderer(Platform);
	return;
}

extern "C" PLATFORM_GAME_UPDATE_AND_RENDER(VulkanUpdateAndRender)
{
	Vulkan->Render();

	return;
}

extern "C" PLATFORM_CREATE_MESH_BUFFER(VulkanCreateMeshBuffer)
{
	// Vulkan->CreateMeshBuffer(MeshRenderer);
	return;
}

#include "KantiManagers/KantiRenderManager.cpp"

#endif


// OpenGL
#ifdef OPENGL
extern "C" PLATFORM_GAME_RENDER_INITIALIZE(OpenGLInitialize)
{

	return;
}

extern "C" PLATFORM_GAME_UPDATE_AND_RENDER(OpenGLUpdateAndRender)
{
	return;
}

extern "C" PLATFORM_CREATE_MESH_BUFFER(OpenGLCreateMeshBuffer)
{
	return;
}

#include "KantiManagers/KantiRenderManager.cpp"
#endif

void KEngine::EngineLoop()
{
	// Get Input
	{
		KantiCameraManager::GetMainCamera()->Update(KantiTimeManager::GetMSPerFrame());
	}

	for (uint32 Index = 0; Index < Meshes.Count(); ++Index)
	{
		Meshes[Index]->Update();
	}

	// GameUpdateAndRender
	KRenderer.Render();

	// Play sound

	char CharBuffer[100];
	sprintf_s(CharBuffer, "MSPerFrame: %fms/f - FPS: %ff/s - MCPF: %fmc/f\n",
		KantiTimeManager::GetMSPerFrame(), KantiTimeManager::GetFPS(), KantiTimeManager::GetMCPF());
	// KantiDebugManager::DebugConsole(CharBuffer);

	// TODO(Julian): Move this into a global end frame
	KInput.Mapper.Dispatch();
	KInput.Mapper.Clear();

	KantiTimeManager::EndFrame();
}
