#include "VulkanEncapsulatedDevice.h"

VulkanEncapsulatedDevice::VulkanEncapsulatedDevice()
{
}

VulkanEncapsulatedDevice::VulkanEncapsulatedDevice(VkPhysicalDevice NewDevice)
{
	InitializeDevice(NewDevice);
}

VulkanEncapsulatedDevice::~VulkanEncapsulatedDevice()
{
	if (CommandPool)
	{
		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
	}
	if (LogicalDevice)
	{
		vkDestroyDevice(LogicalDevice, nullptr);
	}
}

void VulkanEncapsulatedDevice::InitializeDevice(VkPhysicalDevice NewDevice)
{
	Assert(NewDevice);
	this->PhysicalDevice = NewDevice;

	// Store Properties features, limits and properties of the physical device for later use
	// Device properties also contain limits and sparse properties
	vkGetPhysicalDeviceProperties(NewDevice, &DeviceProperties);
	// Features should be checked by the examples before using them
	vkGetPhysicalDeviceFeatures(NewDevice, &DeviceFeatures);
	// Memory properties are used regularly for creating all kinds of buffer
	vkGetPhysicalDeviceMemoryProperties(NewDevice, &DeviceMemoryProperties);
}

VkBool32 VulkanEncapsulatedDevice::CheckDeviceExtensionPresent(VkPhysicalDevice PhysicalDeviceToCheck, KString ExtensionName)
{
	uint32 ExtensionCount = 0;
	KList<VkExtensionProperties> Extensions;
	vkEnumerateDeviceExtensionProperties(PhysicalDeviceToCheck, NULL, &ExtensionCount, NULL);

	Extensions.Resize(ExtensionCount);
	vkEnumerateDeviceExtensionProperties(PhysicalDeviceToCheck, NULL, &ExtensionCount, Extensions.Data());
	for (uint32 Index = 0; Index < ExtensionCount; ++Index)
	{
		KString ExtName = Extensions[Index].extensionName;

		if (ExtensionName == ExtName)
		{
			return true;
		}
	}
	return false;
}

uint32 VulkanEncapsulatedDevice::GetMemoryType(uint32 TypeBits, VkMemoryPropertyFlags MemoryProperties, VkBool32* MemTypeFound)
{
	for (uint32 Index = 0; Index < DeviceMemoryProperties.memoryTypeCount; Index++)
	{
		if ((TypeBits & 1) == 1)
		{
			if ((DeviceMemoryProperties.memoryTypes[Index].propertyFlags & MemoryProperties) == MemoryProperties)
			{
				if (MemTypeFound)
				{
					*MemTypeFound = true;
				}
				return Index;
			}
		}
		TypeBits >>= 1;
	}

	if (MemTypeFound)
	{
		*MemTypeFound = false;
		return 0;
	}
	else
	{
		// throw std::runtime_error("Could not find a matching memory type");
		return 0;
	}
}

uint32 VulkanEncapsulatedDevice::GetQueueFamiliyIndex(VkQueueFlagBits QueueFlags)
{
	uint32 QueueCount;

	// Get number of available queue families on this device
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, NULL);
	Assert(QueueCount >= 1);

	// Get available queue families
	KList<VkQueueFamilyProperties> QueueProperties(QueueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, QueueProperties.Data());

	for (uint32 Index = 0; Index < QueueCount; Index++)
	{
		if (QueueProperties[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			return Index;
			break;
		}
	}

	// throw std::runtime_error("Could not find a matching queue family index");
	return 0;
}

VkResult VulkanEncapsulatedDevice::CreateLogicalDevice(bool32 UseSwapChain)
{
	// Get queue family indices for graphics and compute
	// Note that the indices may overlap depending on the implementation
	QueueFamilyIndices.Graphics = GetQueueFamiliyIndex(VK_QUEUE_GRAPHICS_BIT);
	QueueFamilyIndices.Compute = GetQueueFamiliyIndex(VK_QUEUE_COMPUTE_BIT);
	//todo: Transfer?

	// Pass queue information for graphics and compute, so examples can later on request queues from both
	KList<real32> QueuePriorities;

	KList<VkDeviceQueueCreateInfo> QueueCreateInfos;
	// We need one queue create info per queue family index
	// If graphics and compute share the same queue family index we only need one queue create info but
	// with two queues to request

	// Graphics
	QueuePriorities.PushBack(0.0f);
	VkDeviceQueueCreateInfo GraphicsInfo = {};
	GraphicsInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	GraphicsInfo.queueFamilyIndex = QueueFamilyIndices.Graphics;
	GraphicsInfo.queueCount = 1;
	QueueCreateInfos.PushBack(GraphicsInfo);
	// Compute
	// If compute has a different queue family index, add another create info, else just add
	if (QueueFamilyIndices.Graphics != QueueFamilyIndices.Compute)
	{
		VkDeviceQueueCreateInfo ComputeInfo = {};
		ComputeInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		ComputeInfo.queueFamilyIndex = QueueFamilyIndices.Compute;
		ComputeInfo.queueCount = 1;
		ComputeInfo.pQueuePriorities = QueuePriorities.Data();
		QueueCreateInfos.PushBack(ComputeInfo);
	}
	else
	{
		QueueCreateInfos[0].queueCount++;
		QueuePriorities.PushBack(0.0f);
	}
	QueueCreateInfos[0].pQueuePriorities = QueuePriorities.Data();

	// Create the logical device representation
	KList<const char*> DeviceExtensions;
	if (UseSwapChain)
	{
		// If the device will be used for presenting to a display via a swapchain
		// we need to request the swapchain extension
		DeviceExtensions.PushBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.queueCreateInfoCount = QueueCreateInfos.Count();
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.Data();
	DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;

	// Cnable the debug marker extension if it is present (likely meaning a debugging tool is present)
	if (CheckDeviceExtensionPresent(PhysicalDevice, VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
	{
		DeviceExtensions.PushBack(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		EnableDebugMarkers = true;
	}

	if (DeviceExtensions.Count() > 0)
	{
		DeviceCreateInfo.enabledExtensionCount = DeviceExtensions.Count();
		// TODO(Julian): Make sure this works since it's a two layer list i.e. string list plus a list of strings
		DeviceCreateInfo.ppEnabledExtensionNames = (char const* const*)DeviceExtensions.Data();
	}

	VkResult Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &LogicalDevice);

	if (Result == VK_SUCCESS)
	{
		// Create a default command pool for graphics command buffers
		CommandPool = CreateCommandPool(QueueFamilyIndices.Graphics);
	}

	return Result;
}

VkResult VulkanEncapsulatedDevice::CreateBuffer(device_creation_info* Info)
{
	// Create the buffer handle
	VkBufferCreateInfo BufferCreateInfo = CreateBufferCreateInfo(Info->UsageFlags, Info->DeviceSize);
	BufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK_RESULT(vkCreateBuffer(LogicalDevice, &BufferCreateInfo, nullptr, Info->DeviceBuffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements MemoryRequirements;
	VkMemoryAllocateInfo MemoryAllocInfo = CreateMemoryAllocInfo();

	vkGetBufferMemoryRequirements(LogicalDevice, *Info->DeviceBuffer, &MemoryRequirements);
	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	// Find a memory type index that fits the properties of the buffer
	MemoryAllocInfo.memoryTypeIndex = GetMemoryType(MemoryRequirements.memoryTypeBits, Info->MemoryPropertyFlags);
	VK_CHECK_RESULT(vkAllocateMemory(LogicalDevice, &MemoryAllocInfo, nullptr, Info->DeviceMemory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (Info->BufferData != nullptr)
	{
		void* MappedData = 0;
		VK_CHECK_RESULT(vkMapMemory(LogicalDevice, *Info->DeviceMemory, 0, Info->DeviceSize, 0, &MappedData));
		MemCopy(MappedData, Info->BufferData, Info->DeviceSize);
		vkUnmapMemory(LogicalDevice, *Info->DeviceMemory);
	}

	// Attach the memory to the buffer object
	VK_CHECK_RESULT(vkBindBufferMemory(LogicalDevice, *Info->DeviceBuffer, *Info->DeviceMemory, 0));

	return VK_SUCCESS;
}

VkResult VulkanEncapsulatedDevice::CreateBuffer(device_creation_info* Info, VulkanBuffer* Buffer)
{
	Buffer->Device = LogicalDevice;

	// Create the buffer handle
	VkBufferCreateInfo BufferCreateInfo = CreateBufferCreateInfo(Info->UsageFlags, Info->DeviceSize);

	VK_CHECK_RESULT(vkCreateBuffer(LogicalDevice, &BufferCreateInfo, nullptr, &Buffer->Buffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements MemoryRequirements;
	VkMemoryAllocateInfo MemoryAllocInfo = CreateMemoryAllocInfo();

	vkGetBufferMemoryRequirements(LogicalDevice, Buffer->Buffer, &MemoryRequirements);
	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	// Find a memory type index that fits the properties of the buffer
	MemoryAllocInfo.memoryTypeIndex = GetMemoryType(MemoryRequirements.memoryTypeBits, Info->MemoryPropertyFlags);
	VK_CHECK_RESULT(vkAllocateMemory(LogicalDevice, &MemoryAllocInfo, nullptr, &Buffer->DeviceMemory));

	Buffer->BufferAlignment = MemoryRequirements.alignment;
	Buffer->BufferSize = MemoryAllocInfo.allocationSize;
	Buffer->BufferUsageFlags = Info->UsageFlags;
	Buffer->MemoryPropertyFlags = Info->MemoryPropertyFlags;

	// If a pointer to the buffer data has been passed, map the buffer and copy over the data
	if (Info->BufferData != nullptr)
	{
		VK_CHECK_RESULT(Buffer->MapBuffer());
		MemCopy(Buffer->BufferMappedData, Info->BufferData, Info->DeviceSize);
		Buffer->UnmapBuffer();
	}

	// Initialize a default descriptor that covers the whole buffer size
	Buffer->SetupBufferInfo();

	// Attach the memory to the buffer object
	return Buffer->BindBuffer();
}

void VulkanEncapsulatedDevice::CopyBuffer(VulkanBuffer* Source, VulkanBuffer* Destination, VkQueue Queue, VkBufferCopy* CopyRegion)
{
	Assert(Destination->BufferSize <= Source->BufferSize);
	Assert(Source->Buffer && Source->Buffer);
	VkCommandBuffer CopyBuffer = CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	VkBufferCopy BufferCopy =  { };
	if (CopyRegion == nullptr)
	{
		BufferCopy.size = Source->BufferSize;
	}
	else
	{
		BufferCopy = *CopyRegion;
	}

	vkCmdCopyBuffer(CopyBuffer, Source->Buffer, Destination->Buffer, 1, &BufferCopy);

	FlushCommandBuffer(CopyBuffer, Queue);
}

VkCommandPool VulkanEncapsulatedDevice::CreateCommandPool(uint32 QueueFamilyIndex, VkCommandPoolCreateFlags CommandCreateFlags)
{
	VkCommandPoolCreateInfo CommandPoolInfo = {};
	CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolInfo.queueFamilyIndex = QueueFamilyIndex;
	CommandPoolInfo.flags = CommandCreateFlags;
	VkCommandPool CreatedCommandPool = 0;
	VK_CHECK_RESULT(vkCreateCommandPool(LogicalDevice, &CommandPoolInfo, nullptr, &CreatedCommandPool));
	return CreatedCommandPool;
}

VkCommandBuffer VulkanEncapsulatedDevice::CreateCommandBuffer(VkCommandBufferLevel BufferLevel, bool32 BeginBuffer)
{
	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { };
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = BufferLevel;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer CommandBuffer = {};
	VK_CHECK_RESULT(vkAllocateCommandBuffers(LogicalDevice, &CommandBufferAllocateInfo, &CommandBuffer));

	// If requested, also start recording for the new command buffer
	if (BeginBuffer)
	{
		VkCommandBufferBeginInfo CommandBufferInfo = { };
		CommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferInfo.pNext = NULL;

		VK_CHECK_RESULT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferInfo));
	}

	return CommandBuffer;
}

void VulkanEncapsulatedDevice::FlushCommandBuffer(VkCommandBuffer CommandBuffer, VkQueue Queue, bool32 FreeBuffer)
{
	if (CommandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	VK_CHECK_RESULT(vkEndCommandBuffer(CommandBuffer));

	VkSubmitInfo SubmitInfo = { };
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &CommandBuffer;

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo FenceInfo = {};
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.flags = 0;
	VkFence Fence;
	VK_CHECK_RESULT(vkCreateFence(LogicalDevice, &FenceInfo, nullptr, &Fence));

	// Submit to the queue
	VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &SubmitInfo, Fence));
	// Wait for the fence to signal that command buffer has finished executing

	VkResult Result;

	Result = vkWaitForFences(LogicalDevice, 1, &Fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
	VK_CHECK_RESULT(Result);

	vkDestroyFence(LogicalDevice, Fence, nullptr);

	if (FreeBuffer)
	{
		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &CommandBuffer);
	}
}

void VulkanEncapsulatedDevice::SetupDevice(VkInstance Instance, VkQueue& Queue)
{
	VkResult Error;

	// Physical device
	uint32 GPUCount = 0;
	// Get number of available physical devices
	VK_CHECK_RESULT(vkEnumeratePhysicalDevices(Instance, &GPUCount, nullptr));
	Assert(GPUCount > 0);
	// Enumerate devices
	KList<VkPhysicalDevice> PhysicalDevices(GPUCount);
	Error = vkEnumeratePhysicalDevices(Instance, &GPUCount, PhysicalDevices.Data());
	if (Error)
	{
		KString ErrorMessage = "Could not enumerate phyiscal devices : \n";
		ErrorMessage.PushBack((const char)Error);
		ErrorMessage.PushBack("Fatal Error \n");

		K_ERROR(ErrorMessage);
	}

	// Note :
	// This example will always use the first physical device reported,
	// change the vector index if you have multiple Vulkan devices installed
	// and want to use another one
	PhysicalDevice = PhysicalDevices[0];

	// Vulkan device creation
	// This is handled by a separate class that gets a logical device representation
	// and encapsulates functions related to a device
	InitializeDevice(PhysicalDevice);
	VK_CHECK_RESULT(CreateLogicalDevice());

	// todo: remove
	// Store properties (including limits) and features of the phyiscal device
	// So examples can check against them and see if a feature is actually supported
	vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProperties);
	vkGetPhysicalDeviceFeatures(PhysicalDevice, &DeviceFeatures);
	// Gather physical device memory properties
	vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &DeviceMemoryProperties);

	// Get a graphics queue from the device
	vkGetDeviceQueue(LogicalDevice, QueueFamilyIndices.Graphics, 0, &Queue);
}

VkBool32 VulkanEncapsulatedDevice::GetSupportedDepthFormat(VkFormat* VulkanDepthFormat)
{
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	KList<VkFormat> DepthFormats(5);
	// TODO(Julian): Make this initializer list friendly
	DepthFormats.PushBack(VK_FORMAT_D32_SFLOAT_S8_UINT);
	DepthFormats.PushBack(VK_FORMAT_D32_SFLOAT);
	DepthFormats.PushBack(VK_FORMAT_D24_UNORM_S8_UINT);
	DepthFormats.PushBack(VK_FORMAT_D16_UNORM_S8_UINT);
	DepthFormats.PushBack(VK_FORMAT_D16_UNORM);


	for (uint32 Index = 0; Index < DepthFormats.Count(); ++Index)
	{
		VkFormatProperties FormatProperties;
		vkGetPhysicalDeviceFormatProperties(PhysicalDevice, DepthFormats[Index], &FormatProperties);
		// Format must support depth stencil attachment for optimal tiling
		if (FormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			*VulkanDepthFormat = DepthFormats[Index];
			return true;
		}
	}

	return false;
}

void* VulkanEncapsulatedDevice::operator new(memory_index Size)
{ 
	return MemAlloc(Size, 8);
}

void VulkanEncapsulatedDevice::operator delete(void * Block)
{ 
	MemDealloc(Block);
}
