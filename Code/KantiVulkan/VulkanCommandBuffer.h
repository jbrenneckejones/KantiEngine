#ifndef VULKAN_COMMAND_BUFFER

#include "KantiVulkan.h"

struct commandbuffer_render_info
{
	VkClearColorValue Color;
	uint32 Width;
	uint32 Height;
	VkPipeline Pipeline;
	VkPipelineLayout PipelineLayout;
};

struct framebuffer_render_info
{
	VkDevice Device;
	uint32 Width;
	uint32 Height;
	VkImageView View;
};

struct image_layout_info
{
	VkCommandBuffer CommandBuffer;
	VkImage Image;
	VkImageAspectFlags AspectMask;
	VkImageLayout OldImageLayout;
	VkImageLayout NewImageLayout;
	VkImageSubresourceRange SubresourceRange;
};

class VulkanCommandBuffer
{
	public:

	// Global render pass for frame buffer writes
	VkRenderPass RenderPass;

	// Command buffer pool
	VkCommandPool CommandPool;


	// Descriptor set pool
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;

	// Color buffer format
	VkFormat Colorformat = VK_FORMAT_B8G8R8A8_UNORM;
	// Depth buffer format
	// Depth format is selected during Vulkan initialization
	VkFormat DepthFormat;

	// Command buffer used for setup
	VkCommandBuffer SetupCommandBuffer = VK_NULL_HANDLE;
	// Command buffer for submitting a post present image barrier
	KList<VkCommandBuffer> PostPresentCommandBuffers;
	// Command buffers for submitting a pre present image barrier
	KList<VkCommandBuffer> PrePresentCommandBuffers;
	// Command buffers used for rendering
	KList<VkCommandBuffer> DrawCommandBuffers;

	uint32 FrameBuffersCount = 3;
	// List of available frame buffers (same as number of swap chain images)
	KList<VkFramebuffer> FrameBuffers;
	// Active frame buffer index
	uint32 CurrentBuffer = 0;

	VkCommandBuffer* GetCurrentDrawBuffer();

	// Pure virtual function to be overriden by the dervice class
	// Called in case of an event where e.g. the framebuffer has to be rebuild and thus
	// all command buffers that may reference this
	void BuildCommandBuffers(commandbuffer_render_info& Info, KList<k_object> Objects);

	// Builds the command buffers used to submit the present barriers
	void BuildPresentCommandBuffers(class VulkanSwapChain* SwapChain);

	// Creates a new (graphics) command pool object storing command buffers
	void CreateCommandPool(class VulkanSwapChain* SwapChain, VkDevice Device);

	// Create framebuffers for all requested swap chain images
	// Can be overriden in derived class to setup a custom framebuffer (e.g. for MSAA)
	void SetupFrameBuffer(framebuffer_render_info& Info, class VulkanSwapChain* SwapChain);

	// Check if command buffers are valid (!= VK_NULL_HANDLE)
	bool32 CheckCommandBuffers();
	// Create command buffers for drawing commands
	void CreateCommandBuffers(VkDevice Device, uint32 ImageCount);
	// Destroy all command buffers and set their handles to VK_NULL_HANDLE
	// May be necessary during runtime if options are toggled 
	void DestroyCommandBuffers(VkDevice Device);
	// Create command buffer for setup commands
	void CreateSetupCommandBuffer(VkDevice Device);
	// Finalize setup command bufferm submit it to the queue and remove it
	void FlushSetupCommandBuffer(VkDevice Device, VkQueue Queue);

	// Command buffer creation
	// Creates and returns a new command buffer
	VkCommandBuffer CreateCommandBuffer(VkDevice Device, VkCommandBufferLevel Level, bool32 Begin);
	// End the command buffer, submit it to the queue and free (if requested)
	// Note : Waits for the queue to become idle
	void FlushCommandBuffer(VkCommandBuffer CommandBuffer, VkQueue VulkanQueue, bool32 FreeBuffer, VkDevice Device);


	// Create an image memory barrier for changing the layout of
	// an image and put it into an active command buffer
	// See chapter 11.4 "Image Layout" for details
	void SetImageLayout(image_layout_info& Info, bool32 FixedSubresource = false);
};

#define VULKAN_COMMAND_BUFFER
#endif