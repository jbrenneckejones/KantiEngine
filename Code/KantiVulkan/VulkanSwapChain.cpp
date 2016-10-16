#include "VulkanSwapChain.h"
#include "../KantiManagers/KantiDebugManager.h"

void VulkanSwapChain::InitializeSurface(renderer_platform& Platform)
{
	VkResult Error;

	// Create the os-specific surface
	// TODO(Julian): Do checks for other surfaces
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hinstance = (HINSTANCE)Platform.Instance;
	SurfaceCreateInfo.hwnd = (HWND)Platform.Window;
	Error = vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, nullptr, &Surface);

	// Get available queue family properties
	uint32 QueueCount;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, NULL);
	Assert(QueueCount >= 1);

	KList<VkQueueFamilyProperties> QueueProperties(QueueCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueCount, QueueProperties.Data());

	// Iterate over each queue to learn whether it supports presenting:
	// Find a queue with present support
	// Will be used to present the swap chain images to the windowing system
	KList<VkBool32> SupportsPresent(QueueCount);
	for (uint32 Index = 0; Index < QueueCount; Index++) 
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, Index, Surface, &SupportsPresent[Index]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint32 GraphicsQueueNodeIndex = UINT32_MAX;
	uint32 PresentQueueNodeIndex = UINT32_MAX;
	for (uint32 Index = 0; Index < QueueCount; Index++) 
	{
		if ((QueueProperties[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
		{
			if (GraphicsQueueNodeIndex == UINT32_MAX) 
			{
				GraphicsQueueNodeIndex = Index;
			}

			if (SupportsPresent[Index] == VK_TRUE) 
			{
				GraphicsQueueNodeIndex = Index;
				PresentQueueNodeIndex = Index;
				break;
			}
		}
	}
	if (PresentQueueNodeIndex == UINT32_MAX) 
	{	
		// If there's no queue that supports both present and graphics
		// try to find a separate present queue
		for (uint32 Index = 0; Index < QueueCount; ++Index) 
		{
			if (SupportsPresent[Index] == VK_TRUE) 
			{
				PresentQueueNodeIndex = Index;
				break;
			}
		}
	}

	// Exit if either a graphics or a presenting queue hasn't been found
	if (GraphicsQueueNodeIndex == UINT32_MAX || PresentQueueNodeIndex == UINT32_MAX) 
	{
		K_ERROR("Could not find a graphics and/or presenting queue! Fatal error");
	}

	// todo : Add support for separate graphics and presenting queue
	if (GraphicsQueueNodeIndex != PresentQueueNodeIndex) 
	{
		K_ERROR("Separate graphics and presenting queues are not supported yet! Fatal error");
	}

	QueueNodeIndex = GraphicsQueueNodeIndex;

	// Get list of supported surface formats
	uint32 FormatCount;
	Error = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, NULL);
	Assert(!Error);
	Assert(FormatCount > 0);

	KList<VkSurfaceFormatKHR> SurfaceFormats(FormatCount);
	Error = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, SurfaceFormats.Data());
	Assert(!Error);

	// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
	// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
	if ((FormatCount == 1) && (SurfaceFormats[0].format == VK_FORMAT_UNDEFINED))
	{
		ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		// Always select the first available color format
		// If you need a specific format (e.g. SRGB) you'd need to
		// iterate over the list of available surface format and
		// check for it's presence
		ColorFormat = SurfaceFormats[0].format;
	}
	ColorSpace = SurfaceFormats[0].colorSpace;
}

void VulkanSwapChain::Connect(VkInstance VulkanInstance, VkPhysicalDevice VulkanPhysicalDevice, VkDevice VulkanDevice)
{
	this->Instance = VulkanInstance;
	this->PhysicalDevice = VulkanPhysicalDevice;
	this->Device = VulkanDevice;
}

void VulkanSwapChain::Create(uint32* Width, uint32* Height, bool32 UseVsync)
{
	VkResult Error;
	VkSwapchainKHR OldSwapchain = SwapChain;

	// Get physical device surface properties and formats
	VkSurfaceCapabilitiesKHR SurfaceCapabilities;
	Error = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfaceCapabilities);
	Assert(!Error);

	// Get available present modes
	uint32 PresentModeCount;
	Error = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, NULL);
	Assert(!Error);
	Assert(PresentModeCount > 0);

	KList<VkPresentModeKHR> PresentModes(PresentModeCount);

	Error = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, PresentModes.Data());
	Assert(!Error);

	VkExtent2D SwapchainExtent = {};
	// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
	if (SurfaceCapabilities.currentExtent.width == (uint32)-1)
	{
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		SwapchainExtent.width = *Width;
		SwapchainExtent.height = *Height;
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
		SwapchainExtent = SurfaceCapabilities.currentExtent;
		*Width = SurfaceCapabilities.currentExtent.width;
		*Height = SurfaceCapabilities.currentExtent.height;
	}


	// Select a present mode for the swapchain

	// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
	// This mode waits for the vertical blank ("v-sync")
	VkPresentModeKHR SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	// If v-sync is not requested, try to find a mailbox mode
	// It's the lowest latency non-tearing present mode available
	if (!UseVsync)
	{
		for (uint32 Index = 0; Index < PresentModeCount; Index++)
		{
			if (PresentModes[Index] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				SwapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			if ((SwapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (PresentModes[Index] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				SwapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}
	}

	// Determine the number of images
	uint32 DesiredNumberOfSwapchainImages = SurfaceCapabilities.minImageCount + 1;
	if ((SurfaceCapabilities.maxImageCount > 0) && (DesiredNumberOfSwapchainImages > SurfaceCapabilities.maxImageCount))
	{
		DesiredNumberOfSwapchainImages = SurfaceCapabilities.maxImageCount;
	}

	// Find the transformation of the surface
	VkSurfaceTransformFlagsKHR SurfacePreTransform;
	if (SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		// We prefer a non-rotated transform
		SurfacePreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else 
	{
		SurfacePreTransform = SurfaceCapabilities.currentTransform;
	}

	VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
	SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapchainCreateInfo.pNext = NULL;
	SwapchainCreateInfo.surface = Surface;
	SwapchainCreateInfo.minImageCount = DesiredNumberOfSwapchainImages;
	SwapchainCreateInfo.imageFormat = ColorFormat;
	SwapchainCreateInfo.imageColorSpace = ColorSpace;
	SwapchainCreateInfo.imageExtent = { SwapchainExtent.width, SwapchainExtent.height };
	SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	SwapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)SurfacePreTransform;
	SwapchainCreateInfo.imageArrayLayers = 1;
	SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapchainCreateInfo.queueFamilyIndexCount = 0;
	SwapchainCreateInfo.pQueueFamilyIndices = NULL;
	SwapchainCreateInfo.presentMode = SwapchainPresentMode;
	SwapchainCreateInfo.oldSwapchain = OldSwapchain;
	// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
	SwapchainCreateInfo.clipped = VK_TRUE;
	SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	Error = vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &SwapChain);
	Assert(!Error);

	// If an existing sawp chain is re-created, destroy the old swap chain
	// This also cleans up all the presentable images
	if (OldSwapchain != VK_NULL_HANDLE) 
	{ 
		for (uint32 Index = 0; Index < ImageCount; Index++)
		{
			vkDestroyImageView(Device, Buffers[Index].View, nullptr);
		}
		vkDestroySwapchainKHR(Device, OldSwapchain, nullptr);
	}

	Error = vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, NULL);
	Assert(!Error);

	// Get the swap chain images
	Images.Resize(ImageCount);
	Error = vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, Images.Data());
	Assert(!Error);

	// Get the swap chain buffers containing the image and imageview
	Buffers.Resize(ImageCount);
	for (uint32 Index = 0; Index < ImageCount; Index++)
	{
		VkImageViewCreateInfo ColorAttachmentView = {};
		ColorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ColorAttachmentView.pNext = NULL;
		ColorAttachmentView.format = ColorFormat;
		ColorAttachmentView.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};
		ColorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ColorAttachmentView.subresourceRange.baseMipLevel = 0;
		ColorAttachmentView.subresourceRange.levelCount = 1;
		ColorAttachmentView.subresourceRange.baseArrayLayer = 0;
		ColorAttachmentView.subresourceRange.layerCount = 1;
		ColorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ColorAttachmentView.flags = 0;

		Buffers[Index].Image = Images[Index];

		ColorAttachmentView.image = Buffers[Index].Image;

		Error = vkCreateImageView(Device, &ColorAttachmentView, nullptr, &Buffers[Index].View);
		Assert(!Error);
	}
}

VkResult VulkanSwapChain::AcquireNextImage(VkSemaphore PresentCompleteSemaphore, uint32* ImageIndex)
{
	// By setting timeout to UINT64_MAX we will always wait until the next image has been acquired or an actual error is thrown
	// With that we don't have to handle VK_NOT_READY
	return vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX, PresentCompleteSemaphore, (VkFence)nullptr, ImageIndex);
}

VkResult VulkanSwapChain::QueuePresent(VkQueue Queue, uint32 ImageIndex, VkSemaphore WaitSemaphore)
{
	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &SwapChain;
	PresentInfo.pImageIndices = &ImageIndex;
	// Check if a wait semaphore has been specified to wait for before presenting the image
	if (WaitSemaphore != VK_NULL_HANDLE)
	{
		PresentInfo.pWaitSemaphores = &WaitSemaphore;
		PresentInfo.waitSemaphoreCount = 1;
	}
	return vkQueuePresentKHR(Queue, &PresentInfo);
}

void VulkanSwapChain::Cleanup()
{
	for (uint32 Index = 0; Index < ImageCount; Index++)
	{
		vkDestroyImageView(Device, Buffers[Index].View, nullptr);
	}
	vkDestroySwapchainKHR(Device, SwapChain, nullptr);
	vkDestroySurfaceKHR(Instance, Surface, nullptr);
}
