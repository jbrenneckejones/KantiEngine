#ifndef VULKAN_HELPER

#include "KantiVulkan.h"

/** Helper function for memory info **/
inline VkMemoryAllocateInfo CreateMemoryAllocInfo()
{
	VkMemoryAllocateInfo MemoryAllocInfo = {};
	MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocInfo.pNext = NULL;
	MemoryAllocInfo.allocationSize = 0;
	MemoryAllocInfo.memoryTypeIndex = 0;

	return MemoryAllocInfo;
}

inline VkDescriptorSetAllocateInfo CreateDescriptorSetAllocateInfo(VkDescriptorPool DescriptorPool,
	const VkDescriptorSetLayout* SetLayouts, uint32 DescriptorSetCount)
{
	VkDescriptorSetAllocateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	Result.pNext = NULL;
	Result.descriptorPool = DescriptorPool;
	Result.pSetLayouts = SetLayouts;
	Result.descriptorSetCount = DescriptorSetCount;

	return Result;
}

inline VkWriteDescriptorSet CreateWriteDescriptorSet(VkDescriptorSet DescriptorSet, 
	VkDescriptorType DescriptorType, uint32 Binding, VkDescriptorBufferInfo* BufferInfo)
{
	VkWriteDescriptorSet Result = {};
	Result.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	Result.pNext = NULL;
	Result.dstSet = DescriptorSet;
	Result.descriptorType = DescriptorType;
	Result.dstBinding = Binding;
	Result.pBufferInfo = BufferInfo;
	// Default value in all examples
	Result.descriptorCount = 1;

	return Result;
}

inline VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(
	VkDescriptorType Type, VkShaderStageFlags StageFlags, uint32 Binding)
{
	VkDescriptorSetLayoutBinding Result = {};
	Result.descriptorType = Type;
	Result.stageFlags = StageFlags;
	Result.binding = Binding;
	// Default value in all examples
	Result.descriptorCount = 1; 

	return Result;
}

inline VkDescriptorSetLayoutCreateInfo CreateDescriptorSetLayoutCreateInfo(
	const VkDescriptorSetLayoutBinding* Bindings, uint32 BindingCount)
{
	VkDescriptorSetLayoutCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	Result.pNext = NULL;
	Result.pBindings = Bindings;
	Result.bindingCount = BindingCount;

	return Result;
}

inline VkPipelineLayoutCreateInfo CreatePipelineLayoutCreateInfo(
	const VkDescriptorSetLayout* SetLayouts, uint32 SetLayoutCount)
{
	VkPipelineLayoutCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	Result.pNext = NULL;
	Result.setLayoutCount = SetLayoutCount;
	Result.pSetLayouts = SetLayouts;

	return Result;
}

inline VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyStateCreateInfo(
	VkPrimitiveTopology Topology, VkPipelineInputAssemblyStateCreateFlags Flags, VkBool32 PrimitiveRestartEnable)
{
	VkPipelineInputAssemblyStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	Result.topology = Topology;
	Result.flags = Flags;
	Result.primitiveRestartEnable = PrimitiveRestartEnable;

	return Result;
}

inline VkPipelineRasterizationStateCreateInfo CreatePipelineRasterizationStateCreateInfo(
	VkPolygonMode PolygonMode, 
	VkCullModeFlags CullMode, 
	VkFrontFace FrontFace, 
	VkPipelineRasterizationStateCreateFlags Flags)
{
	VkPipelineRasterizationStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	Result.polygonMode = PolygonMode;
	Result.cullMode = CullMode;
	Result.frontFace = FrontFace;
	Result.flags = Flags;
	Result.depthClampEnable = VK_TRUE;
	Result.lineWidth = 1.0f;

	return Result;
}

inline VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState(
	VkColorComponentFlags ColorWriteMask, VkBool32 BlendEnable)
{
	VkPipelineColorBlendAttachmentState Result = {};
	Result.colorWriteMask = ColorWriteMask;
	Result.blendEnable = BlendEnable;

	return Result;
}

inline VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateCreateInfo(
	uint32 AttachmentCount,	const VkPipelineColorBlendAttachmentState* Attachments)
{
	VkPipelineColorBlendStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	Result.pNext = NULL;
	Result.attachmentCount = AttachmentCount;
	Result.pAttachments = Attachments;

	return Result;
}

inline VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateCreateInfo(
	VkBool32 DepthTestEnable, VkBool32 DepthWriteEnable, VkCompareOp DepthCompareOp)
{
	VkPipelineDepthStencilStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	Result.depthTestEnable = DepthTestEnable;
	Result.depthWriteEnable = DepthWriteEnable;
	Result.depthCompareOp = DepthCompareOp;
	Result.front = Result.back;
	Result.back.compareOp = VK_COMPARE_OP_ALWAYS;

	return Result;
}

inline VkPipelineViewportStateCreateInfo CreatePipelineViewportStateCreateInfo(
	uint32 ViewportCount, uint32 ScissorCount, VkPipelineViewportStateCreateFlags Flags)
{
	VkPipelineViewportStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	Result.viewportCount = ViewportCount;
	Result.scissorCount = ScissorCount;
	Result.flags = Flags;

	return Result;
}

inline VkPipelineMultisampleStateCreateInfo CreatePipelineMultisampleStateCreateInfo(
	VkSampleCountFlagBits RasterizationSamples, VkPipelineMultisampleStateCreateFlags Flags)
{
	VkPipelineMultisampleStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	Result.rasterizationSamples = RasterizationSamples;

	return Result;
}

inline VkPipelineDynamicStateCreateInfo CreatePipelineDynamicStateCreateInfo(
	const VkDynamicState* DynamicStates, uint32 DynamicStateCount, VkPipelineDynamicStateCreateFlags Flags)
{
	VkPipelineDynamicStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	Result.pDynamicStates = DynamicStates;
	Result.dynamicStateCount = DynamicStateCount;

	return Result;
}

inline VkGraphicsPipelineCreateInfo CreatePipelineCreateInfo(
	VkPipelineLayout Layout, VkRenderPass RenderPass, VkPipelineCreateFlags Flags)
{
	VkGraphicsPipelineCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	Result.pNext = NULL;
	Result.layout = Layout;
	Result.renderPass = RenderPass;
	Result.flags = Flags;

	return Result;
}

inline VkDescriptorPoolSize CreateDescriptorPoolSize(
	VkDescriptorType Type, uint32 DescriptorCount)
{
	VkDescriptorPoolSize Result = {};
	Result.type = Type;
	Result.descriptorCount = DescriptorCount;

	return Result;
}

inline VkDescriptorPoolCreateInfo CreateDescriptorPoolCreateInfo(
	uint32 PoolSizeCount, VkDescriptorPoolSize* PoolSizes, uint32 MaxSets)
{
	VkDescriptorPoolCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	Result.pNext = NULL;
	Result.poolSizeCount = PoolSizeCount;
	Result.pPoolSizes = PoolSizes;
	Result.maxSets = MaxSets;

	return Result;
}

inline VkCommandBufferBeginInfo CreateCommandBufferBeginInfo()
{
	VkCommandBufferBeginInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	Result.pNext = NULL;

	return Result;
}

inline VkRenderPassBeginInfo CreateRenderPassBeginInfo()
{
	VkRenderPassBeginInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	Result.pNext = NULL;

	return Result;
}

inline VkViewport CreateViewport(
	real32 Width, real32 Height, real32 MinDepth, real32 MaxDepth)
{
	VkViewport Result = {};
	Result.width = Width;
	Result.height = Height;
	Result.minDepth = MinDepth;
	Result.maxDepth = MaxDepth;

	return Result;
}

inline VkRect2D CreateRect2D(
	int32 Width, int32 Height, int32 OffsetX, int32 OffsetY)
{
	VkRect2D Result = {};
	Result.extent.width = Width;
	Result.extent.height = Height;
	Result.offset.x = OffsetX;
	Result.offset.y = OffsetY;

	return Result;
}

/** Helper function for buffer create info **/
inline VkBufferCreateInfo CreateBufferCreateInfo(VkBufferUsageFlags Flags, VkDeviceSize DeviceSize)
{
	VkBufferCreateInfo BufferCreateInfo = {};
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.usage = Flags;
	BufferCreateInfo.size = DeviceSize;
	BufferCreateInfo.flags = 0;

	return BufferCreateInfo;
}

inline device_creation_info CreateTransferBuffer(uint32 BufferSize, void* BufferData)
{
	device_creation_info Result = {};

	Result.BufferData = BufferData;
	Result.DeviceBuffer = 0;
	Result.DeviceMemory = 0;
	Result.DeviceSize = BufferSize;
	Result.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	Result.UsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	return Result;
}

inline device_creation_info CreateNonStagedBuffer(uint32 BufferSize, void* BufferData, VkBufferUsageFlags BufferFlags)
{
	device_creation_info Result = {};

	Result.BufferData = BufferData;
	Result.DeviceBuffer = 0;
	Result.DeviceMemory = 0;
	Result.DeviceSize = BufferSize;
	Result.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	Result.UsageFlags = BufferFlags;

	return Result;
}

inline device_creation_info CreateLocalBuffer(uint32 BufferSize, VkBufferUsageFlagBits BufferBit)
{
	device_creation_info Result = {};

	Result.BufferData = nullptr;
	Result.DeviceBuffer = 0;
	Result.DeviceMemory = 0;
	Result.DeviceSize = BufferSize;
	Result.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	Result.UsageFlags = BufferBit | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	return Result;
}

inline VkMemoryAllocateInfo CreateDefaultMemoryInfo()
{
	VkMemoryAllocateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	Result.pNext = NULL;
	Result.allocationSize = 0;
	Result.memoryTypeIndex = 0;

	return Result;
}

inline VkVertexInputBindingDescription CreateVertexInputBindingDescription(	
	uint32 Binding,	uint32 Stride, VkVertexInputRate InputRate)
{
	VkVertexInputBindingDescription Result = {};
	Result.binding = Binding;
	Result.stride = Stride;
	Result.inputRate = InputRate;
	return Result;
}

inline VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(
	uint32 Binding, uint32 Location, VkFormat Format, uint32 Offset)
{
	VkVertexInputAttributeDescription Result = {};
	Result.location = Location;
	Result.binding = Binding;
	Result.format = Format;
	Result.offset = Offset;

	return Result;
}

inline VkPipelineVertexInputStateCreateInfo CreatePipelineVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo Result = {};
	Result.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	Result.pNext = NULL;

	return Result;
}

#define VULKAN_HELPER
#endif