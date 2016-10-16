#include "KantiRenderManager.h"
#include "KantiVulkan/VulkanEncapsulatedDevice.h"
#include "KantiVulkan/VulkanBuffer.h"
#include "KantiVulkan/VulkanHelper.h"

#include "KantiPrimitives.h"
#include "KantiIntrinsics.h"

#include "KantiTimeManager.h"

#include "KantiRandomManager.h"

#include "KantiCameraManager.h"

#include "KantiFileManager.h"

void KantiRenderObject::PrepareUniformBuffer(VulkanEncapsulatedDevice* Device)
{
	// Vertex shader uniform buffer block
	VkMemoryAllocateInfo AllocationInfo = CreateDefaultMemoryInfo();
	VkMemoryRequirements MemoryRequirements;

	VkBufferCreateInfo BufferInfo = CreateBufferCreateInfo(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,	sizeof(RenderMatrices));

	VK_CHECK_RESULT(vkCreateBuffer(Device->LogicalDevice, &BufferInfo, nullptr, &UniformData.Buffer));
	vkGetBufferMemoryRequirements(Device->LogicalDevice, UniformData.Buffer, &MemoryRequirements);
	AllocationInfo.allocationSize = MemoryRequirements.size;
	AllocationInfo.memoryTypeIndex = Device->GetMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(Device->LogicalDevice, &AllocationInfo, nullptr, &UniformData.Memory));
	VK_CHECK_RESULT(vkBindBufferMemory(Device->LogicalDevice, UniformData.Buffer, UniformData.Memory, 0));

	UniformData.Descriptor.buffer = UniformData.Buffer;
	UniformData.Descriptor.offset = 0;
	UniformData.Descriptor.range = sizeof(RenderMatrices);
	UniformData.AllocSize = (uint32)AllocationInfo.allocationSize;
}

void KantiRenderObject::Draw(VkCommandBuffer CommandBuffer, VkPipelineLayout PipelineLayout)
{
	VkDeviceSize Offsets[1] = { 0 };
	vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
	vkCmdBindVertexBuffers(CommandBuffer, 0, 1, &VertexBuffer.Buffer, Offsets);
	vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(CommandBuffer, IndexCount, 1, 0, 0, 1);
}

void KantiRenderObject::UpdateUniformBuffer(VulkanEncapsulatedDevice* Device, KantiCameraManager* Camera, real32 Timer)
{
	RenderMatrices.Projection = Camera->Matrices.Perspective;
	RenderMatrices.View = Camera->Matrices.View;

	// RenderMatrices.View = Matrix4x4::LookAt(Vector3(0.0f, 0.0f, -Zoom), Vector3(-1.0f, -1.5f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

	// RenderMatrices.View = Matrix4x4::LookAt(Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));

	RenderMatrices.Model = Matrix4x4::Identity(1.0f);
	// RenderMatrices.Model = Rotate(RenderMatrices.Model, ToRadians(Rotation.X), Vector3(1.0f, 0.0f, 0.0f));
	// RenderMatrices.Model = Rotate(RenderMatrices.Model, ToRadians(Rotation.Y), Vector3(0.0f, 1.0f, 0.0f));
	// RenderMatrices.Model = Rotate(RenderMatrices.Model, ToRadians(Rotation.Z), Vector3(0.0f, 0.0f, 1.0f));

	// RenderMatrices.View = LookAt(Vector3(0.0f, 0.0f, -Zoom), Vector3(-1.0f, -1.5f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
	// RenderMatrices.View = Rotate(RenderMatrices.View, ToRadians(Rotation.X), Vector3(1.0f, 0.0f, 0.0f));
	// RenderMatrices.View = Rotate(RenderMatrices.View, ToRadians(Rotation.Y), Vector3(0.0f, 1.0f, 0.0f));

	// Matrix4x4 UBO = ((RenderMatrices.Projection * RenderMatrices.View) * RenderMatrices.Model) * Position;

	RenderMatrices.Model.Translate(Position);
	RenderMatrices.Model = RenderMatrices.Model * Rotation.ToMatrix();
	// Rotation.Z = (rotSpeed * timer) + rotOffset;
	// RenderMatrices.Model = Rotate(RenderMatrices.Model, ToRadians(rotation.z), Vector3(0.0f, 0.0f, 1.0f));

	// ubo.normal = glm::inverseTranspose(ubo.view * ubo.model);

	// ubo.lightPos = glm::vec3(0.0f, 0.0f, 2.5f);
	// ubo.lightPos.x = sin(glm::radians(timer)) * 8.0f;
	// ubo.lightPos.z = cos(glm::radians(timer)) * 8.0f;

	// Matrix4x4 Mat = ((RenderMatrices.Projection * RenderMatrices.View) * RenderMatrices.Model) * Vector4(Position, 1.0f);

	uint8* PointerData = 0;
	VK_CHECK_RESULT(vkMapMemory(Device->LogicalDevice, UniformData.Memory, 0, sizeof(RenderMatrices), 0, (void **)&PointerData));
	MemCopy(PointerData, &RenderMatrices, sizeof(RenderMatrices));
	vkUnmapMemory(Device->LogicalDevice, UniformData.Memory);
}

void KantiRenderObject::SetupDescriptorSet(VulkanEncapsulatedDevice* Device, VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout)
{
	VkDescriptorSetAllocateInfo AllocInfo = CreateDescriptorSetAllocateInfo(DescriptorPool,	&DescriptorSetLayout, 1);

	VK_CHECK_RESULT(vkAllocateDescriptorSets(Device->LogicalDevice, &AllocInfo, &DescriptorSet));

	// Binding 0 : Vertex shader uniform buffer
	VkWriteDescriptorSet WriteDescriptorSet = CreateWriteDescriptorSet(DescriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &UniformData.Descriptor);

	vkUpdateDescriptorSets(Device->LogicalDevice, 1, &WriteDescriptorSet, 0, NULL);
}

void KantiRenderObject::Generate(VulkanEncapsulatedDevice* Device, VkQueue Queue)
{
	KList<vertex> vBuffer;
	KList<uint32> iBuffer;

	// SetCubeVertex(vBuffer, iBuffer);
	render_data RenderData = {};
	KantiFileManager::LoadOBJFile("Hexagon Shape.obj", RenderData);
	// Pyrimid->VertexBuffer.
	vBuffer = RenderData.Vertices;
	iBuffer = RenderData.Indices;

	for(uint32 Index = 0; Index < vBuffer.Count(); ++Index)
	{
		vBuffer[Index].Color = Vector3(
			KantiRandomManager::RandomRangeUniform(0.0f, 1.0f),
			KantiRandomManager::RandomRangeUniform(0.0f, 1.0f),
			KantiRandomManager::RandomRangeUniform(0.0f, 1.0f));
	}

	uint32 VertexBufferSize = vBuffer.Count() * sizeof(vertex);
	uint32 IndexBufferSize = iBuffer.Count() * sizeof(uint32);

	bool32 UseStaging = true;

	if (UseStaging)
	{
		VulkanBuffer VertexStaging, IndexStaging;

		// Create staging buffers
		// Vertex data
		device_creation_info VertexInfo = CreateTransferBuffer(VertexBufferSize, vBuffer.Data());
		Device->CreateBuffer(&VertexInfo, &VertexStaging);
		// Index data
		device_creation_info IndiceInfo = CreateTransferBuffer(IndexBufferSize, iBuffer.Data());
		Device->CreateBuffer(&IndiceInfo, &IndexStaging);

		// Create device local buffers
		// Vertex buffer
		device_creation_info VertexLocalInfo = CreateLocalBuffer(VertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		Device->CreateBuffer(&VertexLocalInfo, &VertexBuffer);
		// Index buffer
		device_creation_info IndiceLocalInfo = CreateLocalBuffer(IndexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		Device->CreateBuffer(&IndiceLocalInfo, &IndexBuffer);

		// Copy from staging buffers
		VkCommandBuffer CopyCommand = Device->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

		VkBufferCopy CopyRegion = {};

		CopyRegion.size = VertexBufferSize;
		vkCmdCopyBuffer(
			CopyCommand,
			VertexStaging.Buffer,
			VertexBuffer.Buffer,
			1,
			&CopyRegion);

		CopyRegion.size = IndexBufferSize;
		vkCmdCopyBuffer(
			CopyCommand,
			IndexStaging.Buffer,
			IndexBuffer.Buffer,
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
		device_creation_info VertexInfo = CreateNonStagedBuffer(VertexBufferSize, vBuffer.Data(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
		Device->CreateBuffer(&VertexInfo, &VertexBuffer);
		// Index data
		device_creation_info IndiceInfo = CreateNonStagedBuffer(IndexBufferSize, iBuffer.Data(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		Device->CreateBuffer(&IndiceInfo, &IndexBuffer);
	}

	IndexCount = iBuffer.Count();

	PrepareUniformBuffer(Device);
}
