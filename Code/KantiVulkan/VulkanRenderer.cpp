#include "VulkanRenderer.h"

void VulkanRenderObject::PrepareUniformBuffer(VulkanEncapsulatedDevice* Device)
{
	// Vertex shader uniform buffer block
	VkMemoryAllocateInfo AllocationInfo = CreateDefaultMemoryInfo();
	VkMemoryRequirements MemoryRequirements;

	VkBufferCreateInfo BufferInfo = CreateBufferCreateInfo(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(MeshInfo->Matrices));

	VK_CHECK_RESULT(vkCreateBuffer(Device->LogicalDevice, &BufferInfo, nullptr, &UniformData.Buffer));
	vkGetBufferMemoryRequirements(Device->LogicalDevice, UniformData.Buffer, &MemoryRequirements);
	AllocationInfo.allocationSize = MemoryRequirements.size;
	AllocationInfo.memoryTypeIndex = Device->GetMemoryType(MemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(Device->LogicalDevice, &AllocationInfo, nullptr, &UniformData.Memory));
	VK_CHECK_RESULT(vkBindBufferMemory(Device->LogicalDevice, UniformData.Buffer, UniformData.Memory, 0));

	UniformData.Descriptor.buffer = UniformData.Buffer;
	UniformData.Descriptor.offset = 0;
	UniformData.Descriptor.range = sizeof(MeshInfo->Matrices);
	UniformData.AllocSize = (uint32)AllocationInfo.allocationSize;
}

void VulkanRenderObject::Draw(VkCommandBuffer DrawCommandBuffer, VkPipelineLayout PipelineLayout)
{
	VkDeviceSize Offsets[1] = { 0 };
	vkCmdBindDescriptorSets(DrawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescriptorSet, 0, NULL);
	vkCmdBindVertexBuffers(DrawCommandBuffer, 0, 1, &VertexBuffer.Buffer, Offsets);
	vkCmdBindIndexBuffer(DrawCommandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(DrawCommandBuffer, IndexCount, 1, 0, 0, 1);
}

void VulkanRenderObject::UpdateUniformBuffer(VulkanEncapsulatedDevice* Device)
{
	uint8* PointerData = 0;
	VK_CHECK_RESULT(vkMapMemory(Device->LogicalDevice, UniformData.Memory, 0, sizeof(MeshInfo->Matrices), 0, (void **)&PointerData));
	MemCopy(PointerData, &MeshInfo->Matrices, sizeof(MeshInfo->Matrices));
	vkUnmapMemory(Device->LogicalDevice, UniformData.Memory);
}

void VulkanRenderObject::SetupDescriptorSet(VulkanRenderer* Renderer)
{
	VkDescriptorSetAllocateInfo AllocInfo = CreateDescriptorSetAllocateInfo(
		Renderer->CommandBuffer.DescriptorPool, &Renderer->DescriptorSetLayout, 1);

	VK_CHECK_RESULT(vkAllocateDescriptorSets(Renderer->Device->LogicalDevice, &AllocInfo, &DescriptorSet));

	// Image descriptor for the shadow map attachment
	VkDescriptorImageInfo TexDescriptor =
		CreateDescriptorImageInfo(
			Renderer->OffScreenFrameBuffer.DepthSampler,
			Renderer->OffScreenFrameBuffer.Depth.View,
			VK_IMAGE_LAYOUT_GENERAL);

	KList<VkWriteDescriptorSet> WriteDescriptorSets =
	{
		// Binding 0 : Vertex shader uniform buffer
		CreateWriteDescriptorSet(
			DescriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&UniformData.Descriptor),
		// Binding 1 : Fragment shader texture sampler
		CreateWriteDescriptorSet(
			DescriptorSet,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1,
			&TexDescriptor)
	};

	vkUpdateDescriptorSets(Renderer->Device->LogicalDevice, WriteDescriptorSets.Count(), WriteDescriptorSets.Data(), 0, NULL);

	// Offscreen
	VK_CHECK_RESULT(vkAllocateDescriptorSets(Renderer->Device->LogicalDevice, &AllocInfo, &Renderer->DescriptorSets.Offscreen));

	KList<VkWriteDescriptorSet> OffScreenWriteDescriptorSets =
	{
		// Binding 0 : Vertex shader uniform buffer
		CreateWriteDescriptorSet(
			Renderer->DescriptorSets.Offscreen,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&Renderer->UniformData.Offscreen.Descriptor),
	};

	vkUpdateDescriptorSets(Renderer->Device->LogicalDevice, OffScreenWriteDescriptorSets.Count(), OffScreenWriteDescriptorSets.Data(), 0, NULL);

	// 3D scene
	VK_CHECK_RESULT(vkAllocateDescriptorSets(Renderer->Device->LogicalDevice, &AllocInfo, &Renderer->DescriptorSets.Scene));

	// Image descriptor for the shadow map attachment
	TexDescriptor.sampler = Renderer->OffScreenFrameBuffer.DepthSampler;
	TexDescriptor.imageView = Renderer->OffScreenFrameBuffer.Depth.View;

	KList<VkWriteDescriptorSet> SceneDescriptorSets =
	{
		// Binding 0 : Vertex shader uniform buffer
		CreateWriteDescriptorSet(
			Renderer->DescriptorSets.Scene,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&Renderer->UniformData.Scene.Descriptor),
		// Binding 1 : Fragment shader shadow sampler
		CreateWriteDescriptorSet(
			Renderer->DescriptorSets.Scene,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1,
			&TexDescriptor)
	};
	vkUpdateDescriptorSets(Renderer->Device->LogicalDevice, SceneDescriptorSets.Count(), SceneDescriptorSets.Data(), 0, NULL);
}

void VulkanRenderObject::Generate(VulkanEncapsulatedDevice* Device, VkQueue Queue)
{
	KList<KVertex> vBuffer;
	KList<uint32> iBuffer;

	vBuffer = MeshInfo->MeshData.Vertices;
	iBuffer = MeshInfo->MeshData.Indices;

	uint32 VertexBufferSize = vBuffer.Count() * sizeof(KVertex);
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

void VulkanRenderer::CreateMeshBuffer(KMeshRenderer* MeshRenderer)
{
	VulkanRenderObject RenderObject;
	RenderObject.MeshInfo = MeshRenderer;
	RenderObject.Generate(Device, Queue);
	RenderObject.PrepareUniformBuffer(Device);

	Meshes[MeshRenderer->InstanceID] = new VulkanRenderObject(RenderObject);

	SetupDescriptorPool();
	ExtraPrepare();
	// vkUpdateDescriptorSets(Device->LogicalDevice, );

	// SetupDescriptorPool();
	// RenderObject.SetupDescriptorSet(Device, CommandBuffer.DescriptorPool, DescriptorSetLayout);
	// BuildCommandBuffers();
	
}
