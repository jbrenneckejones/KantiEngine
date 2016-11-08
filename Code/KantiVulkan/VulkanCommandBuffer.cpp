#include "VulkanCommandBuffer.h"

VkCommandBuffer* VulkanCommandBuffer::GetCurrentDrawBuffer()
{
	return &DrawCommandBuffers[CurrentBuffer];
}

void VulkanCommandBuffer::BuildCommandBuffers(commandbuffer_render_info& Info, std::map<UniqueID, VulkanRenderObject*> Meshes)
{
	VkCommandBufferBeginInfo CommandBufferInfo = { };
	CommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferInfo.pNext = NULL;

	VkClearValue ClearValues[2];
	ClearValues[0].color = Info.Color;
	ClearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo RenderPassBeginInfo = { };
	RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassBeginInfo.pNext = NULL;

	RenderPassBeginInfo.renderPass = RenderPass;
	RenderPassBeginInfo.renderArea.offset.x = 0;
	RenderPassBeginInfo.renderArea.offset.y = 0;
	RenderPassBeginInfo.renderArea.extent.width = Info.Width;
	RenderPassBeginInfo.renderArea.extent.height = Info.Height;
	RenderPassBeginInfo.clearValueCount = 2;
	RenderPassBeginInfo.pClearValues = ClearValues;

	for (int32 Index = 0; Index < (int32)DrawCommandBuffers.Count(); ++Index)
	{
		RenderPassBeginInfo.framebuffer = FrameBuffers[Index];

		VK_CHECK_RESULT(vkBeginCommandBuffer(DrawCommandBuffers[Index], &CommandBufferInfo));

		vkCmdBeginRenderPass(DrawCommandBuffers[Index], &RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport Viewport = { };
		Viewport.width = (real32)Info.Width;
		Viewport.height = (real32)Info.Height;
		Viewport.minDepth =  0.0f;
		Viewport.maxDepth = 1.0f;

		vkCmdSetViewport(DrawCommandBuffers[Index], 0, 1, &Viewport);

		VkRect2D Scissor = { };
		Scissor.extent.width = Info.Width;
		Scissor.extent.height = Info.Height;
		Scissor.offset.x = 0;
		Scissor.offset.y = 0;

		vkCmdSetScissor(DrawCommandBuffers[Index], 0, 1, &Scissor);
		vkCmdBindPipeline(DrawCommandBuffers[Index], VK_PIPELINE_BIND_POINT_GRAPHICS, Info.Pipeline);

		for(auto Mesh : Meshes)
		{
			Mesh.second->Draw(*GetCurrentDrawBuffer(), Info.PipelineLayout); //, Info.DescriptorSet);
		}

		vkCmdEndRenderPass(DrawCommandBuffers[Index]);

		VK_CHECK_RESULT(vkEndCommandBuffer(DrawCommandBuffers[Index]));
	}
}

void VulkanCommandBuffer::BuildPresentCommandBuffers(VulkanSwapChain* SwapChain)

{
	Assert(SwapChain);

	VkCommandBufferBeginInfo CommandBufferInfo = { };
	CommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferInfo.pNext = NULL;

	for (uint32 Index = 0; Index < SwapChain->ImageCount; Index++)
	{
		// Command buffer for post present barrier

		// Insert a post present image barrier to transform the image back to a
		// color attachment that our render pass can write to
		// We always use undefined image layout as the source as it doesn't actually matter
		// what is done with the previous image contents

		VK_CHECK_RESULT(vkBeginCommandBuffer(PostPresentCommandBuffers[Index], &CommandBufferInfo));

		VkImageMemoryBarrier PostPresentBarrier = { };
		PostPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		PostPresentBarrier.pNext = NULL;
		// Some default values
		PostPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PostPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		PostPresentBarrier.srcAccessMask = 0;
		PostPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		PostPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		PostPresentBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		PostPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PostPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PostPresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		PostPresentBarrier.image = SwapChain->Buffers[Index].Image;

		vkCmdPipelineBarrier(
			PostPresentCommandBuffers[Index],
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &PostPresentBarrier);

		VK_CHECK_RESULT(vkEndCommandBuffer(PostPresentCommandBuffers[Index]));

		// Command buffers for pre present barrier

		// Submit a pre present image barrier to the queue
		// Transforms the (framebuffer) image layout from color attachment to present(khr) for presenting to the swap chain

		VK_CHECK_RESULT(vkBeginCommandBuffer(PrePresentCommandBuffers[Index], &CommandBufferInfo));

		VkImageMemoryBarrier PrePresentBarrier = { };
		PrePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		PrePresentBarrier.pNext = NULL;
		// Some default values
		PrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		PrePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		PrePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		PrePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		PrePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		PrePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PrePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		PrePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		PrePresentBarrier.image = SwapChain->Buffers[Index].Image;

		vkCmdPipelineBarrier(
			PrePresentCommandBuffers[Index],
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_FLAGS_NONE,
			0, nullptr, // No memory barriers,
			0, nullptr, // No buffer barriers,
			1, &PrePresentBarrier);

		VK_CHECK_RESULT(vkEndCommandBuffer(PrePresentCommandBuffers[Index]));
	}
}

void VulkanCommandBuffer::CreateCommandPool(VulkanSwapChain* SwapChain, VkDevice Device)
{
	VkCommandPoolCreateInfo CommandPoolInfo = {};
	CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolInfo.queueFamilyIndex = SwapChain->QueueNodeIndex;
	CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK_RESULT(vkCreateCommandPool(Device, &CommandPoolInfo, nullptr, &CommandPool));
}

void VulkanCommandBuffer::SetupFrameBuffer(framebuffer_render_info& Info, VulkanSwapChain* SwapChain)
{
	VkImageView Attachments[2];

	// Depth/Stencil attachment is the same for all frame buffers
	Attachments[1] = Info.View;

	VkFramebufferCreateInfo FrameBufferCreateInfo = {};
	FrameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FrameBufferCreateInfo.pNext = NULL;
	FrameBufferCreateInfo.renderPass = RenderPass;
	FrameBufferCreateInfo.attachmentCount = 2;
	FrameBufferCreateInfo.pAttachments = Attachments;
	FrameBufferCreateInfo.width = Info.Width;
	FrameBufferCreateInfo.height = Info.Height;
	FrameBufferCreateInfo.layers = 1;

	Assert(SwapChain);

	// Create frame buffers for every swap chain image
	FrameBuffers.Resize(SwapChain->ImageCount);
	for (uint32 Index = 0; Index < FrameBuffersCount; Index++)
	{
		Attachments[0] = SwapChain->Buffers[Index].View;
		VK_CHECK_RESULT(vkCreateFramebuffer(Info.Device, &FrameBufferCreateInfo, nullptr, &FrameBuffers[Index]));
	}
}

bool32 VulkanCommandBuffer::CheckCommandBuffers()
{
	for (VkCommandBuffer* CommandBuffer = DrawCommandBuffers.Data(); *CommandBuffer; ++CommandBuffer)
	{
		if (CommandBuffer == VK_NULL_HANDLE)
		{
			return false;
		}
	}
	return true;
}

void VulkanCommandBuffer::CreateCommandBuffers(VkDevice Device, uint32 ImageCount)
{
	// Create one command buffer per frame buffer
	// in the swap chain
	// Command buffers store a reference to the
	// frame buffer inside their render pass info
	// so for static usage withouth having to rebuild
	// them each frame, we use one per frame buffer

	DrawCommandBuffers.Resize(ImageCount);

	PrePresentCommandBuffers.Resize(ImageCount);

	PostPresentCommandBuffers.Resize(ImageCount);


	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { };
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = DrawCommandBuffers.Count();

	VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, DrawCommandBuffers.Data()));

	// Command buffers for submitting present barriers
	// One pre and post present buffer per swap chain image
	VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, PrePresentCommandBuffers.Data()));
	VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, PostPresentCommandBuffers.Data()));
}

void VulkanCommandBuffer::DestroyCommandBuffers(VkDevice Device)
{
	vkFreeCommandBuffers(Device, CommandPool, DrawCommandBuffers.Count(), DrawCommandBuffers.Data());
	vkFreeCommandBuffers(Device, CommandPool, PrePresentCommandBuffers.Count(), PrePresentCommandBuffers.Data());
	vkFreeCommandBuffers(Device, CommandPool, PostPresentCommandBuffers.Count(), PostPresentCommandBuffers.Data());
}

void VulkanCommandBuffer::CreateSetupCommandBuffer(VkDevice Device)
{
	if (SetupCommandBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(Device, CommandPool, 1, &SetupCommandBuffer);
		SetupCommandBuffer = VK_NULL_HANDLE; // todo : check if still necessary
	}

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { };
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &SetupCommandBuffer));

	VkCommandBufferBeginInfo CommandBufferBeginInfo = {};
	CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VK_CHECK_RESULT(vkBeginCommandBuffer(SetupCommandBuffer, &CommandBufferBeginInfo));
}

void VulkanCommandBuffer::FlushSetupCommandBuffer(VkDevice Device, VkQueue Queue)
{
	if(SetupCommandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	VK_CHECK_RESULT(vkEndCommandBuffer(SetupCommandBuffer));

	VkSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	CommandBufferSubmitInfo.commandBufferCount = 1;
	CommandBufferSubmitInfo.pCommandBuffers = &SetupCommandBuffer;

	VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &CommandBufferSubmitInfo, VK_NULL_HANDLE));
	VK_CHECK_RESULT(vkQueueWaitIdle(Queue));

	vkFreeCommandBuffers(Device, CommandPool, 1, &SetupCommandBuffer);
	SetupCommandBuffer = VK_NULL_HANDLE; 
}

VkCommandBuffer VulkanCommandBuffer::CreateCommandBuffer(VkDevice Device, VkCommandBufferLevel Level, bool32 Begin)
{
	VkCommandBuffer CommandBuffer = {};

	VkCommandBufferAllocateInfo CommandBufferAllocateInfo = { };
	CommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	CommandBufferAllocateInfo.commandPool = CommandPool;
	CommandBufferAllocateInfo.level = Level;
	CommandBufferAllocateInfo.commandBufferCount = 1;

	VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandBufferAllocateInfo, &CommandBuffer));

	// If requested, also start the new command buffer
	if (Begin)
	{
		VkCommandBufferBeginInfo CommandBufferBeginInfo = { };
		CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CommandBufferBeginInfo.pNext = NULL;

		VK_CHECK_RESULT(vkBeginCommandBuffer(CommandBuffer, &CommandBufferBeginInfo));
	}

	return CommandBuffer;
}

void VulkanCommandBuffer::FlushCommandBuffer(VkCommandBuffer CommandBuffer, VkQueue VulkanQueue, bool32 FreeBuffer, VkDevice Device)
{
	if (CommandBuffer == VK_NULL_HANDLE)
	{
		return;
	}

	VK_CHECK_RESULT(vkEndCommandBuffer(CommandBuffer));

	VkSubmitInfo CommandBufferSubmitInfo = {};
	CommandBufferSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	CommandBufferSubmitInfo.commandBufferCount = 1;
	CommandBufferSubmitInfo.pCommandBuffers = &CommandBuffer;

	VK_CHECK_RESULT(vkQueueSubmit(VulkanQueue, 1, &CommandBufferSubmitInfo, VK_NULL_HANDLE));
	VK_CHECK_RESULT(vkQueueWaitIdle(VulkanQueue));

	if (FreeBuffer)
	{
		vkFreeCommandBuffers(Device, CommandPool, 1, &CommandBuffer);
	}
}

void VulkanCommandBuffer::SetImageLayout(image_layout_info& Info, bool32 UseFixedSubresource)
{
	if(UseFixedSubresource)
	{
		Info.SubresourceRange = {};
		Info.SubresourceRange.aspectMask = Info.AspectMask;
		Info.SubresourceRange.baseMipLevel = 0;
		Info.SubresourceRange.levelCount = 1;
		Info.SubresourceRange.layerCount = 1;
	}

	// Create an image barrier object
	VkImageMemoryBarrier ImageMemoryBarrier = { };
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	// Some default values
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	ImageMemoryBarrier.oldLayout = Info.OldImageLayout;
	ImageMemoryBarrier.newLayout = Info.NewImageLayout;
	ImageMemoryBarrier.image = Info.Image;
	ImageMemoryBarrier.subresourceRange = Info.SubresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (Info.OldImageLayout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			// Image layout is undefined (or does not matter)
			// Only valid as initial layout
			// No flags required, listed only for completeness
			ImageMemoryBarrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			// Image is preinitialized
			// Only valid as initial layout for linear images, preserves memory contents
			// Make sure host writes have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image is a color attachment
			// Make sure any writes to the color buffer have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image is a depth/stencil attachment
			// Make sure any writes to the depth/stencil buffer have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image is a transfer source 
			// Make sure any reads from the image have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image is a transfer destination
			// Make sure any writes to the image have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image is read by a shader
			// Make sure any shader reads from the image have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (Info.NewImageLayout)
	{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			// Image will be used as a transfer destination
			// Make sure any writes to the image have been finished
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			// Image will be used as a transfer source
			// Make sure any reads from and writes to the image have been finished
			ImageMemoryBarrier.srcAccessMask = ImageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			// Image will be used as a color attachment
			// Make sure any writes to the color buffer have been finished
			ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			// Image layout will be used as a depth/stencil attachment
			// Make sure any writes to depth/stencil buffer have been finished
			ImageMemoryBarrier.dstAccessMask = ImageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			// Image will be read in a shader (sampler, input attachment)
			// Make sure any writes to the image have been finished
			if (ImageMemoryBarrier.srcAccessMask == 0)
			{
				ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
	}

	// Put barrier on top
	VkPipelineStageFlags SourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags DestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		Info.CommandBuffer, 
		SourceStageFlags, 
		DestinationStageFlags, 
		0, 
		0, nullptr,
		0, nullptr,
		1, &ImageMemoryBarrier);
}
