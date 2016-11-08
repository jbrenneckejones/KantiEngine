#include "KantiEngine.h"


k_internal void
MemCopy(void* Destination, void* Source, memory_index Size)
{
	KEngine::EngineInstance->KMemory->MemoryCopy(Destination, Source, Size);
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
MemRealloc(void* Pointer, memory_index SourceSize, memory_index NewSize, uint8 Alignment)
{
	return KEngine::EngineInstance->KMemory->ReAllocate(Pointer, SourceSize, NewSize, Alignment);
}

k_internal void
MemDealloc(void* Pointer)
{
	KEngine::EngineInstance->KMemory->Deallocate(Pointer);
}

// Vulkan
#ifdef VULKAN
#include "KantiVulkan/KantiVulkan.cpp"

#include "KantiVulkan/VulkanBuffer.cpp"
#include "KantiVulkan/VulkanCommandBuffer.cpp"
#include "KantiVulkan/VulkanDebug.cpp"
#include "KantiVulkan/VulkanEncapsulatedDevice.cpp"
#include "KantiVulkan/VulkanSwapChain.cpp"

extern "C" PLATFORM_GAME_RENDER_INITIALIZE(VulkanInitialize)
{
	vulkan_base_properties Properties = {};
	Properties.ApplicationName = "Kanti Engine";
	Properties.DestinationHeight = 1280;
	Properties.DestinationWidth = 720;
	Properties.EnableDebugMarkers = true;
	// Properties.EnabledFeatures = ;
	Properties.EnableValidation = true;
	Properties.EnableVSync = false;

	VulkanRendererInstance = new VulkanRenderer(Platform, Properties);
	VulkanRendererInstance->Prepare();

	return;
}

extern "C" PLATFORM_GAME_UPDATE_AND_RENDER(VulkanUpdateAndRender)
{
	VulkanRendererInstance->Render();
	return;
}

extern "C" PLATFORM_CREATE_MESH_BUFFER(VulkanCreateMeshBuffer)
{
	VulkanRendererInstance->CreateMeshBuffer(MeshRenderer);
	return;
}

#include "KantiManagers/KantiRenderManager.cpp"
#include "KantiVulkan/VulkanRenderer.cpp"

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

	KString Stuff = "lllslsls";
	Stuff += "skkkdkk";

	char CharBuffer[100];
	sprintf_s(CharBuffer, "MSPerFrame: %fms/f - FPS: %ff/s - MCPF: %fmc/f\n",
		KantiTimeManager::GetMSPerFrame(), KantiTimeManager::GetFPS(), KantiTimeManager::GetMCPF());
	// KantiDebugManager::DebugConsole(CharBuffer);

	// TODO(Julian): Move this into a global end frame
	KInput.Mapper.Dispatch();
	KInput.Mapper.Clear();

	KantiTimeManager::EndFrame();
}
