#include "KantiVulkan.h"
#include "VulkanRenderer.h"

// Vulkan Device

// End Vulkan Device

// Vulkan render object

/*

void vulkan_render_object::PrepareUniformBuffer()
{
	// Vertex shader uniform buffer block
	VkMemoryAllocateInfo MemoryAllocInfo = {};
	MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocInfo.pNext = NULL;
	MemoryAllocInfo.allocationSize = 0;
	MemoryAllocInfo.memoryTypeIndex = 0;

	VkMemoryRequirements MemoryRequirements;

	VkBufferCreateInfo BufferCreateInfo = { };
	BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferCreateInfo.pNext = NULL;
	BufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	BufferCreateInfo.size = sizeof(ObjectData.UniformBuffer);
	BufferCreateInfo.flags = 0;

	VK_CHECK_RESULT(vkCreateBuffer(Device->LogicalDevice, &BufferCreateInfo, nullptr, &BufferData.UniformData->Buffer));
	vkGetBufferMemoryRequirements(Device->LogicalDevice, BufferData.UniformData->Buffer, &MemoryRequirements);
	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	MemoryAllocInfo.memoryTypeIndex = Device->GetMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(Device->LogicalDevice, &MemoryAllocInfo, nullptr, &BufferData.UniformData->Memory));
	VK_CHECK_RESULT(vkBindBufferMemory(Device->LogicalDevice,  BufferData.UniformData->Buffer,  BufferData.UniformData->Memory, 0));

	BufferData.UniformData->Descriptor.buffer =  BufferData.UniformData->Buffer;
	BufferData.UniformData->Descriptor.offset = 0;
	BufferData.UniformData->Descriptor.range = sizeof(ObjectData.UniformBuffer);
	BufferData.UniformData->AllocSize = MemoryAllocInfo.allocationSize;
}

vulkan_render_object::vulkan_render_object(platform_buffer_data& PlatformBufferData, vulkan_device* VulkanDevice)
{
	BufferData = (vulkan_buffer_data&)PlatformBufferData;
	Device = VulkanDevice;
}

void vulkan_render_object::Draw(VkCommandBuffer CommandBuffer, VkPipelineLayout PipelineLayout)
{
	VkDeviceSize Offsets[1] = { 0 };
	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &BufferData.VertexBuffer->Buffer, Offsets);
	vkCmdBindIndexBuffer(CommandBuffer, BufferData.IndexBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(CommandBuffer, BufferData.IndexCount, 1, 0, 0, 1);
}

void vulkan_render_object::UpdateUniformBuffer(Matrix4x4 Perspective, Vector3 Rotation, real32 Zoom, real32 Timer)
{
	ObjectData.UniformBuffer.Projection = Perspective;

	ObjectData.UniformBuffer.View = LookAt(
		Vector3(0.0f, 0.0f, -Zoom),
		Vector3(-1.0f, -1.5, 0.0f),
		Vector3(0.0f, 1.0f, 0.0f)
	);

	// ObjectData.UniformBuffer.View = Rotate(ObjectData.UniformBuffer.View, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	// ObjectData.UniformBuffer.View = Rotate(ObjectData.UniformBuffer.View, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ObjectData.UniformBuffer.Model = Matrix4x4();
	ObjectData.UniformBuffer.Model = Translate(ObjectData.UniformBuffer.Model, ObjectData.Position);
	// rotation.z = (rotSpeed * timer) + rotOffset;
	// ObjectData.UniformBuffer.Model = glm::rotate(ubo.model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// ubo.normal = glm::inverseTranspose(ubo.view * ubo.model);

	// ubo.lightPos = glm::vec3(0.0f, 0.0f, 2.5f);
	// ubo.lightPos.x = sin(glm::radians(timer)) * 8.0f;
	// ubo.lightPos.z = cos(glm::radians(timer)) * 8.0f;

	vulkan_buffer_data* UniformData = (vulkan_buffer_data *)ObjectData.BufferData;

	uint8* PointerData;
	VK_CHECK_RESULT(vkMapMemory(Device->LogicalDevice, , UniformData->UniformData->Memory, 0, sizeof(ObjectData.UniformBuffer), 0, (void **)&PointerData));
	MemCopy(PointerData, &ObjectData.UniformBuffer, sizeof(ObjectData.UniformBuffer));
	vkUnmapMemory(Device->LogicalDevice, UniformData->UniformData->Memory);
}

void vulkan_render_object::SetupDescriptorSet(VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout)
{
	VkDescriptorSetAllocateInfo DescriptorAllocInfo = {};
	DescriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorAllocInfo.pNext = NULL;
	DescriptorAllocInfo.descriptorPool = DescriptorPool;
	DescriptorAllocInfo.pSetLayouts = &DescriptorSetLayout;
	DescriptorAllocInfo.descriptorSetCount = 1;

	VK_CHECK_RESULT(vkAllocateDescriptorSets(Device->LogicalDevice, &DescriptorAllocInfo, &DescriptorSet));

	// Binding 0 : Vertex shader uniform buffer
	VkWriteDescriptorSet WriteDescriptorSet = {};
	WriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	WriteDescriptorSet.pNext = NULL;
	WriteDescriptorSet.dstSet = DescriptorSet;
	WriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	WriteDescriptorSet.dstBinding = 0;
	WriteDescriptorSet.pBufferInfo = &BufferData.UniformData->Descriptor;
	// Default value in all examples
	WriteDescriptorSet.descriptorCount = 1;

	vkUpdateDescriptorSets(Device->LogicalDevice, 1, &WriteDescriptorSet, 0, NULL);
}

void vulkan_render_object::Generate(k_object* CreationInfo, VkQueue Queue)
{
	ObjectData.Color = CreationInfo->Color;
	ObjectData.Position = CreationInfo->Position;
	ObjectData.Rotation = CreationInfo->Rotation;

	std::vector<vertex> VertexBuffer;
	std::vector<uint32> IndexBuffer;

	Vector3 Normal;

	uint32 VertexBufferSize = VertexBuffer.size() * sizeof(vertex);
	uint32 IndexBufferSize = IndexBuffer.size() * sizeof(uint32);

	bool UseStaging = true;

	if (UseStaging)
	{
		VulkanBuffer VertexStaging, IndexStaging;

		device_creation_info VertexInfo = {};
		VertexInfo.UsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		VertexInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		VertexInfo.BufferData = VertexBuffer.data();
		VertexInfo.DeviceSize = VertexBufferSize;

		// Create staging buffers
		// Vertex data
		Device->CreateBuffer(&VertexInfo, &VertexStaging);

		device_creation_info IndexInfo = {};
		IndexInfo.UsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		IndexInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		IndexInfo.BufferData = IndexBuffer.data();
		IndexInfo.DeviceSize = IndexBufferSize;

		// Index data
		Device->CreateBuffer(&IndexInfo, &IndexStaging);

		// Create device local buffers

		device_creation_info VertexLocalInfo = {};
		VertexLocalInfo.UsageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		VertexLocalInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VertexLocalInfo.BufferData = VertexBuffer.data();
		VertexLocalInfo.DeviceSize = VertexBufferSize;
		// Vertex buffer
		Device->CreateBuffer(&VertexLocalInfo);

		device_creation_info IndexLocalInfo = {};
		IndexLocalInfo.UsageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		IndexLocalInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		IndexLocalInfo.BufferData = IndexBuffer.data();
		IndexLocalInfo.DeviceSize = IndexBufferSize;
		// Index buffer
		Device->CreateBuffer(&IndexLocalInfo);

		// Copy from staging buffers
		VkCommandBuffer CopyCommand = Device->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBufferCopy CopyRegion = {};

		CopyRegion.size = VertexBufferSize;
		vkCmdCopyBuffer(
			CopyCommand,
			VertexStaging.Buffer,
			(VkBuffer)CreationInfo->BufferData->VertexBuffer,
			1,
			&CopyRegion);

		CopyRegion.size = IndexBufferSize;
		vkCmdCopyBuffer(
			CopyCommand,
			IndexStaging.Buffer,
			(VkBuffer)CreationInfo->BufferData->IndexBuffer,
			1,
			&CopyRegion);

		Device->FlushCommandBuffer(CopyCommand, Queue, true);

		vkDestroyBuffer(Device->LogicalDevice, VertexStaging.Buffer, nullptr);
		vkFreeMemory(Device->LogicalDevice, VertexStaging.DeviceMemory, nullptr);
		vkDestroyBuffer(Device->LogicalDevice, IndexStaging.Buffer, nullptr);
		vkFreeMemory(Device->LogicalDevice, IndexStaging.DeviceMemory, nullptr);
	}
	else
	{
		device_creation_info VertexLocalInfo = {};
		VertexLocalInfo.UsageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		VertexLocalInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		VertexLocalInfo.BufferData = VertexBuffer.data();
		VertexLocalInfo.DeviceSize = VertexBufferSize;
		// Vertex buffer
		Device->CreateBuffer(&VertexLocalInfo, BufferData.VertexBuffer);

		device_creation_info IndexLocalInfo = {};
		IndexLocalInfo.UsageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		IndexLocalInfo.MemoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		IndexLocalInfo.BufferData = IndexBuffer.data();
		IndexLocalInfo.DeviceSize = IndexBufferSize;
		// Index buffer
		Device->CreateBuffer(&IndexLocalInfo, BufferData.IndexBuffer);
	}

	BufferData.IndexCount = IndexBuffer.size();

	PrepareUniformBuffer();
}

*/


PLATFORM_GAME_RENDER_INITIALIZE(VulkanInitialize)
{
	vulkan_base_properties Properties = {};
	Properties.ApplicationName = "Kanti Engine";
	Properties.DestinationHeight = 1280;
	Properties.DestinationWidth = 720;
	Properties.EnableDebugMarkers = true;
	// Properties.EnabledFeatures = ;
	Properties.EnableValidation = true;
	Properties.EnableVSync = false;

	VulkanRendererInstance = new VulkanRenderer(Platform, Properties);
	VulkanRendererInstance->Prepare();

	return;
}

PLATFORM_GAME_UPDATE_AND_RENDER(VulkanUpdateAndRender)
{
	VulkanRendererInstance->Render();
	return;
}
