#include "KantiRenderManager.h"

void KantiRenderManager::Initialize()
{
	// Vulkan
#ifdef VULKAN
	PlatformInitializeRenderer = VulkanInitialize;
	PlatformRenderer = VulkanUpdateAndRender;
	PlatformCreateMeshBuffer = VulkanCreateMeshBuffer;
#endif

	// OpenGL
#ifdef OPENGL
	PlatformInitializeRenderer = OpenGLInitialize;
	PlatformRenderer = OpenGLUpdateAndRender;
	PlatformCreateMeshBuffer = OpenGLCreateMeshBuffer;
#endif
}
