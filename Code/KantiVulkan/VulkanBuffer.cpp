#include "VulkanBuffer.h"

VkMappedMemoryRange VulkanBuffer::GetMappedMemoryRange(VkDeviceMemory Memory, VkDeviceSize Offset, VkDeviceSize Size)
{
	VkMappedMemoryRange MappedRange = {};
	MappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	MappedRange.memory = Memory;
	MappedRange.offset = Offset;
	MappedRange.size = Size;

	return MappedRange;
}

VkResult VulkanBuffer::MapBuffer(VkDeviceSize DeviceSize, VkDeviceSize DeviceOffset)
{
	return vkMapMemory(Device, DeviceMemory, DeviceOffset, DeviceSize, 0, &BufferMappedData);
}

void VulkanBuffer::UnmapBuffer()
{
	if (BufferMappedData)
	{
		vkUnmapMemory(Device, DeviceMemory);
		BufferMappedData = nullptr;
	}
}

VkResult VulkanBuffer::BindBuffer(VkDeviceSize DeviceOffset)
{
	return vkBindBufferMemory(Device, Buffer, DeviceMemory, DeviceOffset);
}

void VulkanBuffer::SetupBufferInfo(VkDeviceSize DeviceSize, VkDeviceSize DeviceOffset)
{
	BufferInfo.offset = DeviceOffset;
	BufferInfo.buffer = Buffer;
	BufferInfo.range = DeviceSize;
}

VkResult VulkanBuffer::FlushBuffer(VkDeviceSize DeviceSize, VkDeviceSize DeviceOffset)
{
	VkMappedMemoryRange MemoryRange = GetMappedMemoryRange(DeviceMemory, DeviceOffset, DeviceSize);
	return vkFlushMappedMemoryRanges(Device, 1, &MemoryRange);
}

VkResult VulkanBuffer::InvalidateBuffer(VkDeviceSize DeviceSize, VkDeviceSize DeviceOffset)
{
	VkMappedMemoryRange MemoryRange = GetMappedMemoryRange(DeviceMemory, DeviceOffset, DeviceSize);
	return vkInvalidateMappedMemoryRanges(Device, 1, &MemoryRange);
}

void VulkanBuffer::DestroyBuffer()
{
	if (Buffer)
	{
		vkDestroyBuffer(Device, Buffer, nullptr);
	}
	if (DeviceMemory)
	{
		vkFreeMemory(Device, DeviceMemory, nullptr);
	}
}

VkBool32 VulkanBuffer::CreateBuffer(buffer_creation_info* Info)
{
	Assert(Info);

	VkMemoryRequirements MemoryRequirements;
	VkMemoryAllocateInfo MemoryAllocInfo = CreateMemoryAllocInfo();

	VkBufferCreateInfo BufferCreateInfo = CreateBufferCreateInfo(Info->UsageFlags, Info->DeviceSize);

	VK_CHECK_RESULT(vkCreateBuffer(Device, &BufferCreateInfo, nullptr, &Buffer));

	vkGetBufferMemoryRequirements(Device, *Info->Buffer, &MemoryRequirements);
	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocInfo.memoryTypeIndex = Info->EncapsulatedDevice->GetMemoryType(MemoryRequirements.memoryTypeBits, MemoryPropertyFlags);
	VK_CHECK_RESULT(vkAllocateMemory(Device, &MemoryAllocInfo, nullptr, &DeviceMemory));
	if(Info->BufferData != nullptr)
	{
		void* MappedData = 0;
		VK_CHECK_RESULT(vkMapMemory(Device, *Info->DeviceMemory, 0, Info->DeviceSize, 0, &MappedData));

		MemCopy(MappedData, Info->BufferData, Info->DeviceSize);
		vkUnmapMemory(Device, *Info->DeviceMemory);
	}
	VK_CHECK_RESULT(vkBindBufferMemory(Device, *Info->Buffer, *Info->DeviceMemory, 0));

	if(Info->DescriptorBufferInfo)
	{
		Info->DescriptorBufferInfo->offset = 0;
		Info->DescriptorBufferInfo->buffer = *Info->Buffer;
		Info->DescriptorBufferInfo->range = Info->DeviceSize;
	}

	return true;
}