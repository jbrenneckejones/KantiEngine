#ifndef KANTI_VULKAN

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif


#define VK_CHECK_RESULT(Result) { Assert(Result == VK_SUCCESS); }

#define VK_FLAGS_NONE 0

#define DEFAULT_FENCE_TIMEOUT 100000000000

class VulkanRenderer* VulkanRendererInstance;


#define KANTI_VULKAN
#endif