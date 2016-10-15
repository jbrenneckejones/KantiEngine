#include "../KantiFileManager.h"
#include "KantiVulkan.h"
#include "VulkanSwapChain.h"
#include "VulkanSwapChain.cpp"
#include "VulkanBuffer.h"
#include "VulkanBuffer.cpp"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandBuffer.cpp"
#include "VulkanDebug.h"
#include "VulkanDebug.cpp"
#include "VulkanEncapsulatedDevice.h"
#include "VulkanEncapsulatedDevice.cpp"
#include "../KantiRenderManager.h"
#include "../KantiRenderManager.cpp"
#include "../KantiFileManager.h"
#include "../KantiInputManager.h"
#include "../KantiRandomManager.h"
#include "../KantiCameraManager.h"

struct vulkan_base_properties
{
	// Set to true when example is created with enabled validation layers
	bool32 EnableValidation = false;
	// Set to true when the debug marker extension is detected
	bool32 EnableDebugMarkers = false;
	// Set to true if v-sync will be forced for the swapchain
	bool32 EnableVSync = false;
	// Device features enabled by the example
	// If not set, no additional features are enabled (may result in validation layer errors)
	VkPhysicalDeviceFeatures EnabledFeatures = {};
	/** brief Indicates that the view (position, rotation) has changed and */
	bool32 ViewUpdated = false;
	// Destination dimensions for resizing the window
	uint32 DestinationWidth;
	uint32 DestinationHeight;

	k_string ApplicationName;
};

struct vulkan_rendering_properties
{
	bool32 Prepared = false;
	uint32 Width = 1280;
	uint32 Height = 720;

	VkClearColorValue DefaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

	bool32 Paused = false;

	// List of shader modules created (stored for cleanup)
	KList<VkShaderModule> ShaderModules;

	// Pipeline stage flags for the submit info structure
	VkPipelineStageFlags SubmitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

	// Pipeline cache object
	VkPipelineCache PipelineCache;

	VkPipelineLayout PipelineLayout;

	// Create a cache pool for rendering pipelines
	void CreatePipelineCache(VkDevice Device)
	{
		VkPipelineCacheCreateInfo PipelineCacheCreateInfo = {};
		PipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreatePipelineCache(Device, &PipelineCacheCreateInfo, nullptr, &PipelineCache));
	}
};

class VulkanRenderer
{
	public:

	KList<KantiRenderObject*> Meshes;

	VulkanEncapsulatedDevice* Device;

	VulkanSwapChain SwapChain;

	VulkanCommandBuffer CommandBuffer;

	VulkanDebugLayer Debug;

	vulkan_base_properties BaseProperties;

	vulkan_rendering_properties RenderProperties;

	// Synchronization semaphores
	struct
	{
// Swap chain image presentation
		VkSemaphore PresentComplete;
		// Command buffer submission and execution
		VkSemaphore RenderComplete;
	} Semaphores;

	struct
	{
		VkPipelineVertexInputStateCreateInfo InputState;
		KList<VkVertexInputBindingDescription> BindingDescriptions;
		KList<VkVertexInputAttributeDescription> AttributeDescriptions;
	} Vertices;

	struct
	{
		VkPipeline Solid;
	} Pipelines;

	// Vulkan instance, stores all per-application states
	VkInstance Instance;
	// Handle to the device graphics queue that command buffers are submitted to
	VkQueue Queue;
	// Contains command buffers and semaphores to be presented to the queue
	VkSubmitInfo SubmitInfo;

	VkDescriptorSetLayout DescriptorSetLayout;

	struct
	{
		VkImage Image;
		VkDeviceMemory Memory;
		VkImageView View;
	} DepthStencil;

	VulkanRenderer(renderer_platform& Platform, vulkan_base_properties& Properties)
	{
		// Window = (HWND)Platform.Window;
		// WindowInstance = (HINSTANCE)Platform.Instance;

		Device = new VulkanEncapsulatedDevice();
		Device->Platform = &Platform;

		// Check for validation command line flag
		/*
		for (int32 i = 0; i < __argc; i++)
		{
		if (__argv[i] == std::k_string("-validation"))
		{
		enableValidation = true;
		}
		if (__argv[i] == std::k_string("-vsync"))
		{
		enableVSync = true;
		}
		}
		*/

		BaseProperties = Properties;

		// Enable console if validation is active
		// Debug message callback will output to it
		if(Properties.EnableValidation)
		{
			// setupConsole("VulkanExample");
		}

		InitializeVulkan(Properties.EnableValidation);
	}

	// Create application wide Vulkan instance
	VkResult CreateInstance(bool32 VulkanEnableValidation)
	{
		BaseProperties.EnableValidation = VulkanEnableValidation;

		VkApplicationInfo ApplicationInfo = {};
		ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		ApplicationInfo.pApplicationName = BaseProperties.ApplicationName.Data();
		ApplicationInfo.pEngineName = BaseProperties.ApplicationName.Data();
		ApplicationInfo.apiVersion = VK_API_VERSION_1_0;

		KList<const char*> EnabledExtensions;
		EnabledExtensions.PushBack(VK_KHR_SURFACE_EXTENSION_NAME);
		EnabledExtensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		VkInstanceCreateInfo InstanceCreateInfo = {};
		InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstanceCreateInfo.pNext = NULL;
		InstanceCreateInfo.pApplicationInfo = &ApplicationInfo;
		if(EnabledExtensions.Count() > 0)
		{
			if(VulkanEnableValidation)
			{
				EnabledExtensions.PushBack(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			}
			InstanceCreateInfo.enabledExtensionCount = EnabledExtensions.Count();
			InstanceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.Data();
		}
		if(VulkanEnableValidation)
		{
			KList<const char*> ValidationLayerNames;
			// This is a meta layer that enables all of the standard
			// validation layers in the correct order :
			// threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
			ValidationLayerNames.PushBack("VK_LAYER_LUNARG_standard_validation");

			InstanceCreateInfo.enabledLayerCount = ValidationLayerNames.Count();
			InstanceCreateInfo.ppEnabledLayerNames = ValidationLayerNames.Data();
		}
		return vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance);
	}

	// Returns the base asset path (for shaders, models, textures) depending on the os
	k_string GetAssetPath()
	{
		return "./../Data/";
	}

	// Called if the window is resized and some resources have to be recreatesd
	void WindowResize()
	{
		if(!RenderProperties.Prepared)
		{
			return;
		}
		RenderProperties.Prepared = false;

		// Recreate swap chain
		RenderProperties.Width = BaseProperties.DestinationWidth;
		RenderProperties.Height = BaseProperties.DestinationHeight;
		CommandBuffer.CreateSetupCommandBuffer(Device->LogicalDevice);
		CreateSwapChainImages();

		// Recreate the frame buffers

		vkDestroyImageView(Device->LogicalDevice, DepthStencil.View, nullptr);
		vkDestroyImage(Device->LogicalDevice, DepthStencil.Image, nullptr);
		vkFreeMemory(Device->LogicalDevice, DepthStencil.Memory, nullptr);
		SetupDepthStencil();

		for(uint32 Index = 0; Index < CommandBuffer.FrameBuffersCount; Index++)
		{
			vkDestroyFramebuffer(Device->LogicalDevice, CommandBuffer.FrameBuffers[Index], nullptr);
		}
		framebuffer_render_info FrameInfo = {};
		FrameInfo.Device = Device->LogicalDevice;
		FrameInfo.Height = RenderProperties.Height;
		FrameInfo.Width = RenderProperties.Width;
		FrameInfo.View = DepthStencil.View;
		CommandBuffer.SetupFrameBuffer(FrameInfo, &SwapChain);

		CommandBuffer.FlushSetupCommandBuffer(Device->LogicalDevice, Queue);

		// Command buffers need to be recreated as they may store
		// references to the recreated frame buffer
		CommandBuffer.DestroyCommandBuffers(Device->LogicalDevice);
		CommandBuffer.CreateCommandBuffers(Device->LogicalDevice, SwapChain.ImageCount);

		commandbuffer_render_info CommandInfo = {};
		CommandInfo.Color = RenderProperties.DefaultClearColor;
		CommandInfo.Width = RenderProperties.Width;
		CommandInfo.Height = RenderProperties.Height;
		CommandInfo.Pipeline = Pipelines.Solid;
		CommandInfo.PipelineLayout = RenderProperties.PipelineLayout;

		CommandBuffer.BuildCommandBuffers(CommandInfo, KList<k_object>());
		CommandBuffer.BuildPresentCommandBuffers(&SwapChain);

		vkQueueWaitIdle(Queue);
		vkDeviceWaitIdle(Device->LogicalDevice);

		// camera.updateAspectRatio((float)width / (float)height);

		// Notify derived class
		WindowResized();
		ViewChanged();

		RenderProperties.Prepared = true;
	}

	// Default ctor
	// TODO(Julian): Maybe get rid of this
	VulkanRenderer(renderer_platform& Platform, bool32 VulkanEnableValidation, VkPhysicalDeviceFeatures VulkanEnabledFeatures)
	{
		// Window = (HWND)Platform.Window;
		// WindowInstance = (HINSTANCE)Platform.Instance;

		Device = new VulkanEncapsulatedDevice();
		Device->Platform = &Platform;

		// Check for validation command line flag
		/*
		for (int32 i = 0; i < __argc; i++)
		{
		if (__argv[i] == std::k_string("-validation"))
		{
		enableValidation = true;
		}
		if (__argv[i] == std::k_string("-vsync"))
		{
		enableVSync = true;
		}
		}
		*/

		BaseProperties.EnabledFeatures = VulkanEnabledFeatures;

		// Enable console if validation is active
		// Debug message callback will output to it
		if(VulkanEnableValidation)
		{
			// setupConsole("VulkanExample");
		}
	}

	// dtor
	/*
	~renderer()
	{
	// Clean up Vulkan resources
	SwapChain.Cleanup();
	if (DescriptorPool != VK_NULL_HANDLE)
	{
	vkDestroyDescriptorPool(Device, DescriptorPool, nullptr);
	}
	if (SetupCommandBuffer != VK_NULL_HANDLE)
	{
	vkFreeCommandBuffers(Device, CommandPool, 1, &SetupCommandBuffer);
	}
	DestroyCommandBuffers();
	vkDestroyRenderPass(Device, RenderPass, nullptr);
	for (uint32 Index = 0; Index < FrameBuffersCount; Index++)
	{
	vkDestroyFramebuffer(Device, FrameBuffers[Index], nullptr);
	}

	for (VkShaderModule* Module = ShaderModules; *Module; ++Module)
	{
	vkDestroyShaderModule(Device, *Module, nullptr);
	}
	vkDestroyImageView(Device, DepthStencil.View, nullptr);
	vkDestroyImage(Device, DepthStencil.Image, nullptr);
	vkFreeMemory(Device, DepthStencil.Memory, nullptr);

	vkDestroyPipelineCache(Device, PipelineCache, nullptr);

	vkDestroyCommandPool(Device, CommandPool, nullptr);

	vkDestroySemaphore(Device, Semaphores.PresentComplete, nullptr);
	vkDestroySemaphore(Device, Semaphores.RenderComplete, nullptr);

	delete EncapsulatedDevice;

	if (EnableValidation)
	{
	if(MessageCallback)
	{
	vkDestroyDebugReportCallbackEXT(Instance, MessageCallback, nullptr);
	}
	}

	vkDestroyInstance(Instance, nullptr);
	}
	*/

	// Get window title with example name, device, et.
	k_string GetWindowTitle()
	{
		return BaseProperties.ApplicationName;
	}

	// Setup the vulkan instance, enable required extensions and connect to the physical device (GPU)
	void InitializeVulkan(bool32 VulkanEnableValidation)
	{
		VkResult Error;

		// Vulkan instance
		Error = CreateInstance(VulkanEnableValidation);
		if(Error)
		{
			K_ERROR(Error);
		}

		// If requested, we enable the default validation layers for debugging
		if(VulkanEnableValidation)
		{
			// The report flags determine what type of messages for the layers will be displayed
			// For validating (debugging) an appplication the error and warning bits should suffice
			VkDebugReportFlagsEXT DebugReportFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT; // | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
																					// Additional flags include performance info, loader and layer debug messages, etc.
			Debug.SetupDebugging(Instance, DebugReportFlags, VK_NULL_HANDLE);
		}

		Device->SetupDevice(Instance, Queue);

		// Find a suitable depth format
		VkBool32 IsValidDepthFormat = Device->GetSupportedDepthFormat(&CommandBuffer.DepthFormat);
		Assert(IsValidDepthFormat);

		SwapChain.Connect(Instance, Device->PhysicalDevice, Device->LogicalDevice);

		// Create synchronization objects
		VkSemaphoreCreateInfo SemaphoreCreateInfo = {};
		SemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		SemaphoreCreateInfo.pNext = NULL;
		SemaphoreCreateInfo.flags = 0;

		// Create a semaphore used to synchronize image presentation
		// Ensures that the image is displayed before we start submitting new commands to the queu
		VK_CHECK_RESULT(vkCreateSemaphore(Device->LogicalDevice, &SemaphoreCreateInfo, nullptr, &Semaphores.PresentComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands have been sumbitted and executed
		VK_CHECK_RESULT(vkCreateSemaphore(Device->LogicalDevice, &SemaphoreCreateInfo, nullptr, &Semaphores.RenderComplete));
		// Create a semaphore used to synchronize command submission
		// Ensures that the image is not presented until all commands for the text overlay have been sumbitted and executed
		// Will be inserted after the render complete semaphore if the text overlay is enabled
		// VK_CHECK_RESULT(vkCreateSemaphore(Device, &SemaphoreCreateInfo, nullptr, &Semaphores.textOverlayComplete));

		// Set up submit info structure
		// Semaphores will stay the same during application lifetime
		// Command buffer submission info is set by each example
		SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.pNext = NULL;

		SubmitInfo.pWaitDstStageMask = &RenderProperties.SubmitPipelineStages;
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = &Semaphores.PresentComplete;
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = &Semaphores.RenderComplete;
	}

	void Draw()
	{
		PrepareFrame();

		// Command buffer to be sumitted to the queue
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = CommandBuffer.GetCurrentDrawBuffer();

		// Submit to queue
		VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &SubmitInfo, VK_NULL_HANDLE));

		SubmitFrame();
	}

	// Pure virtual render function (override in derived class)
	void Render()
	{
		if(!RenderProperties.Prepared)
		{
			return;
		}

		vkDeviceWaitIdle(Device->LogicalDevice);
		Draw();
		vkDeviceWaitIdle(Device->LogicalDevice);
		if(!RenderProperties.Paused)
		{
			UpdateUniformBuffers();
		}
	}

	// Called when view change occurs
	// Can be overriden in derived class to e.g. update uniform buffers
	// Containing view dependant matrices
	void ViewChanged()
	{
		UpdateUniformBuffers();
	}

	// Called when the window has been resized
	// Can be overriden in derived class to recreate or rebuild resources attached to the frame buffer / swapchain
	void WindowResized()
	{
		// Can be overriden in derived class
	}

	// Setup default depth and stencil views
	void SetupDepthStencil()
	{
		VkImageCreateInfo Image = {};
		Image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		Image.pNext = NULL;
		Image.imageType = VK_IMAGE_TYPE_2D;
		Image.format = CommandBuffer.DepthFormat;
		Image.extent = { RenderProperties.Width, RenderProperties.Height, 1 };
		Image.mipLevels = 1;
		Image.arrayLayers = 1;
		Image.samples = VK_SAMPLE_COUNT_1_BIT;
		Image.tiling = VK_IMAGE_TILING_OPTIMAL;
		Image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		Image.flags = 0;

		VkMemoryAllocateInfo MemoryAllocInfo = {};
		MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		MemoryAllocInfo.pNext = NULL;
		MemoryAllocInfo.allocationSize = 0;
		MemoryAllocInfo.memoryTypeIndex = 0;

		VkImageViewCreateInfo DepthStencilView = {};
		DepthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		DepthStencilView.pNext = NULL;
		DepthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		DepthStencilView.format = CommandBuffer.DepthFormat;
		DepthStencilView.flags = 0;
		DepthStencilView.subresourceRange = {};
		DepthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		DepthStencilView.subresourceRange.baseMipLevel = 0;
		DepthStencilView.subresourceRange.levelCount = 1;
		DepthStencilView.subresourceRange.baseArrayLayer = 0;
		DepthStencilView.subresourceRange.layerCount = 1;

		VkMemoryRequirements MemoryRequirements;

		VK_CHECK_RESULT(vkCreateImage(Device->LogicalDevice, &Image, nullptr, &DepthStencil.Image));
		vkGetImageMemoryRequirements(Device->LogicalDevice, DepthStencil.Image, &MemoryRequirements);
		MemoryAllocInfo.allocationSize = MemoryRequirements.size;
		MemoryAllocInfo.memoryTypeIndex = Device->GetMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_RESULT(vkAllocateMemory(Device->LogicalDevice, &MemoryAllocInfo, nullptr, &DepthStencil.Memory));

		VK_CHECK_RESULT(vkBindImageMemory(Device->LogicalDevice, DepthStencil.Image, DepthStencil.Memory, 0));

		image_layout_info LayoutInfo = {};
		LayoutInfo.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		LayoutInfo.CommandBuffer = CommandBuffer.SetupCommandBuffer;
		LayoutInfo.Image = DepthStencil.Image;
		LayoutInfo.NewImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		LayoutInfo.OldImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// LayoutInfo.SubresourceRange = nullptr;
		CommandBuffer.SetImageLayout(LayoutInfo, true);

		DepthStencilView.image = DepthStencil.Image;
		VK_CHECK_RESULT(vkCreateImageView(Device->LogicalDevice, &DepthStencilView, nullptr, &DepthStencil.View));
	}
	// Setup a default render pass
	// Can be overriden in derived class to setup a custom render pass (e.g. for MSAA)
	void SetupRenderPass()
	{
		VkAttachmentDescription Attachments[2] = {};

		// Color attachment
		Attachments[0].format = CommandBuffer.Colorformat;
		Attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		Attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		Attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// Depth attachment
		Attachments[1].format = CommandBuffer.DepthFormat;
		Attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		Attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		Attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		Attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		Attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		Attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		Attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference ColorReference = {};
		ColorReference.attachment = 0;
		ColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference DepthReference = {};
		DepthReference.attachment = 1;
		DepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription Subpass = {};
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.flags = 0;
		Subpass.inputAttachmentCount = 0;
		Subpass.pInputAttachments = NULL;
		Subpass.colorAttachmentCount = 1;
		Subpass.pColorAttachments = &ColorReference;
		Subpass.pResolveAttachments = NULL;
		Subpass.pDepthStencilAttachment = &DepthReference;
		Subpass.preserveAttachmentCount = 0;
		Subpass.pPreserveAttachments = NULL;

		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.pNext = NULL;
		RenderPassInfo.attachmentCount = 2;
		RenderPassInfo.pAttachments = Attachments;
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &Subpass;
		RenderPassInfo.dependencyCount = 0;
		RenderPassInfo.pDependencies = NULL;

		VK_CHECK_RESULT(vkCreateRenderPass(Device->LogicalDevice, &RenderPassInfo, nullptr, &CommandBuffer.RenderPass));
	}

	// Connect and prepare the swap chain
	void InitializeSwapchain()
	{
		SwapChain.InitializeSurface(*Device->Platform);
	}
	// Create swap chain images
	void CreateSwapChainImages()
	{
		SwapChain.Create(&RenderProperties.Width, &RenderProperties.Height, BaseProperties.EnableVSync);
	}

	// Prepare commonly used Vulkan functions
	void Prepare()
	{
		// TODO(Julian): Put these somewhere else

		InitializeSwapchain();

		// TODO(Julian): Sort these
		if(BaseProperties.EnableDebugMarkers)
		{
			Debug.SetupDebug(Device->LogicalDevice);
		}
		CommandBuffer.CreateCommandPool(&SwapChain, Device->LogicalDevice);
		CommandBuffer.CreateSetupCommandBuffer(Device->LogicalDevice);
		CreateSwapChainImages();
		CommandBuffer.CreateCommandBuffers(Device->LogicalDevice, SwapChain.ImageCount);
		CommandBuffer.BuildPresentCommandBuffers(&SwapChain);
		SetupDepthStencil();
		SetupRenderPass();
		RenderProperties.CreatePipelineCache(Device->LogicalDevice);

		framebuffer_render_info RenderInfo = {};
		RenderInfo.Device = Device->LogicalDevice;
		RenderInfo.Height = RenderProperties.Height;
		RenderInfo.Width = RenderProperties.Width;
		RenderInfo.View = DepthStencil.View;

		CommandBuffer.SetupFrameBuffer(RenderInfo, &SwapChain);
		CommandBuffer.FlushSetupCommandBuffer(Device->LogicalDevice, Queue);
		// Recreate setup command buffer for derived class
		CommandBuffer.CreateSetupCommandBuffer(Device->LogicalDevice);
		// Create a simple texture loader class

		// Do other preperations
		ExtraPrepare();

		RenderProperties.Prepared = true;
	}

	VkShaderModule LoadShader(k_string FileName, VkDevice VulkanDevice, VkShaderStageFlagBits ShaderStage)
	{
		k_string Result;

		Result = GetFileContents(FileName);

		VkShaderModule ShaderModule = 0;
		VkShaderModuleCreateInfo ModuleCreateInfo = {};
		ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ModuleCreateInfo.pNext = NULL;
		ModuleCreateInfo.codeSize = Result.Count();
		ModuleCreateInfo.pCode = (uint32 *)Result.Data();
		ModuleCreateInfo.flags = 0;

		VK_CHECK_RESULT(vkCreateShaderModule(VulkanDevice, &ModuleCreateInfo, NULL, &ShaderModule));

		// MemDealloc(Buffer);

		return ShaderModule;
	}

	// Load a SPIR-V shader
	VkPipelineShaderStageCreateInfo LoadShader(k_string FileName, VkShaderStageFlagBits ShaderStage)
	{
		VkPipelineShaderStageCreateInfo ShaderStageCreateInfo = {};
		ShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStageCreateInfo.stage = ShaderStage;

		ShaderStageCreateInfo.module = LoadShader(FileName, Device->LogicalDevice, ShaderStage);

		ShaderStageCreateInfo.pName = "main"; // todo : make param
		Assert(ShaderStageCreateInfo.module != NULL);

		RenderProperties.ShaderModules.PushBack(ShaderStageCreateInfo.module);
		return ShaderStageCreateInfo;
	}

	// Start the main render loop
	void RenderLoop()
	{
		BaseProperties.DestinationWidth = RenderProperties.Width;
		BaseProperties.DestinationHeight = RenderProperties.Height;
		// Flush device to make sure all resources can be freed
		vkDeviceWaitIdle(Device->LogicalDevice);
	}

	// Prepare a submit info structure containing
	// semaphores and submit buffer info for vkQueueSubmit
	VkSubmitInfo PrepareSubmitInfo(
		KList<VkCommandBuffer> CommandBuffers,
		KList<VkPipelineStageFlags> PipelineStages)
	{
		VkSubmitInfo CommandBufferSubmitInfo = {};
		CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		CommandBufferSubmitInfo.pNext = NULL;

		CommandBufferSubmitInfo.pWaitDstStageMask = PipelineStages.Data();
		CommandBufferSubmitInfo.waitSemaphoreCount = 1;
		CommandBufferSubmitInfo.pWaitSemaphores = &Semaphores.PresentComplete;
		CommandBufferSubmitInfo.commandBufferCount = CommandBuffers.Count();
		CommandBufferSubmitInfo.pCommandBuffers = CommandBuffers.Data();
		CommandBufferSubmitInfo.signalSemaphoreCount = 1;
		CommandBufferSubmitInfo.pSignalSemaphores = &Semaphores.RenderComplete;
		return CommandBufferSubmitInfo;
	}

	// Prepare the frame for workload submission
	// - Acquires the next image from the swap chain
	// - Submits a post present barrier
	// - Sets the default wait and signal semaphores
	void PrepareFrame()
	{
		// Acquire the next image from the swap chaing
		VK_CHECK_RESULT(SwapChain.AcquireNextImage(Semaphores.PresentComplete, &CommandBuffer.CurrentBuffer));

		// Submit post present image barrier to transform the image back to a color attachment that our render pass can write to
		VkSubmitInfo FrameSubmitInfo = {};
		FrameSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		FrameSubmitInfo.pNext = NULL;

		FrameSubmitInfo.commandBufferCount = 1;
		FrameSubmitInfo.pCommandBuffers = &CommandBuffer.PostPresentCommandBuffers[CommandBuffer.CurrentBuffer];
		VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &FrameSubmitInfo, VK_NULL_HANDLE));
	}

	// Submit the frames' workload
	// - Submits the text overlay (if enabled)
	// -
	void SubmitFrame()
	{
		// Submit pre present image barrier to transform the image from color attachment to present(khr) for presenting to the swap chain
		VkSubmitInfo FrameSubmitInfo = {};
		FrameSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		FrameSubmitInfo.pNext = NULL;

		FrameSubmitInfo.commandBufferCount = 1;
		FrameSubmitInfo.pCommandBuffers = &CommandBuffer.PrePresentCommandBuffers[CommandBuffer.CurrentBuffer];
		VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &FrameSubmitInfo, VK_NULL_HANDLE));

		// VK_CHECK_RESULT(swapChain.queuePresent(queue, currentBuffer, submitTextOverlay ? semaphores.textOverlayComplete : semaphores.renderComplete));
		VK_CHECK_RESULT(SwapChain.QueuePresent(Queue, CommandBuffer.CurrentBuffer, Semaphores.RenderComplete));

		VK_CHECK_RESULT(vkQueueWaitIdle(Queue));
	}


	// Extra for rendering
	void ExtraPrepare()
	{
		PrepareVertices();
		SetupDescriptorSetLayout();
		PreparePipelines();
		SetupDescriptorPool();
		SetupDescriptorSets();
		UpdateUniformBuffers();
		BuildCommandBuffers();
	}

	void PrepareVertices()
	{
		// Gear definitions
		/*
		k_list<Vector3> Colors = {
		Vector3(1.0f, 0.0f, 0.0f),
		Vector3(0.0f, 1.0f, 0.2f),
		Vector3(0.0f, 0.0f, 1.0f)
		};

		k_list<Vector3> Positions = {
			Vector3(-3.0f, 0.0f, 0.0f),
				Vector3(3.1f, 0.0f, 0.0f),
				Vector3(-3.1f, -6.2f, 0.0f)
		};
		*/

		KList<Vector3> Colors = {};

		KList<Vector3> Positions = {};


		uint32 GridX = 0;
		uint32 GridZ = 0;

		real32 Distance = 2.0f;

		for(uint32 Index = 0; Index < 1000; ++Index)
		{
			Positions.PushBack(Vector3((real32)GridX * Distance, KantiRandomManager::RandomRangeUniform(0.0f, 1.0f), (real32)GridZ * Distance));
			Colors.PushBack(Vector3(
				KantiRandomManager::RandomRangeUniform(0.0f, 1.0f),
				KantiRandomManager::RandomRangeUniform(0.0f, 1.0f),
				KantiRandomManager::RandomRangeUniform(0.0f, 1.0f)));

			GridX++;

			if(GridX > 25)
			{
				GridX = 0;
				GridZ++;
			}
		}


		Meshes.Resize(Positions.Count());
		for(uint32 Index = 0; Index < Meshes.Count(); ++Index)
		{
			Meshes[Index] = new KantiRenderObject();
			Meshes[Index]->Position = Positions[Index];
			Meshes[Index]->Color = Colors[Index];
			Meshes[Index]->Generate(Device, Queue);
		}

		// Binding and attribute descriptions are shared across all meshes
		Vertices.BindingDescriptions.Resize(1);
		Vertices.BindingDescriptions[0] = CreateVertexInputBindingDescription(0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX);

		// Attribute descriptions
		// Describes memory layout and shader positions
		Vertices.AttributeDescriptions.Resize(2);
		// Location 0 : Position
		Vertices.AttributeDescriptions[0] =
			CreateVertexInputAttributeDescription(
			0,
			0,
			VK_FORMAT_R32G32B32_SFLOAT,
			0);
		// TODO(Julian): Turn these into automatic functions
		// Location 1 : Normal
		Vertices.AttributeDescriptions[1] =
			CreateVertexInputAttributeDescription(
			0,
			1,
			VK_FORMAT_R32G32B32_SFLOAT,
			sizeof(real32) * 3);
		/*
		// Location 2 : Color
		Vertices.AttributeDescriptions[2] =
			CreateVertexInputAttributeDescription(
			0,
			2,
			VK_FORMAT_R32G32B32_SFLOAT,
			sizeof(real32) * 6);
			*/

		Vertices.InputState = CreatePipelineVertexInputStateInfo();
		Vertices.InputState.vertexBindingDescriptionCount = Vertices.BindingDescriptions.Count();
		Vertices.InputState.pVertexBindingDescriptions = Vertices.BindingDescriptions.Data();
		Vertices.InputState.vertexAttributeDescriptionCount = Vertices.AttributeDescriptions.Count();
		Vertices.InputState.pVertexAttributeDescriptions = Vertices.AttributeDescriptions.Data();
	}

	void SetupDescriptorSetLayout()
	{
		KList<VkDescriptorSetLayoutBinding> SetLayoutBindings =
		{
			// Binding 0 : Vertex shader uniform buffer
			CreateDescriptorSetLayoutBinding(
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT,
			0)
		};

		VkDescriptorSetLayoutCreateInfo DescriptorLayout =
			CreateDescriptorSetLayoutCreateInfo(
			SetLayoutBindings.Data(),
			SetLayoutBindings.Count());

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(Device->LogicalDevice, &DescriptorLayout, nullptr, &DescriptorSetLayout));

		VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo =
			CreatePipelineLayoutCreateInfo(
			&DescriptorSetLayout,
			1);

		VK_CHECK_RESULT(vkCreatePipelineLayout(Device->LogicalDevice, &PipelineLayoutCreateInfo, nullptr, &RenderProperties.PipelineLayout));
	}

	void PreparePipelines()
	{
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyState =
			CreatePipelineInputAssemblyStateCreateInfo(
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			0,
			VK_FALSE);

		VkPipelineRasterizationStateCreateInfo RasterizationState =
			CreatePipelineRasterizationStateCreateInfo(
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
			0);

		VkPipelineColorBlendAttachmentState BlendAttachmentState =
			CreatePipelineColorBlendAttachmentState(
			0xf,
			VK_FALSE);

		VkPipelineColorBlendStateCreateInfo ColorBlendState =
			CreatePipelineColorBlendStateCreateInfo(
			1,
			&BlendAttachmentState);

		VkPipelineDepthStencilStateCreateInfo DepthStencilState =
			CreatePipelineDepthStencilStateCreateInfo(
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS_OR_EQUAL);

		VkPipelineViewportStateCreateInfo ViewportState =
			CreatePipelineViewportStateCreateInfo(1, 1, 0);

		VkPipelineMultisampleStateCreateInfo MultisampleState =
			CreatePipelineMultisampleStateCreateInfo(
			VK_SAMPLE_COUNT_1_BIT,
			0);

		KList<VkDynamicState> DynamicStateEnables = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState =
			CreatePipelineDynamicStateCreateInfo(
			DynamicStateEnables.Data(),
			DynamicStateEnables.Count(),
			0);

		// Solid rendering pipeline
		// Load shaders
		KList<VkPipelineShaderStageCreateInfo> ShaderStages(2);

		k_string VertShader = "./../Data/triangle.vert.spv"; //GetAssetPath();
		// VertShader.PushBack("gears.vert.spv");

		k_string FragShader = "./../Data/triangle.frag.spv"; // GetAssetPath();
		// FragShader.PushBack("gears.frag.spv");
		ShaderStages[0] = LoadShader(VertShader , VK_SHADER_STAGE_VERTEX_BIT);
		ShaderStages[1] = LoadShader(FragShader, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkGraphicsPipelineCreateInfo PipelineCreateInfo =
			CreatePipelineCreateInfo(RenderProperties.PipelineLayout, CommandBuffer.RenderPass, 0);

		PipelineCreateInfo.pVertexInputState = &Vertices.InputState;
		PipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
		PipelineCreateInfo.pRasterizationState = &RasterizationState;
		PipelineCreateInfo.pColorBlendState = &ColorBlendState;
		PipelineCreateInfo.pMultisampleState = &MultisampleState;
		PipelineCreateInfo.pViewportState = &ViewportState;
		PipelineCreateInfo.pDepthStencilState = &DepthStencilState;
		PipelineCreateInfo.pDynamicState = &dynamicState;
		PipelineCreateInfo.stageCount = ShaderStages.Count();
		PipelineCreateInfo.pStages = ShaderStages.Data();

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(Device->LogicalDevice, RenderProperties.PipelineCache, 1, &PipelineCreateInfo, nullptr, &Pipelines.Solid));
	}

	void SetupDescriptorPool()
	{
		// One UBO for each gear
		KList<VkDescriptorPoolSize> PoolSizes =
		{
			CreateDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Meshes.Count()),
		};

		VkDescriptorPoolCreateInfo DescriptorPoolInfo =
			CreateDescriptorPoolCreateInfo(
			PoolSizes.Count(),
			PoolSizes.Data(),
			// Three descriptor sets (for each gear)
			Meshes.Count());

		VK_CHECK_RESULT(vkCreateDescriptorPool(Device->LogicalDevice, &DescriptorPoolInfo, nullptr, &CommandBuffer.DescriptorPool));
	}

	void SetupDescriptorSets()
	{
		for (uint32 Index = 0; Index < Meshes.Count(); ++Index)
		{
			Meshes[Index]->SetupDescriptorSet(Device, CommandBuffer.DescriptorPool, DescriptorSetLayout);
		}
	}

	void UpdateUniformBuffers()
	{
		for (uint32 BufferIndex = 0; BufferIndex < Meshes.Count(); ++BufferIndex)
		{
			Meshes[BufferIndex]->UpdateUniformBuffer(Device, KantiCameraManager::GetMainCamera(), KantiTimeManager::GetMSPerFrame());
		}
	}

	void BuildCommandBuffers()
	{
		VkCommandBufferBeginInfo CommandBufferBegin = CreateCommandBufferBeginInfo();

		VkClearValue ClearValues[2];
		ClearValues[0].color = RenderProperties.DefaultClearColor;
		ClearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo RenderPassBeginInfo = CreateRenderPassBeginInfo();
		RenderPassBeginInfo.renderPass = CommandBuffer.RenderPass;
		RenderPassBeginInfo.renderArea.offset.x = 0;
		RenderPassBeginInfo.renderArea.offset.y = 0;
		RenderPassBeginInfo.renderArea.extent.width = RenderProperties.Width;
		RenderPassBeginInfo.renderArea.extent.height = RenderProperties.Height;
		RenderPassBeginInfo.clearValueCount = 2;
		RenderPassBeginInfo.pClearValues = ClearValues;

		for (uint32 BufferIndex = 0; BufferIndex < CommandBuffer.DrawCommandBuffers.Count(); ++BufferIndex)
		{
			RenderPassBeginInfo.framebuffer = CommandBuffer.FrameBuffers[BufferIndex];

			VK_CHECK_RESULT(vkBeginCommandBuffer(CommandBuffer.DrawCommandBuffers[BufferIndex], &CommandBufferBegin));

			vkCmdBeginRenderPass(CommandBuffer.DrawCommandBuffers[BufferIndex], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport Viewport = CreateViewport((real32)RenderProperties.Width, (real32)RenderProperties.Height, 0.0f, 1.0f);
			vkCmdSetViewport(CommandBuffer.DrawCommandBuffers[BufferIndex], 0, 1, &Viewport);

			VkRect2D Scissor = CreateRect2D(RenderProperties.Width, RenderProperties.Height, 0, 0);
			vkCmdSetScissor(CommandBuffer.DrawCommandBuffers[BufferIndex], 0, 1, &Scissor);

			vkCmdBindPipeline(CommandBuffer.DrawCommandBuffers[BufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines.Solid);

			for (uint32 Index = 0; Index < Meshes.Count(); ++Index)
			{
				Meshes[Index]->Draw(CommandBuffer.DrawCommandBuffers[BufferIndex], RenderProperties.PipelineLayout);
			}

			vkCmdEndRenderPass(CommandBuffer.DrawCommandBuffers[BufferIndex]);

			VK_CHECK_RESULT(vkEndCommandBuffer(CommandBuffer.DrawCommandBuffers[BufferIndex]));
		}
	}

	static void *operator new(memory_index Size)
	{
		return MemAlloc(Size, 8);
	}

	static void operator delete(void *Block)
	{
		MemDealloc(Block);
	}
};