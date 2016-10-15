#ifndef KANTI_VULKAN

#include "KantiPlatform.h"
#include "KantiRenderPlatform.h"
#include "vulkan/vulkan.h"

// TODO(Julian): Remove this eventually
#include <vector>

#define VK_FLAGS_NONE 0
#define DEFAULT_FENCE_TIMEOUT 100000

#define VK_CHECK_RESULT(Function)																		\
{																										\
	VkResult Result = (Function);																		\
	if (Result != VK_SUCCESS)																			\
	{																									\
		Assert(Result == VK_SUCCESS);																	\
	}																									\
}

/* Number of descriptor sets needs to be the same at alloc,       */
/* pipeline layout creation, and descriptor set layout creation   */
#define NUM_DESCRIPTOR_SETS 1

/* Number of samples needs to be the same at image creation,      */
/* renderpass creation and pipeline creation.                     */
#define NUM_SAMPLES VK_SAMPLE_COUNT_1_BIT

/* Number of viewports and number of scissors have to be the same */
/* at pipeline creation and in any call to set them dynamically   */
/* They also have to be the same as each other                    */
#define NUM_VIEWPORTS 1
#define NUM_SCISSORS NUM_VIEWPORTS

/* Amount of time, in nanoseconds, to wait for a command buffer to complete */
#define FENCE_TIMEOUT 100000000

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
	{                                                                          \
		info.fp##entrypoint =                                                  \
			(PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
		if (info.fp##entrypoint == NULL) {                                     \
			std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
			exit(-1);                                                          \
		}                                                                      \
	}

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                  \
	{                                                                          \
		info.fp##entrypoint =                                                  \
			(PFN_vk##entrypoint)vkGetDeviceProcAddr(dev, "vk" #entrypoint);    \
		if (info.fp##entrypoint == NULL) {                                     \
			std::cout << "vkGetDeviceProcAddr failed to find vk" #entrypoint;  \
			exit(-1);                                                          \
		}                                                                      \
	}

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef NOMINMAX
#define NOMINMAX /* Don't let Windows define min() or max() */
#endif

#define CheckVKResult(Result, String)          \
{                                              \
	OutputDebugStringA((LPCSTR)Result);        \
	OutputDebugStringA(" ");                   \
	OutputDebugStringA(String);                \
	OutputDebugStringA("\n");                  \
}                                              \

/*
* structure to track all objects related to a texture.
*/
struct texture_object
{
	VkSampler sampler;

	VkImage image;
	VkImageLayout imageLayout;

	VkDeviceMemory mem;
	VkImageView view;
	int32_t tex_width, tex_height;
};

/*
* Keep each of our swap chain buffers' image, command buffer and view in one
* spot
*/
typedef struct _swap_chain_buffers
{
	VkImage image;
	VkImageView view;
} swap_chain_buffer;

/*
* A layer can expose extensions, keep track of those
* extensions here.
*/
typedef struct
{
	VkLayerProperties properties;
	std::vector<VkExtensionProperties> extensions;
} layer_properties;

struct renderer_data
{
	HINSTANCE connection;        // hInstance - Windows Instance
	char name[80];               // Name to put on the window/icon
	HWND window;                 // hWnd - window handle

	VkSurfaceKHR surface;
	bool32 prepared;
	bool32 use_staging_buffer;
	bool32 save_images;

	std::vector<const char*> instance_layer_names;
	std::vector<const char*> instance_extension_names;
	std::vector<layer_properties> instance_layer_properties;
	std::vector<VkExtensionProperties> instance_extension_properties;
	VkInstance inst;

	std::vector<const char*> device_extension_names;
	std::vector<VkExtensionProperties> device_extension_properties;
	std::vector<VkPhysicalDevice> gpus;
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	uint32 graphics_queue_family_index;
	uint32 present_queue_family_index;
	VkPhysicalDeviceProperties gpu_props;
	std::vector<VkQueueFamilyProperties> queue_props;
	VkPhysicalDeviceMemoryProperties memory_properties;

	VkFramebuffer *framebuffers;
	int32 width, height;
	VkFormat format;

	uint32 swapchainImageCount;
	VkSwapchainKHR swap_chain;
	std::vector<swap_chain_buffer> buffers;
	VkSemaphore imageAcquiredSemaphore;

	VkCommandPool cmd_pool;

	struct
	{
		VkFormat format;

		VkImage image;
		VkDeviceMemory mem;
		VkImageView view;
	} depth;

	struct texture_object* textures;

	struct
	{
		VkBuffer buf;
		VkDeviceMemory mem;
		VkDescriptorBufferInfo buffer_info;
	} uniform_data;

	struct
	{
		VkDescriptorImageInfo image_info;
	} texture_data;
	VkDeviceMemory stagingMemory;
	VkImage stagingImage;

	struct
	{
		VkBuffer buf;
		VkDeviceMemory mem;
		VkDescriptorBufferInfo buffer_info;
	} vertex_buffer;
	VkVertexInputBindingDescription vi_binding;
	VkVertexInputAttributeDescription vi_attribs[2];

	matrix4x4 Projection;
	matrix4x4 View;
	matrix4x4 Model;
	matrix4x4 Clip;
	matrix4x4 MVP;

	VkCommandBuffer cmd; // Buffer for initialization commands
	VkPipelineLayout pipeline_layout;
	std::vector<VkDescriptorSetLayout> desc_layout;
	VkPipelineCache pipelineCache;
	VkRenderPass render_pass;
	VkPipeline pipeline;

	VkPipelineShaderStageCreateInfo shaderStages[2];

	VkDescriptorPool desc_pool;
	VkDescriptorSet* desc_set;

	PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
	PFN_vkDebugReportMessageEXT dbgBreakCallback;
	std::vector<VkDebugReportCallbackEXT> debug_report_callbacks;

	uint32 current_buffer;
	uint32 queue_family_count;

	VkViewport viewport;
	VkRect2D scissor;
};

VkResult VulkanInitializeGlobalExtensionProperties(layer_properties& LayerProperties);

VkResult VulkanInitializeGlobalLayerProperties(renderer_data& Info);

void VulkanInitializeInstance();

void VulkanInitialize(void* Handle, void* Window, renderer_data& Renderer);

void VulkanUpdateAndRender(real32 Time, renderer_data& Renderer);

#define KANTI_VULKAN
#endif
