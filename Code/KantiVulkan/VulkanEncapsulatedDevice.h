#ifndef VULKAN_ENCAPSULATED_DEVICE

struct device_creation_info
{
	VkBufferUsageFlags UsageFlags;
	VkMemoryPropertyFlags MemoryPropertyFlags;
	VkDeviceSize DeviceSize;
	VkBuffer* DeviceBuffer;
	VkDeviceMemory* DeviceMemory;
	void* BufferData = nullptr;
};

/**
* @brief Encapsulates access to a Vulkan buffer backed up by device memory
* @note To be filled by an external source like the VulkanDevice
*/
class VulkanEncapsulatedDevice
{
	public:

	/** @brief Physical device representation */
	VkPhysicalDevice PhysicalDevice;
	/** @brief Logical device, application's view of the physical device (GPU) */
	// todo: getter? should always point to VulkanDevice->device
	VkDevice LogicalDevice;
	/** @brief Properties of the physical device including limits that the application can check against */
	VkPhysicalDeviceProperties DeviceProperties;
	/** @brief Features of the physical device that an application can use to check if a feature is supported */
	VkPhysicalDeviceFeatures DeviceFeatures;
	/** @brief Memory types and heaps of the physical device */
	VkPhysicalDeviceMemoryProperties DeviceMemoryProperties;

	renderer_platform* Platform;

	/** @brief Default command pool for the graphics queue family index */
	VkCommandPool CommandPool = VK_NULL_HANDLE;

	/** @brief Set to true when the debug marker extension is detected */
	bool32 EnableDebugMarkers = false;

	/**  @brief Contains queue family indices */
	struct
	{
		uint32 Graphics = 0;
		uint32 Compute = 0;
	} QueueFamilyIndices;

	/* Need to setup device right after */
	VulkanEncapsulatedDevice();

	/**
	* Default constructor
	*
	* @param physicalDevice Phyiscal device that is to be used
	*/
	VulkanEncapsulatedDevice(VkPhysicalDevice NewPhysicalDevice);

	/** 
	* Default destructor
	*
	* @note Frees the logical device
	*/
	~VulkanEncapsulatedDevice();

	void InitializeDevice(VkPhysicalDevice NewDevice);

	VkBool32 CheckDeviceExtensionPresent(VkPhysicalDevice PhysicalDeviceToCheck, KString ExtensionName);

	/**
	* Get the index of a memory type that has all the requested property bits set
	*
	* @param typeBits Bitmask with bits set for each memory type supported by the resource to request for (from VkMemoryRequirements)
	* @param properties Bitmask of properties for the memory type to request
	* @param (Optional) memTypeFound Pointer to a bool that is set to true if a matching memory type has been found
	* 
	* @return Index of the requested memory type
	*
	* @throw Throws an exception if memTypeFound is null and no memory type could be found that supports the requested properties
	*/
	uint32 GetMemoryType(uint32 TypeBits, VkMemoryPropertyFlags MemoryProperties, VkBool32* MemTypeFound = nullptr);

	/**
	* Get the index of a queue family that supports the requested queue flags
	*
	* @param queueFlags Queue flags to find a queue family index for
	*
	* @return Index of the queue family index that matches the flags
	*
	* @throw Throws an exception if no queue family index could be found that supports the requested flags
	*/
	uint32 GetQueueFamiliyIndex(VkQueueFlagBits QueueFlags);

	/**
	* Create the logical device based on the assigned physical device, also gets default queue family indices
	*
	* @param enabledFeatures Can be used to enable certain features upon device creation
	* @param useSwapChain Set to false for headless rendering to omit the swapchain device extensions
	*
	* @return VkResult of the device creation call
	*/
	VkResult CreateLogicalDevice(bool32 UseSwapChain = true);

	/**
	* Create a buffer on the device
	*
	* @param usageFlags Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer)
	* @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
	* @param size Size of the buffer in byes
	* @param buffer Pointer to the buffer handle acquired by the function
	* @param memory Pointer to the memory handle acquired by the function
	* @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
	*
	* @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
	*/
	VkResult CreateBuffer(device_creation_info* Info);

	/**
	* Create a buffer on the device
	*
	* @param usageFlags Usage flag bitmask for the buffer (i.e. index, vertex, uniform buffer)
	* @param memoryPropertyFlags Memory properties for this buffer (i.e. device local, host visible, coherent)
	* @param buffer Pointer to a vk::Vulkan buffer object
	* @param size Size of the buffer in byes
	* @param data Pointer to the data that should be copied to the buffer after creation (optional, if not set, no data is copied over)
	*
	* @return VK_SUCCESS if buffer handle and memory have been created and (optionally passed) data has been copied
	*/
	VkResult CreateBuffer(device_creation_info* Info, class VulkanBuffer* Buffer);

	/**
	* Copy buffer data from src to dst using VkCmdCopyBuffer
	* 
	* @param src Pointer to the source buffer to copy from
	* @param dst Pointer to the destination buffer to copy tp
	* @param queue Pointer
	* @param copyRegion (Optional) Pointer to a copy region, if NULL, the whole buffer is copied
	*
	* @note Source and destionation pointers must have the approriate transfer usage flags set (TRANSFER_SRC / TRANSFER_DST)
	*/
	void CopyBuffer(class VulkanBuffer* Source, class VulkanBuffer* Destination, VkQueue Queue, VkBufferCopy* CopyRegion = nullptr);

	/** 
	* Create a command pool for allocation command buffers from
	* 
	* @param queueFamilyIndex Family index of the queue to create the command pool for
	* @param createFlags (Optional) Command pool creation flags (Defaults to VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
	*
	* @note Command buffers allocated from the created pool can only be submitted to a queue with the same family index
	*
	* @return A handle to the created command buffer
	*/
	VkCommandPool CreateCommandPool(uint32 QueueFamilyIndex, VkCommandPoolCreateFlags CommandCreateFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	/**
	* Allocate a command buffer from the command pool
	*
	* @param level Level of the new command buffer (primary or secondary)
	* @param (Optional) begin If true, recording on the new command buffer will be started (vkBeginCommandBuffer) (Defaults to false)
	*
	* @return A handle to the allocated command buffer
	*/
	VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel BufferLevel, bool32 BeginBuffer = false);

	/**
	* Finish command buffer recording and submit it to a queue
	*
	* @param commandBuffer Command buffer to flush
	* @param queue Queue to submit the command buffer to 
	* @param free (Optional) Free the command buffer once it has been submitted (Defaults to true)
	*
	* @note The queue that the command buffer is submitted to must be from the same family index as the pool it was allocated from
	* @note Uses a fence to ensure command buffer has finished executing
	*/
	void FlushCommandBuffer(VkCommandBuffer CommandBuffer, VkQueue Queue, bool32 FreeBuffer = true);


	void SetupDevice(VkInstance Instance, VkQueue& Queue);

	VkBool32 GetSupportedDepthFormat(VkFormat* VulkanDepthFormat);


	static void *operator new(memory_index Size);

	static void operator delete(void *Block);
};

#define VULKAN_ENCAPSULATED_DEVICE
#endif