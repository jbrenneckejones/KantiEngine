#ifndef KANTI_VULKAN

#ifdef _WIN32
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#endif

#ifdef _DEBUG
k_internal int32 ValidationLayerCount = 1;
k_internal const char* ValidationLayerNames[] =
{
	// This is a meta layer that enables all of the standard
	// validation layers in the correct order :
	// threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
	"VK_LAYER_LUNARG_standard_validation"
};

VkDebugReportCallbackEXT VulkanDebugReportCallback;

k_internal VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessageCallback(VkDebugReportFlagsEXT ReportFlags,
	VkDebugReportObjectTypeEXT ReportObject, uint64 Source, memory_index Location,
	int32 ErrorCode, const char* LayerName, const char* Message, void* UserData)
{
	KString Prefix;

	if (ReportFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		Prefix = "ERROR";
	};
	if (ReportFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		Prefix = "WARNING";
	};

	KString Output = ("[Validation %s]: %s\n", Prefix, Message);
	DebugMessage(Output);

	return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance Instance, const VkDebugReportCallbackCreateInfoEXT* CreateInfo,
	const VkAllocationCallbacks* Allocator, VkDebugReportCallbackEXT* Callback) 
{
	auto Function = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
	if (Function != nullptr) 
	{
		return Function(Instance, CreateInfo, Allocator, Callback);
	}
	else 
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance Instance, VkDebugReportCallbackEXT Callback, const VkAllocationCallbacks* Allocator) 
{
	auto Function = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");
	if (Function != nullptr) 
	{
		Function(Instance, Callback, Allocator);
	}
}

#endif

class VulkanAllocator
{
public:

	k_internal VkAllocationCallbacks Callback;

	VulkanAllocator()
	{
		Callback = {};
		Callback.pUserData = (void*)this;
		Callback.pfnAllocation = &Allocation;
		Callback.pfnReallocation = &Reallocation;
		Callback.pfnFree = &Free;
		Callback.pfnInternalAllocation = nullptr;
		Callback.pfnInternalFree = nullptr;
	}

private:

	k_internal void* VKAPI_CALL Allocation(
		void* UserData,
		memory_index Size,
		memory_index Alignment,
		VkSystemAllocationScope AllocationScope)
	{
		return MemAlloc(Size, (uint8)Alignment);
	}

	k_internal void* VKAPI_CALL Reallocation(
		void* UserData,
		void* Original,
		memory_index Size,
		memory_index Alignment,
		VkSystemAllocationScope AllocationScope)
	{
		return MemRealloc(Original, Size, Alignment);
	}

	k_internal void VKAPI_CALL Free(
		void* UserData,
		void* Memory)
	{
		MemDealloc(Memory);
	}
};

VkAllocationCallbacks VulkanAllocator::Callback = {};

k_internal const char* VulkanDeviceExtensions[] =
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

k_internal const KVertex SquareVertices[] =
{
	{ { -0.5f, -0.5f,  0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.0f },  { 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
	{ {  0.5f,  0.5f,  0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f } },
	{ { -0.5f,  0.5f,  0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } },

	{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } },
	{ { 0.5f, -0.5f,  -0.5f },{ 1.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 0.0f } },
	{ { 0.5f, 0.5f,	  -0.5f },{ 1.0f, 1.0f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } },
	{ { -0.5f, 0.5f,  -0.5f },{ 0.0f, 1.0f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f, 0.0f } }
};

k_internal const uint16 SquareIndices[] =
{
	0, 1, 2, 2, 3, 0,

	4, 5, 6, 6, 7, 4
};

class VulkanRenderer
{
public:

	renderer_platform					Platform;
	VulkanAllocator						VulkanAlloc;
	bool32								VulkanEnableValidation = true;
	VkDebugReportCallbackEXT			VulkanReportCallback;

	VkInstance							VulkanInstance;
	VkPhysicalDevice					VulkanPhysicalDevice;
	int32								VulkanGraphicsFamily;
	VkDevice							VulkanDevice;

	int32								VulkanGraphicsIndex;
	VkQueue								VulkanGraphicsQueue;

	int32								VulkanPresentIndex;
	VkQueue								VulkanPresentQueue;
	VkSurfaceKHR						VulkanSurface;

	VkSwapchainKHR						VulkanSwapChain;
	KList<VkImage>						VulkanSwapChainImages;
	VkFormat							VulkanSwapChainFormat;
	VkExtent2D							VulkanSwapChainExtent;
	KList<VkImageView>					VulkanSwapChainImageViews;
	KList<VkFramebuffer>				VulkanSwapChainFrameBuffers;

	VkPipeline							VulkanGraphicsPipeline;
	VkPipelineLayout					VulkanPipelineLayout;
	VkRenderPass						VulkanRenderPass;

	VkCommandPool						VulkanCommandPool;
	KList<VkCommandBuffer>				VulkanCommandBuffers;

	VkSemaphore							VulkanImageAvailableSempahore;
	VkSemaphore							VulkanRenderFinishedSemaphore;

	VkBuffer							VulkanVertexBuffer;
	VkDeviceMemory						VulkanVertexBufferMemory;

	VkBuffer							VulkanIndexBuffer;
	VkDeviceMemory						VulkanIndexBufferMemory;

	VkDescriptorSetLayout				VulkanDescriptorSetLayout;

	VkBuffer							VulkanUniformStagingBuffer;
	VkDeviceMemory						VulkanUniformStagingBufferMemory;
	VkBuffer							VulkanUniformBuffer;
	VkDeviceMemory						VulkanUniformBufferMemory;

	VkDescriptorPool					VulkanDescriptorPool;
	VkDescriptorSet						VulkanDescriptorSet;

	VkImage								VulkanTextureImage;
	VkDeviceMemory						VulkanTextureImageMemory;
	VkImageView							VulkanTextureImageView;
	VkSampler							VulkanTextureSampler;

	VkImage								VulkanDepthImage;
	VkDeviceMemory						VulkanDepthImageMemory;
	VkImageView							VulkanDepthImageView;

	KList<KVertex> Vertices;
	KList<uint32> Indices;

public:

	VulkanRenderer(renderer_platform& APIPlatform)
	{
		Platform = APIPlatform;

		VulkanAlloc = VulkanAllocator();

#ifndef _DEBUG
		VulkanEnableValidation = false;
#endif

		Initialize();
	}

	void Initialize()
	{
		DebugMessage("Initializing Vulkan\n\n");
		V_CreateInstance();
		V_SetupDebugCallback();
		V_CreateSurface();
		V_CreateDevice();
		V_CreateQueue();
		V_CreateSwapChain();
		V_CreateImageViews();
		V_CreateRenderPass();
		V_CreateDescriptorSetLayout();
		V_CreateGraphicsPipeline();
		V_CreateCommandPool();
		V_CreateDepthResources();
		V_CreateFrameBuffers();
		V_CreateTextureImage();
		V_CreateTextureImageView();
		V_CreateTextureSampler();

		V_TempLoadModel();

		V_CreateVertexBuffer();
		V_CreateIndexBuffer();
		V_CreateUniformBuffer();
		V_CreateDescriptorPool();
		V_CreateDescriptorSet();
		V_CreateCommandBuffers();
		V_CreateSemaphores();
	}

	void Render()
	{
		// Temp
		V_TempUpdateUniformBuffers();

		V_DrawFrame();
	}

	// Helper

	VkResult H_CreateShaderModule(KString ShaderCode, VkShaderModule& Module)
	{
		VkResult Result = {};

		VkShaderModuleCreateInfo ShaderCreateInfo = {};
		ShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ShaderCreateInfo.codeSize = ShaderCode.Count();
		ShaderCreateInfo.pCode = (uint32_t *)ShaderCode.Data();

		Result = vkCreateShaderModule(VulkanDevice, &ShaderCreateInfo, &VulkanAlloc.Callback, &Module);

		return (Result);
	}

	k_internal VkVertexInputBindingDescription H_GetVertexBindingDescription()
	{
		VkVertexInputBindingDescription VertBindingDescription = {};
		VertBindingDescription.binding = 0;
		VertBindingDescription.stride = sizeof(KVertex);
		VertBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return VertBindingDescription;
	}

	k_internal KList<VkVertexInputAttributeDescription> H_GetVertexAttributeDescriptions() {
		KList<VkVertexInputAttributeDescription> AttributeDescriptions = {};
		AttributeDescriptions.Resize(4);

		AttributeDescriptions[0].binding = 0;
		AttributeDescriptions[0].location = 0;
		AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[0].offset = offsetof(KVertex, Position);

		AttributeDescriptions[1].binding = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescriptions[1].offset = offsetof(KVertex, UV);

		AttributeDescriptions[2].binding = 0;
		AttributeDescriptions[2].location = 2;
		AttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[2].offset = offsetof(KVertex, Color);

		AttributeDescriptions[3].binding = 0;
		AttributeDescriptions[3].location = 3;
		AttributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[3].offset = offsetof(KVertex, Normal);

		return AttributeDescriptions;
	}

	uint32 H_FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Properties)
	{
		VkPhysicalDeviceMemoryProperties MemProperties;
		vkGetPhysicalDeviceMemoryProperties(VulkanPhysicalDevice, &MemProperties);

		for (uint32 Index = 0; Index < MemProperties.memoryTypeCount; Index++)
		{
			if ((TypeFilter & (1 << Index)) && (MemProperties.memoryTypes[Index].propertyFlags & Properties) == Properties)
			{
				return Index;
			}
		}

		return 0;
	}

	VkResult H_CreateBuffer(VkDeviceSize BufferSize, VkBufferUsageFlags BufferUsage,
		VkMemoryPropertyFlags MemoryProperties, VkBuffer& Buffer, VkDeviceMemory& BufferMemory)
	{
		VkResult Result = {};

		VkBufferCreateInfo BufferInfo = {};
		BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferInfo.size = BufferSize;
		BufferInfo.usage = BufferUsage;
		BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		Result = vkCreateBuffer(VulkanDevice, &BufferInfo, &VulkanAlloc.Callback, &Buffer);

		VkMemoryRequirements MemRequirements;
		vkGetBufferMemoryRequirements(VulkanDevice, Buffer, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = H_FindMemoryType(MemRequirements.memoryTypeBits, MemoryProperties);

		Result = vkAllocateMemory(VulkanDevice, &AllocInfo, &VulkanAlloc.Callback, &BufferMemory);

		Result = vkBindBufferMemory(VulkanDevice, Buffer, BufferMemory, 0);

		return (Result);
	}

	VkResult H_CopyBuffer(VkBuffer SourceBuffer, VkBuffer DestinationBuffer, VkDeviceSize BufferSize)
	{
		VkResult Result = {};

		VkCommandBuffer CopyCommandBuffer = H_BeginSingleTimeCommands();

		VkBufferCopy CopyRegion = {};
		CopyRegion.size = BufferSize;
		vkCmdCopyBuffer(CopyCommandBuffer, SourceBuffer, DestinationBuffer, 1, &CopyRegion);

		Result = H_EndSingleTimeCommands(CopyCommandBuffer);

		return (Result);
	}

	VkResult H_CopyImage(VkImage SourceImage, VkImage DestinationImage, uint32 ImageWidth, uint32 ImageHeight) 
	{
		VkResult Result = {};

		VkCommandBuffer ImageCommandBuffer = H_BeginSingleTimeCommands();

		VkImageSubresourceLayers SubResource = {};
		SubResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		SubResource.baseArrayLayer = 0;
		SubResource.mipLevel = 0;
		SubResource.layerCount = 1;

		VkImageCopy Region = {};
		Region.srcSubresource = SubResource;
		Region.dstSubresource = SubResource;
		Region.srcOffset = { 0, 0, 0 };
		Region.dstOffset = { 0, 0, 0 };
		Region.extent.width = ImageWidth;
		Region.extent.height = ImageHeight;
		Region.extent.depth = 1;

		vkCmdCopyImage(
			ImageCommandBuffer,
			SourceImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			DestinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &Region
		);

		Result = H_EndSingleTimeCommands(ImageCommandBuffer);

		return (Result);
	}

	VkResult H_CreateImage(uint32 ImageWidth, uint32 ImageHeight, VkFormat ImageFormat, VkImageTiling ImageTiling,
		VkImageUsageFlags ImageUsage, VkMemoryPropertyFlags ImageProperties, VkImage& Image, VkDeviceMemory& ImageMemory)
	{
		VkResult Result = {};

		VkImageCreateInfo ImageInfo = {};
		ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageInfo.extent.width = ImageWidth;
		ImageInfo.extent.height = ImageHeight;
		ImageInfo.extent.depth = 1;
		ImageInfo.mipLevels = 1;
		ImageInfo.arrayLayers = 1;
		ImageInfo.format = ImageFormat;
		ImageInfo.tiling = ImageTiling;
		ImageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
		ImageInfo.usage = ImageUsage;
		ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		Result = vkCreateImage(VulkanDevice, &ImageInfo, &VulkanAlloc.Callback, &Image);

		VkMemoryRequirements MemRequirements;
		vkGetImageMemoryRequirements(VulkanDevice, Image, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = H_FindMemoryType(MemRequirements.memoryTypeBits, ImageProperties);

		Result = vkAllocateMemory(VulkanDevice, &AllocInfo, &VulkanAlloc.Callback, &ImageMemory);

		Result = vkBindImageMemory(VulkanDevice, Image, ImageMemory, 0);

		return (Result);
	}

	VkCommandBuffer H_BeginSingleTimeCommands() 
	{
		VkCommandBufferAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandPool = VulkanCommandPool;
		AllocInfo.commandBufferCount = 1;

		VkCommandBuffer CommandBuffer;
		vkAllocateCommandBuffers(VulkanDevice, &AllocInfo, &CommandBuffer);

		VkCommandBufferBeginInfo BeginInfo = {};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

		return CommandBuffer;
	}

	VkResult H_EndSingleTimeCommands(VkCommandBuffer CommandBuffer)
	{
		VkResult Result = {};

		Result = vkEndCommandBuffer(CommandBuffer);

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer;

		Result = vkQueueSubmit(VulkanGraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);
		Result = vkQueueWaitIdle(VulkanGraphicsQueue);

		vkFreeCommandBuffers(VulkanDevice, VulkanCommandPool, 1, &CommandBuffer);

		return (Result);
	}

	bool32 H_HasStencilComponent(VkFormat Format) 
	{
		return Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkResult H_TransitionImageLayout(VkImage Image, VkFormat ImageFormat, VkImageLayout ImageOldLayout, VkImageLayout ImageNewLayout) 
	{
		VkResult Result = {};

		VkCommandBuffer CommandBuffer = H_BeginSingleTimeCommands();

		VkImageMemoryBarrier ImageBarrier = {};
		ImageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		ImageBarrier.oldLayout = ImageOldLayout;
		ImageBarrier.newLayout = ImageNewLayout;
		ImageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		ImageBarrier.image = Image;
		ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ImageBarrier.subresourceRange.baseMipLevel = 0;
		ImageBarrier.subresourceRange.levelCount = 1;
		ImageBarrier.subresourceRange.baseArrayLayer = 0;
		ImageBarrier.subresourceRange.layerCount = 1;

		if (ImageNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
		{
			ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (H_HasStencilComponent(ImageFormat))
			{
				ImageBarrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else 
		{
			ImageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		if (ImageOldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
			ImageNewLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) 
		{
			ImageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		}
		else if (ImageOldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED &&
				 ImageNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
		{
			ImageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		else if (ImageOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && 
				 ImageNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
		{
			ImageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			ImageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		}
		else if (ImageOldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
				 ImageNewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			ImageBarrier.srcAccessMask = 0;
			ImageBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		else
		{
			return Result;
		}

		vkCmdPipelineBarrier(
			CommandBuffer,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &ImageBarrier
		);

		Result = H_EndSingleTimeCommands(CommandBuffer);

		return (Result);
	}

	VkResult H_CreateImageView(VkImage Image, VkFormat ImageFormat, VkImageAspectFlags AspectFlags, VkImageView& ImageView)
	{
		VkResult Result = {};

		VkImageViewCreateInfo ViewInfo = {};
		ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ViewInfo.image = Image;
		ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ViewInfo.format = ImageFormat;
		ViewInfo.subresourceRange.aspectMask = AspectFlags;
		ViewInfo.subresourceRange.baseMipLevel = 0;
		ViewInfo.subresourceRange.levelCount = 1;
		ViewInfo.subresourceRange.baseArrayLayer = 0;
		ViewInfo.subresourceRange.layerCount = 1;

		Result = vkCreateImageView(VulkanDevice, &ViewInfo, &VulkanAlloc.Callback, &ImageView);

		return (Result);
	}

	void H_LoadImageFile(KString FileName, KImageData& Image)
	{
		KantiFileManager::LoadImageFile(FileName, Image);
	}

	VkFormat H_FindSupportedFormat(const KList<VkFormat>& Candidates, VkImageTiling Tiling,
		VkFormatFeatureFlags Features) 
	{
		for (uint32 Index = 0; Index < Candidates.Count(); ++Index) 
		{
			VkFormatProperties FormatProperties;
			vkGetPhysicalDeviceFormatProperties(VulkanPhysicalDevice, Candidates[Index], &FormatProperties);

			if (Tiling == VK_IMAGE_TILING_LINEAR && (FormatProperties.linearTilingFeatures & Features) == Features) 
			{
				return Candidates[Index];
			}
			else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (FormatProperties.optimalTilingFeatures & Features) == Features)
			{
				return Candidates[Index];
			}
		}

		return VkFormat();
	}

	VkFormat H_FindDepthFormat() {
		return H_FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	KMeshData H_LoadModel(KString FileName)
	{
		KMeshData Result = {};
		KantiFileManager::LoadOBJFile(FileName, Result);

		return (Result);
	}

	// Subsystem

	void V_TempUpdateUniformBuffers()
	{
		KMatrixData Matrices = {};
		Matrices.Model = KMatrix4x4::Rotate(KMatrix4x4(), KantiTimeManager::GetTime() * ToRadians(90.0f), KVector3(0.0f, 0.0f, 1.0f));
		Matrices.View = KMatrix4x4::LookAt(KVector3(2.0f, 2.0f, 2.0f), KVector3(0.0f, 0.0f, 0.0f), KVector3(0.0f, 0.0f, -1.0f));
		Matrices.Projection = KMatrix4x4::Perspective(ToRadians(45.0f), VulkanSwapChainExtent.width / (float)VulkanSwapChainExtent.height, 0.1f, 10.0f);
		// Matrixs.Projection[1][1] *= -1;

		void* PointerData;
		vkMapMemory(VulkanDevice, VulkanUniformStagingBufferMemory, 0, sizeof(Matrices), 0, &PointerData);
		MemCopy(PointerData, &Matrices, sizeof(Matrices));
		vkUnmapMemory(VulkanDevice, VulkanUniformStagingBufferMemory);

		H_CopyBuffer(VulkanUniformStagingBuffer, VulkanUniformBuffer, sizeof(Matrices));
	}

	VkResult V_CreateInstance()
	{
		DebugMessage("Creating Vulkan Instance\n");

		VkResult Result;

		VkApplicationInfo ApplicationInfo = {};
		ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		ApplicationInfo.pNext = NULL;
		ApplicationInfo.pApplicationName = "KantiEngineApp";
		ApplicationInfo.pEngineName = "KantiEngine";
		ApplicationInfo.engineVersion = ENGINE_VERSION;
		ApplicationInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo InstanceInfo = {};
		InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		InstanceInfo.pNext = NULL;
		InstanceInfo.flags = NULL;
		InstanceInfo.pApplicationInfo = &ApplicationInfo;
		InstanceInfo.enabledLayerCount = NULL;
		InstanceInfo.ppEnabledLayerNames = NULL;
		// InstanceInfo.enabledExtensionCount = NULL;
		// InstanceInfo.ppEnabledExtensionNames = NULL;

		// Extensions

		KList<const char*> EnabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

		EnabledExtensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		if (EnabledExtensions.Count() > 0)
		{
			if (VulkanEnableValidation)
			{
				EnabledExtensions.PushBack(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			}

			InstanceInfo.enabledExtensionCount = (uint32)EnabledExtensions.Count();
			InstanceInfo.ppEnabledExtensionNames = EnabledExtensions.Data();
		}

		if (VulkanEnableValidation)
		{
#ifdef _DEBUG

			InstanceInfo.enabledLayerCount = ValidationLayerCount;
			InstanceInfo.ppEnabledLayerNames = ValidationLayerNames;
#endif
		}

		uint32 ExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);

		KList<VkExtensionProperties> Extensions(ExtensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, Extensions.Data());

		DebugMessage("Extensions Available: \n\n");
		for (uint32 Index = 0; Index < Extensions.Count(); ++Index)
		{
			DebugMessage(Extensions[Index].extensionName);
			DebugMessage("\n");
		}
		DebugMessage("\n");

		Result = vkCreateInstance(&InstanceInfo, &VulkanAlloc.Callback, &VulkanInstance);

		return (Result);
	}

	VkResult V_SetupDebugCallback()
	{
		VkResult Result = {};

#ifdef _DEBUG

		if (!VulkanEnableValidation) 
		{
			return (Result);
		}
			

		VkDebugReportCallbackCreateInfoEXT CreateInfo = {};
		CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		CreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		CreateInfo.pfnCallback = VulkanDebugMessageCallback;

		Result = CreateDebugReportCallbackEXT(VulkanInstance, &CreateInfo, &VulkanAlloc.Callback, &VulkanReportCallback);

#endif

		return (Result);
	}

	VkResult V_CreateDevice()
	{
		DebugMessage("Creating Vulkan GPU Device\n\n");

		VkResult Result = {};

		uint32 DeviceCount = 0;
		vkEnumeratePhysicalDevices(VulkanInstance, &DeviceCount, nullptr);

		if (DeviceCount == 0)
		{
			K_ERROR("Failed to find GPUs with Vulkan support!");
		}

		KList<VkPhysicalDevice> Devices(DeviceCount);
		vkEnumeratePhysicalDevices(VulkanInstance, &DeviceCount, Devices.Data());

		for (uint32 Index = 0; Index < Devices.Count(); ++Index)
		{
			VulkanPhysicalDevice = Devices[Index];
		}

		if (VulkanPhysicalDevice == VK_NULL_HANDLE)
		{
			K_ERROR("Failed to find a suitable GPU!");
		}

		uint32 QueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(VulkanPhysicalDevice, &QueueFamilyCount, nullptr);

		KList<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(VulkanPhysicalDevice, &QueueFamilyCount, QueueFamilies.Data());

		KList<VkDeviceQueueCreateInfo> QueueCreateInfos;

		// Setting graphics family
		VulkanGraphicsFamily = 0;

		float QueuePriority = 1.0f;
		for (uint32 Index = 0; Index < QueueFamilyCount; ++Index) 
		{
			VkDeviceQueueCreateInfo QueueCreateInfo = {};
			QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueCreateInfo.queueFamilyIndex = Index;
			QueueCreateInfo.queueCount = 1;
			QueueCreateInfo.pQueuePriorities = &QueuePriority;

			QueueCreateInfos.PushBack(QueueCreateInfo);
		}


		VkPhysicalDeviceFeatures DeviceFeatures = {};
		// DeviceFeatures.shaderClipDistance = VK_TRUE;
		// DeviceFeatures.shaderCullDistance = VK_TRUE;

		VkDeviceCreateInfo DeviceCreateInfo = {};
		DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.Data();
		DeviceCreateInfo.queueCreateInfoCount = (uint32)QueueCreateInfos.Count();

		DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;

		DeviceCreateInfo.ppEnabledExtensionNames = VulkanDeviceExtensions;
		DeviceCreateInfo.enabledExtensionCount = ArrayCount(VulkanDeviceExtensions);

		if (VulkanEnableValidation)
		{
#ifdef _DEBUG
			DeviceCreateInfo.enabledLayerCount = ValidationLayerCount;
			DeviceCreateInfo.ppEnabledLayerNames = ValidationLayerNames;
#endif
		}
		else
		{
			DeviceCreateInfo.enabledLayerCount = 0;
		}

		Result = vkCreateDevice(VulkanPhysicalDevice, &DeviceCreateInfo, &VulkanAlloc.Callback, &VulkanDevice);

		return (Result);
	}

	VkResult V_CreateSurface()
	{
		DebugMessage("Creating Vulkan Surface\n\n");

		VkResult Result = {};

		VkWin32SurfaceCreateInfoKHR Win32CreateInfo = {};
		Win32CreateInfo.flags = 0;
		Win32CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		Win32CreateInfo.hwnd = (HWND)Platform.Window;
		Win32CreateInfo.hinstance = (HINSTANCE)Platform.Instance;

		Result = vkCreateWin32SurfaceKHR(VulkanInstance, &Win32CreateInfo, &VulkanAlloc.Callback, &VulkanSurface);

		return (Result);
	}

	VkResult V_CreateQueue()
	{
		DebugMessage("Creating Vulkan Queue\n\n");

		VkResult Result = {};

		VulkanGraphicsIndex = -1, VulkanPresentIndex = -1;

		uint32 QueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(VulkanPhysicalDevice, &QueueFamilyCount, nullptr);

		KList<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(VulkanPhysicalDevice, &QueueFamilyCount, QueueFamilies.Data());

		for (uint32 Index = 0; Index < QueueFamilies.Count(); ++Index)
		{
			if (QueueFamilies[Index].queueCount > 0 &&
				QueueFamilies[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				VulkanGraphicsIndex = Index;
			}

			VkBool32 PresentSupport = false;
			Result = vkGetPhysicalDeviceSurfaceSupportKHR(VulkanPhysicalDevice, Index, VulkanSurface, &PresentSupport);

			if (QueueFamilies[Index].queueCount > 0 && PresentSupport)
			{
				VulkanPresentIndex = Index;
			}

			if (VulkanGraphicsIndex >= -1 && VulkanPresentIndex >= -1)
			{
				break;
			}
		}

		vkGetDeviceQueue(VulkanDevice, VulkanGraphicsIndex, 0, &VulkanGraphicsQueue);
		vkGetDeviceQueue(VulkanDevice, VulkanPresentIndex, 0, &VulkanPresentQueue);

		return (Result);
	}

	VkResult V_CreateSwapChain()
	{
		DebugMessage("Creating Vulkan Swapchain\n\n");

		VkResult Result = {};

		uint32 ExtensionCount;
		vkEnumerateDeviceExtensionProperties(VulkanPhysicalDevice, nullptr, &ExtensionCount, nullptr);

		KList<VkExtensionProperties> AvailableExtensions(ExtensionCount);
		vkEnumerateDeviceExtensionProperties(VulkanPhysicalDevice, nullptr, &ExtensionCount, AvailableExtensions.Data());

		// Getting surface capabilities
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VulkanPhysicalDevice, VulkanSurface, &SurfaceCapabilities);

		uint32 FormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanPhysicalDevice, VulkanSurface, &FormatCount, nullptr);

		KList<VkSurfaceFormatKHR> FoundFormats;
		KList<VkPresentModeKHR> FoundPresentModes;
		if (FormatCount != 0)
		{
			FoundFormats.Resize(FormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(VulkanPhysicalDevice, VulkanSurface, &FormatCount, FoundFormats.Data());
		}

		uint32 PresentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanPhysicalDevice, VulkanSurface, &PresentModeCount, nullptr);

		if (PresentModeCount != 0)
		{
			FoundPresentModes.Resize(PresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(VulkanPhysicalDevice, VulkanSurface, &PresentModeCount, FoundPresentModes.Data());
		}

		// Get surface formats
		VkSurfaceFormatKHR SurfaceFormat = {};
		{
			if (FoundFormats.Count() == 1 &&
				FoundFormats[0].format == VK_FORMAT_UNDEFINED)
			{
				SurfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			for (uint32 Index = 0; Index < FoundFormats.Count(); ++Index)
			{
				if (FoundFormats[Index].format == VK_FORMAT_B8G8R8A8_UNORM &&
					FoundFormats[Index].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					SurfaceFormat = FoundFormats[Index];
					break;
				}
			}

			if (SurfaceFormat.format == NULL)
			{
				SurfaceFormat = FoundFormats[0];
			}
		}

		// Get Present mode
		VkPresentModeKHR PresentMode = {};
		{
			for (uint32 Index = 0; Index < FoundPresentModes.Count(); ++Index)
			{
				if (FoundPresentModes[Index] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					PresentMode = FoundPresentModes[Index];
				}
			}

			if (PresentMode == NULL)
			{
				PresentMode = VK_PRESENT_MODE_FIFO_KHR;
			}
		}

		// Get extent
		VkExtent2D Extent;
		{
			if (SurfaceCapabilities.currentExtent.width != MAXUINT32)
			{
				Extent = SurfaceCapabilities.currentExtent;
			}
			else
			{
				VkExtent2D ActualExtent = { 800, 600 };

				ActualExtent.width = Max(SurfaceCapabilities.minImageExtent.width, Min(SurfaceCapabilities.maxImageExtent.width, ActualExtent.width));
				ActualExtent.height = Max(SurfaceCapabilities.minImageExtent.height, Min(SurfaceCapabilities.maxImageExtent.height, ActualExtent.height));

				Extent = ActualExtent;
			}
		}

		uint32 ImageCount = SurfaceCapabilities.minImageCount + 1;
		if (SurfaceCapabilities.maxImageCount > 0 &&
			ImageCount > SurfaceCapabilities.maxImageCount)
		{
			ImageCount = SurfaceCapabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR SwapChainCreateInfo = {};
		SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapChainCreateInfo.surface = VulkanSurface;

		SwapChainCreateInfo.minImageCount = ImageCount;
		SwapChainCreateInfo.imageFormat = SurfaceFormat.format;
		SwapChainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
		SwapChainCreateInfo.imageExtent = Extent;
		SwapChainCreateInfo.imageArrayLayers = 1;
		SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32 QueueFamilyIndices[] = { (uint32)VulkanGraphicsIndex, (uint32)VulkanPresentIndex };

		if (VulkanGraphicsIndex != VulkanPresentIndex)
		{
			SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			SwapChainCreateInfo.queueFamilyIndexCount = 2;
			SwapChainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
		}
		else
		{
			SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		SwapChainCreateInfo.preTransform = SurfaceCapabilities.currentTransform;
		SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		SwapChainCreateInfo.presentMode = PresentMode;
		SwapChainCreateInfo.clipped = VK_TRUE;

		VkSwapchainKHR OldSwapChain = VulkanSwapChain;
		SwapChainCreateInfo.oldSwapchain = OldSwapChain;

		VkSwapchainKHR NewSwapChain = {};
		Result = vkCreateSwapchainKHR(VulkanDevice, &SwapChainCreateInfo, &VulkanAlloc.Callback, &NewSwapChain);
		VulkanSwapChain = NewSwapChain;

		Result = vkGetSwapchainImagesKHR(VulkanDevice, VulkanSwapChain, &ImageCount, nullptr);
		VulkanSwapChainImages.Resize(ImageCount);
		Result = vkGetSwapchainImagesKHR(VulkanDevice, VulkanSwapChain, &ImageCount, VulkanSwapChainImages.Data());

		VulkanSwapChainFormat = SurfaceFormat.format;
		VulkanSwapChainExtent = Extent;

		return (Result);
	}

	VkResult V_RecreateSwapChain()
	{
		DebugMessage("Recreating Vulkan SwapChain\n\n");

		VkResult Result = {};

		Result = vkDeviceWaitIdle(VulkanDevice);

		Result = V_CreateSwapChain();
		Result = V_CreateImageViews();
		Result = V_CreateRenderPass();
		Result = V_CreateGraphicsPipeline();
		Result = V_CreateDepthResources();
		Result = V_CreateFrameBuffers();
		Result = V_CreateCommandBuffers();

		return (Result);
	}

	VkResult V_CreateImageViews()
	{
		DebugMessage("Creating Vulkan Image Views\n\n");

		VkResult Result = {};

		VulkanSwapChainImageViews.Resize(VulkanSwapChainImages.Count());

		for (uint32 Index = 0; Index < VulkanSwapChainImages.Count(); Index++)
		{
			Result = H_CreateImageView(VulkanSwapChainImages[Index],
				VulkanSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, VulkanSwapChainImageViews[Index]);
		}

		return (Result);
	}

	VkResult V_CreateRenderPass()
	{
		DebugMessage("Creating Vulkan Render Pass\n\n");

		VkResult Result = {};

		VkAttachmentDescription ColorAttachment = {};
		ColorAttachment.format = VulkanSwapChainFormat;
		ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription DepthAttachment = {};
		DepthAttachment.format = H_FindDepthFormat();
		DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference ColorAttachmentRef = {};
		ColorAttachmentRef.attachment = 0;
		ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference DepthAttachmentRef = {};
		DepthAttachmentRef.attachment = 1;
		DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription SubPass = {};
		SubPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubPass.colorAttachmentCount = 1;
		SubPass.pColorAttachments = &ColorAttachmentRef;
		SubPass.pDepthStencilAttachment = &DepthAttachmentRef;

		VkSubpassDependency SubPassDependency = {};
		SubPassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		SubPassDependency.dstSubpass = 0;
		SubPassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		SubPassDependency.srcAccessMask = 0;
		SubPassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		SubPassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription Attachments[] = { ColorAttachment, DepthAttachment };
		VkRenderPassCreateInfo RenderPassInfo = {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = ArrayCount(Attachments);
		RenderPassInfo.pAttachments = Attachments;
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &SubPass;
		RenderPassInfo.dependencyCount = 1;
		RenderPassInfo.pDependencies = &SubPassDependency;

		Result = vkCreateRenderPass(VulkanDevice, &RenderPassInfo, &VulkanAlloc.Callback, &VulkanRenderPass);

		return (Result);
	}

	VkResult V_CreateDescriptorSetLayout()
	{
		DebugMessage("Creating Vulkan Descriptor Set Layout\n\n");

		VkResult Result = {};

		VkDescriptorSetLayoutBinding UniformLayoutBinding = {};
		UniformLayoutBinding.binding = 0;
		UniformLayoutBinding.descriptorCount = 1;
		UniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		UniformLayoutBinding.pImmutableSamplers = nullptr;
		UniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding SamplerLayoutBinding = {};
		SamplerLayoutBinding.binding = 1;
		SamplerLayoutBinding.descriptorCount = 1;
		SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		SamplerLayoutBinding.pImmutableSamplers = nullptr;
		SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding Bindings[] = { UniformLayoutBinding, SamplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo LayoutInfo = {};
		LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutInfo.bindingCount = ArrayCount(Bindings);
		LayoutInfo.pBindings = Bindings;

		Result = vkCreateDescriptorSetLayout(VulkanDevice, &LayoutInfo, &VulkanAlloc.Callback, &VulkanDescriptorSetLayout);

		return (Result);
	}

	VkResult V_CreateGraphicsPipeline()
	{
		DebugMessage("Creating Vulkan Graphics Pipeline\n\n");

		VkResult Result = {};

		KString VertShaderCode = GetFileContents("D:/Github/KantiEngine/Data/Shaders/Shader.vert.spv");
		KString FragShaderCode = GetFileContents("D:/Github/KantiEngine/Data/Shaders/Shader.frag.spv");

		VkShaderModule VertShaderModule = {};
		VkShaderModule FragShaderModule = {};
		H_CreateShaderModule(VertShaderCode, VertShaderModule);
		H_CreateShaderModule(FragShaderCode, FragShaderModule);

		VkPipelineShaderStageCreateInfo VertShaderStageInfo = {};
		VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		VertShaderStageInfo.module = VertShaderModule;
		VertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo FragShaderStageInfo = {};
		FragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		FragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		FragShaderStageInfo.module = FragShaderModule;
		FragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo ShaderStages[] = { VertShaderStageInfo, FragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
		VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkVertexInputBindingDescription BindingDescription = H_GetVertexBindingDescription();
		KList<VkVertexInputAttributeDescription> AttributeDescriptions = H_GetVertexAttributeDescriptions();

		VertexInputInfo.vertexBindingDescriptionCount = 1;
		VertexInputInfo.vertexAttributeDescriptionCount = AttributeDescriptions.Count();
		VertexInputInfo.pVertexBindingDescriptions = &BindingDescription;
		VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.Data();

		VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
		InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport Viewport = {};
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = (real32)VulkanSwapChainExtent.width;
		Viewport.height = (real32)VulkanSwapChainExtent.height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;

		VkRect2D Scissor = {};
		Scissor.offset = { 0, 0 };
		Scissor.extent = VulkanSwapChainExtent;

		VkPipelineViewportStateCreateInfo ViewportState = {};
		ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.pViewports = &Viewport;
		ViewportState.scissorCount = 1;
		ViewportState.pScissors = &Scissor;

		VkPipelineRasterizationStateCreateInfo Rasterizer = {};
		Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		Rasterizer.depthClampEnable = VK_FALSE;
		Rasterizer.rasterizerDiscardEnable = VK_FALSE;
		Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		Rasterizer.lineWidth = 1.0f;
		Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		// Fixed this by changing back face to front face
		Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		Rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo Multisampling = {};
		Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		Multisampling.sampleShadingEnable = VK_FALSE;
		Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineDepthStencilStateCreateInfo DepthStencil = {};
		DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencil.depthTestEnable = VK_TRUE;
		DepthStencil.depthWriteEnable = VK_TRUE;
		DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		DepthStencil.depthBoundsTestEnable = VK_FALSE;
		DepthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
		ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo ColorBlending = {};
		ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlending.logicOpEnable = VK_FALSE;
		ColorBlending.logicOp = VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount = 1;
		ColorBlending.pAttachments = &ColorBlendAttachment;
		ColorBlending.blendConstants[0] = 0.0f;
		ColorBlending.blendConstants[1] = 0.0f;
		ColorBlending.blendConstants[2] = 0.0f;
		ColorBlending.blendConstants[3] = 0.0f;

		VkDescriptorSetLayout SetLayouts[] = { VulkanDescriptorSetLayout };
		VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = ArrayCount(SetLayouts);
		PipelineLayoutInfo.pSetLayouts = SetLayouts;
		PipelineLayoutInfo.pushConstantRangeCount = 0;

		Result = vkCreatePipelineLayout(VulkanDevice, &PipelineLayoutInfo, &VulkanAlloc.Callback, &VulkanPipelineLayout);

		VkGraphicsPipelineCreateInfo PipelineInfo = {};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.stageCount = 2;
		PipelineInfo.pStages = ShaderStages;
		PipelineInfo.pVertexInputState = &VertexInputInfo;
		PipelineInfo.pInputAssemblyState = &InputAssembly;
		PipelineInfo.pViewportState = &ViewportState;
		PipelineInfo.pRasterizationState = &Rasterizer;
		PipelineInfo.pMultisampleState = &Multisampling;
		PipelineInfo.pDepthStencilState = &DepthStencil;
		PipelineInfo.pColorBlendState = &ColorBlending;
		PipelineInfo.layout = VulkanPipelineLayout;
		PipelineInfo.renderPass = VulkanRenderPass;
		PipelineInfo.subpass = 0;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		Result = vkCreateGraphicsPipelines(VulkanDevice, VK_NULL_HANDLE, 1, &PipelineInfo, &VulkanAlloc.Callback, &VulkanGraphicsPipeline);

		return (Result);
	}

	VkResult V_CreateFrameBuffers()
	{
		DebugMessage("Creating Vulkan Frame Buffers\n\n");

		VkResult Result = {};

		VulkanSwapChainFrameBuffers.Resize(VulkanSwapChainImageViews.Count());

		for (memory_index Index = 0; Index < VulkanSwapChainImageViews.Count(); Index++)
		{
			VkImageView ViewAttachments[] =
			{
				VulkanSwapChainImageViews[(uint32)Index],
				VulkanDepthImageView
			};

			VkFramebufferCreateInfo FrameBufferInfo = {};
			FrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FrameBufferInfo.renderPass = VulkanRenderPass;
			FrameBufferInfo.attachmentCount = ArrayCount(ViewAttachments);
			FrameBufferInfo.pAttachments = ViewAttachments;
			FrameBufferInfo.width = VulkanSwapChainExtent.width;
			FrameBufferInfo.height = VulkanSwapChainExtent.height;
			FrameBufferInfo.layers = 1;

			Result = vkCreateFramebuffer(VulkanDevice, &FrameBufferInfo, &VulkanAlloc.Callback, &VulkanSwapChainFrameBuffers[(uint32)Index]);
		}

		return (Result);
	}

	VkResult V_CreateCommandPool()
	{
		DebugMessage("Creating Vulkan Command Pool\n\n");

		VkResult Result = {};

		VkCommandPoolCreateInfo PoolInfo = {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.queueFamilyIndex = VulkanGraphicsFamily;

		Result = vkCreateCommandPool(VulkanDevice, &PoolInfo, &VulkanAlloc.Callback, &VulkanCommandPool);

		return (Result);
	}

	VkResult V_CreateDepthResources()
	{
		VkResult Result = {};

		VkFormat DepthFormat = H_FindDepthFormat();

		Result = H_CreateImage(VulkanSwapChainExtent.width, VulkanSwapChainExtent.height,
			DepthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VulkanDepthImage, VulkanDepthImageMemory);

		Result = H_CreateImageView(VulkanDepthImage, DepthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VulkanDepthImageView);

		Result = H_TransitionImageLayout(VulkanDepthImage, DepthFormat,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		return (Result);
	}

	VkResult V_CreateTextureImage()
	{
		DebugMessage("Creating Vulkan Texture Image\n\n");

		VkResult Result = {};

		KImageData Image;
		H_LoadImageFile("D:/Github/KantiEngine/Data/Models/chalet.jpg", Image);

		VkDeviceSize ImageSize = (VkDeviceSize)Image.Size;

		VkImage StagingImage;
		VkDeviceMemory StagingImageMemory;
		Result = H_CreateImage(Image.Width, Image.Height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_LINEAR,
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingImage, StagingImageMemory);

		void* PointerData;
		Result = vkMapMemory(VulkanDevice, StagingImageMemory, 0, ImageSize, 0, &PointerData);
		MemCopy(PointerData, Image.Data, (memory_index)ImageSize);
		vkUnmapMemory(VulkanDevice, StagingImageMemory);

		Result = H_CreateImage(Image.Width, Image.Height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VulkanTextureImage, VulkanTextureImageMemory);

		Result = H_TransitionImageLayout(StagingImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		Result = H_TransitionImageLayout(VulkanTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_PREINITIALIZED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Result = H_CopyImage(StagingImage, VulkanTextureImage, Image.Width, Image.Height);

		Result = H_TransitionImageLayout(VulkanTextureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		return (Result);
	}

	VkResult V_CreateTextureImageView()
	{
		DebugMessage("Creating Vulkan Texture Image View\n\n");

		VkResult Result = {};

		Result = H_CreateImageView(VulkanTextureImage, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_ASPECT_COLOR_BIT, VulkanTextureImageView);

		return (Result);
	}

	VkResult V_CreateTextureSampler()
	{
		DebugMessage("Creating Vulkan Texture Sampler\n\n");

		VkResult Result = {};

		VkSamplerCreateInfo SamplerInfo = {};
		SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		SamplerInfo.magFilter = VK_FILTER_LINEAR;
		SamplerInfo.minFilter = VK_FILTER_LINEAR;
		SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.anisotropyEnable = VK_TRUE;
		SamplerInfo.maxAnisotropy = 16;
		SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		SamplerInfo.unnormalizedCoordinates = VK_FALSE;
		SamplerInfo.compareEnable = VK_FALSE;
		SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		Result = vkCreateSampler(VulkanDevice, &SamplerInfo, &VulkanAlloc.Callback, &VulkanTextureSampler);

		return (Result);
	}

	VkResult V_TempLoadModel()
	{

		KMeshData Mesh = H_LoadModel("D:/Github/KantiEngine/Data/Models/Island.obj");

		Vertices = Mesh.Vertices;
		Indices = Mesh.Indices;

		return (VK_SUCCESS);
	}

	VkResult V_CreateVertexBuffer()
	{
		DebugMessage("Creating Vulkan Vertex Buffer\n\n");

		VkResult Result = {};

		VkDeviceSize BufferSize = sizeof(Vertices[0]) * Vertices.Count();

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* PointerData;
		Result = vkMapMemory(VulkanDevice, StagingBufferMemory, 0, BufferSize, 0, &PointerData);
		MemCopy(PointerData, (void *)Vertices.Data(), (memory_index)BufferSize);
		vkUnmapMemory(VulkanDevice, StagingBufferMemory);

		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VulkanVertexBuffer, VulkanVertexBufferMemory);

		Result = H_CopyBuffer(StagingBuffer, VulkanVertexBuffer, BufferSize);

		return (Result);
	}

	VkResult V_CreateIndexBuffer()
	{
		DebugMessage("Creating Vulkan Index Buffer\n\n");

		VkResult Result = {};

		VkDeviceSize BufferSize = sizeof(Indices[0]) * Indices.Count();

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* PointerData;
		Result = vkMapMemory(VulkanDevice, StagingBufferMemory, 0, BufferSize, 0, &PointerData);
		MemCopy(PointerData, (void *)Indices.Data(), (memory_index)BufferSize);
		vkUnmapMemory(VulkanDevice, StagingBufferMemory);

		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VulkanIndexBuffer, VulkanIndexBufferMemory);

		Result = H_CopyBuffer(StagingBuffer, VulkanIndexBuffer, BufferSize);

		return (Result);
	}

	VkResult V_CreateUniformBuffer()
	{
		DebugMessage("Creating Vulkan Uniform Buffer\n\n");

		VkResult Result = {};

		VkDeviceSize BufferSize = sizeof(KMatrixData);

		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VulkanUniformStagingBuffer, VulkanUniformStagingBufferMemory);

		Result = H_CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VulkanUniformBuffer, VulkanUniformBufferMemory);

		return (Result);
	}

	VkResult V_CreateDescriptorPool()
	{
		DebugMessage("Creating Vulkan Descriptor Pool\n\n");

		VkResult Result = {};

		VkDescriptorPoolSize PoolSizes[] = { {}, {} };
		PoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		PoolSizes[0].descriptorCount = 1;

		PoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		PoolSizes[1].descriptorCount = 1;

		VkDescriptorPoolCreateInfo PoolInfo = {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		PoolInfo.poolSizeCount = ArrayCount(PoolSizes);
		PoolInfo.pPoolSizes = PoolSizes;
		PoolInfo.maxSets = 1;

		Result = vkCreateDescriptorPool(VulkanDevice, &PoolInfo, &VulkanAlloc.Callback, &VulkanDescriptorPool);

		return (Result);
	}

	VkResult V_CreateDescriptorSet()
	{
		DebugMessage("Creating Vulkan Descriptor Set\n\n");

		VkResult Result = {};

		VkDescriptorSetLayout Layouts[] = { VulkanDescriptorSetLayout };
		VkDescriptorSetAllocateInfo AllocInfo = {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		AllocInfo.descriptorPool = VulkanDescriptorPool;
		AllocInfo.descriptorSetCount = 1;
		AllocInfo.pSetLayouts = Layouts;

		Result = vkAllocateDescriptorSets(VulkanDevice, &AllocInfo, &VulkanDescriptorSet);

		VkDescriptorBufferInfo BufferInfo = {};
		BufferInfo.buffer = VulkanUniformBuffer;
		BufferInfo.offset = 0;
		BufferInfo.range = sizeof(KMatrixData);

		VkDescriptorImageInfo ImageInfo = {};
		ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		ImageInfo.imageView = VulkanTextureImageView;
		ImageInfo.sampler = VulkanTextureSampler;

		VkWriteDescriptorSet DescriptorWrite[] = { {}, {} };
		DescriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite[0].dstSet = VulkanDescriptorSet;
		DescriptorWrite[0].dstBinding = 0;
		DescriptorWrite[0].dstArrayElement = 0;
		DescriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		DescriptorWrite[0].descriptorCount = 1;
		DescriptorWrite[0].pBufferInfo = &BufferInfo;

		DescriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrite[1].dstSet = VulkanDescriptorSet;
		DescriptorWrite[1].dstBinding = 1;
		DescriptorWrite[1].dstArrayElement = 0;
		DescriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		DescriptorWrite[1].descriptorCount = 1;
		DescriptorWrite[1].pImageInfo = &ImageInfo;

		vkUpdateDescriptorSets(VulkanDevice, ArrayCount(DescriptorWrite), DescriptorWrite, 0, nullptr);

		return (Result);
	}

	VkResult V_CreateCommandBuffers()
	{
		DebugMessage("Creating Vulkan Command Buffers\n\n");

		VkResult Result = {};

		if (VulkanCommandBuffers.Count() > 0)
		{
			vkFreeCommandBuffers(VulkanDevice, VulkanCommandPool, VulkanCommandBuffers.Count(), VulkanCommandBuffers.Data());
		}
		VulkanCommandBuffers.Resize(VulkanSwapChainFrameBuffers.Count());

		VkCommandBufferAllocateInfo BufferAllocInfo = {};
		BufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		BufferAllocInfo.commandPool = VulkanCommandPool;
		BufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		BufferAllocInfo.commandBufferCount = VulkanCommandBuffers.Count();

		Result = vkAllocateCommandBuffers(VulkanDevice, &BufferAllocInfo, VulkanCommandBuffers.Data());

		for (uint32 Index = 0; Index < VulkanCommandBuffers.Count(); ++Index)
		{
			VkCommandBufferBeginInfo BeginInfo = {};
			BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			vkBeginCommandBuffer(VulkanCommandBuffers[Index], &BeginInfo);

			VkRenderPassBeginInfo RenderPassInfo = {};
			RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			RenderPassInfo.renderPass = VulkanRenderPass;
			RenderPassInfo.framebuffer = VulkanSwapChainFrameBuffers[Index];
			RenderPassInfo.renderArea.offset = { 0, 0 };
			RenderPassInfo.renderArea.extent = VulkanSwapChainExtent;

			VkClearValue ClearValues[] = { {}, {} };
			ClearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			ClearValues[1].depthStencil = { 1.0f, 0 };
			RenderPassInfo.clearValueCount = ArrayCount(ClearValues);
			RenderPassInfo.pClearValues = ClearValues;

			vkCmdBeginRenderPass(VulkanCommandBuffers[Index], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(VulkanCommandBuffers[Index], VK_PIPELINE_BIND_POINT_GRAPHICS, VulkanGraphicsPipeline);

			VkBuffer VertexBuffers[] = { VulkanVertexBuffer };
			VkDeviceSize Offsets[] = { 0 };
			vkCmdBindVertexBuffers(VulkanCommandBuffers[Index], 0, 1, VertexBuffers, Offsets);

			vkCmdBindIndexBuffer(VulkanCommandBuffers[Index], VulkanIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(VulkanCommandBuffers[Index], VK_PIPELINE_BIND_POINT_GRAPHICS,
				VulkanPipelineLayout, 0, 1, &VulkanDescriptorSet, 0, nullptr);

			vkCmdDrawIndexed(VulkanCommandBuffers[Index], Indices.Count(), 1, 0, 0, 0);

			vkCmdEndRenderPass(VulkanCommandBuffers[Index]);

			Result = vkEndCommandBuffer(VulkanCommandBuffers[Index]);
		}

		return (Result);
	}

	VkResult V_CreateSemaphores()
	{
		DebugMessage("Creating Vulkan Semaphores\n\n");

		VkResult Result = {};

		VkSemaphoreCreateInfo SemaphoreInfo = {};
		SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		Result = vkCreateSemaphore(VulkanDevice, &SemaphoreInfo, &VulkanAlloc.Callback, &VulkanImageAvailableSempahore);
		Result = vkCreateSemaphore(VulkanDevice, &SemaphoreInfo, &VulkanAlloc.Callback, &VulkanRenderFinishedSemaphore);

		return (Result);
	}

	VkResult V_DrawFrame()
	{
		VkResult Result = {};

		uint32 ImageIndex;
		Result = vkAcquireNextImageKHR(VulkanDevice, VulkanSwapChain, MAXUINT64, VulkanImageAvailableSempahore, VK_NULL_HANDLE, &ImageIndex);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			V_RecreateSwapChain();
			return (Result);
		}

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore WaitSemaphores[] = { VulkanImageAvailableSempahore };
		VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = WaitSemaphores;
		SubmitInfo.pWaitDstStageMask = WaitStages;

		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &VulkanCommandBuffers[ImageIndex];

		VkSemaphore SignalSemaphores[] = { VulkanRenderFinishedSemaphore };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = SignalSemaphores;

		Result = vkQueueSubmit(VulkanGraphicsQueue, 1, &SubmitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR PresentInfo = {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = SignalSemaphores;

		VkSwapchainKHR SwapChains[] = { VulkanSwapChain };
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = SwapChains;

		PresentInfo.pImageIndices = &ImageIndex;

		Result = vkQueuePresentKHR(VulkanPresentQueue, &PresentInfo);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		{
			V_RecreateSwapChain();
		}

		return (Result);
	}

	void V_Destroy()
	{
		vkDestroyInstance(VulkanInstance, NULL);
	}
};

global_variable VulkanRenderer* Vulkan;

#define KANTI_VULKAN
#endif
