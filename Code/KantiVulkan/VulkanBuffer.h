#ifndef VULKAN_BUFFER


#include "KantiVulkan.h"

struct buffer_creation_info
{
	VkBufferUsageFlags UsageFlags;
	VkMemoryPropertyFlags MemoryPropertyFlags;
	VkDeviceSize DeviceSize;
	void* BufferData;
	VkBuffer* Buffer;
	VkDeviceMemory* DeviceMemory;
	VkDescriptorBufferInfo* DescriptorBufferInfo;

	class VulkanEncapsulatedDevice* EncapsulatedDevice;
};

class VulkanBuffer
{
	public:

	VkBuffer Buffer;
	VkDevice Device;
	VkDeviceMemory DeviceMemory;
	VkDescriptorBufferInfo BufferInfo;
	VkDeviceSize BufferSize = 0;
	VkDeviceSize BufferAlignment = 0;
	void* BufferMappedData = nullptr;

	/** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
	VkBufferUsageFlags BufferUsageFlags;
	/** @brief Memory propertys flags to be filled by external source at buffer creation (to query at some later point) */
	VkMemoryPropertyFlags MemoryPropertyFlags;

	/** Helper function for memory range **/
	VkMappedMemoryRange GetMappedMemoryRange(VkDeviceMemory Memory, VkDeviceSize Offset, VkDeviceSize Size);

	/** 
	* Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	* 
	* @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	* 
	* @return VkResult of the buffer mapping call
	*/
	VkResult MapBuffer(VkDeviceSize DeviceSize = VK_WHOLE_SIZE, VkDeviceSize DeviceOffset = 0);

	/**
	* Unmap a mapped memory range
	*
	* @note Does not return a result as vkUnmapMemory can't fail
	*/
	void UnmapBuffer();

	/** 
	* Attach the allocated memory block to the buffer
	* 
	* @param offset (Optional) Byte offset (from the beginning) for the memory region to bind
	* 
	* @return VkResult of the bindBufferMemory call
	*/
	VkResult BindBuffer(VkDeviceSize DeviceOffset = 0);

	/**
	* Setup the default descriptor for this buffer
	*
	* @param size (Optional) Size of the memory range of the descriptor
	* @param offset (Optional) Byte offset from beginning
	*
	*/
	void SetupBufferInfo(VkDeviceSize DeviceSize = VK_WHOLE_SIZE, VkDeviceSize DeviceOffset = 0);

	/** 
	* Flush a memory range of the buffer to make it visible to the device
	*
	* @note Only required for non-coherent memory
	*
	* @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	*
	* @return VkResult of the flush call
	*/
	VkResult FlushBuffer(VkDeviceSize DeviceSize = VK_WHOLE_SIZE, VkDeviceSize DeviceOffset = 0);

	/**
	* Invalidate a memory range of the buffer to make it visible to the host
	*
	* @note Only required for non-coherent memory
	*
	* @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
	* @param offset (Optional) Byte offset from beginning
	*
	* @return VkResult of the invalidate call
	*/
	VkResult InvalidateBuffer(VkDeviceSize DeviceSize = VK_WHOLE_SIZE, VkDeviceSize DeviceOffset = 0);

	/** 
	* Release all Vulkan resources held by this buffer
	*/
	void DestroyBuffer();

	// Create a buffer, fill it with data (if != NULL) and bind buffer memory
	VkBool32 CreateBuffer(buffer_creation_info* Info);

};

#define VULKAN_BUFFER
#endif