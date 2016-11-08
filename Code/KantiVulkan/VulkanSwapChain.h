#ifndef VULKAN_SWAPCHAIN

class VulkanSwapChain
{
	typedef struct swap_chain_buffers {
		VkImage Image;
		VkImageView View;
	} swap_chain_buffer;

	private: 

	VkInstance Instance;
	VkDevice Device;
	VkPhysicalDevice PhysicalDevice;
	VkSurfaceKHR Surface;

	public:
	VkFormat ColorFormat;
	VkColorSpaceKHR ColorSpace;

	VkSwapchainKHR SwapChain = VK_NULL_HANDLE;

	uint32 ImageCount;
	KList<VkImage> Images;
	KList<swap_chain_buffer> Buffers;

	// Index of the deteced graphics and presenting device queue
	uint32 QueueNodeIndex = UINT32_MAX;

	// Creates an os specific surface
	/**
	* Create the surface object, an abstraction for the native platform window
	*
	* @pre Windows
	* @param platformHandle HINSTANCE of the window to create the surface for
	* @param platformWindow HWND of the window to create the surface for
	*
	* @pre Android 
	* @param window A native platform window
	*
	* @pre Linux (XCB)
	* @param connection xcb connection to the X Server
	* @param window The xcb window to create the surface for
	* @note Targets other than XCB ar not yet supported
	*/
	void InitializeSurface(renderer_platform& Platform);

	/**
	* Set instance, physical and logical device to use for the swpachain and get all required function pointers
	* 
	* @param instance Vulkan instance to use
	* @param physicalDevice Physical device used to query properties and formats relevant to the swapchain
	* @param device Logical representation of the device to create the swapchain for
	*
	*/
	void Connect(VkInstance VulkanInstance, VkPhysicalDevice VulkanPhysicalDevice, VkDevice VulkanDevice);

	/** 
	* Create the swapchain and get it's images with given width and height
	* 
	* @param width Pointer to the width of the swapchain (may be adjusted to fit the requirements of the swapchain)
	* @param height Pointer to the height of the swapchain (may be adjusted to fit the requirements of the swapchain)
	* @param vsync (Optional) Can be used to force vsync'd rendering (by using VK_PRESENT_MODE_FIFO_KHR as presentation mode)
	*/
	void Create(uint32* Width, uint32* Height, bool32 UseVsync = false);

	/** 
	* Acquires the next image in the swap chain
	*
	* @param presentCompleteSemaphore (Optional) Semaphore that is signaled when the image is ready for use
	* @param imageIndex Pointer to the image index that will be increased if the next image could be acquired
	*
	* @note The function will always wait until the next image has been acquired by setting timeout to UINT64_MAX
	*
	* @return VkResult of the image acquisition
	*/
	VkResult AcquireNextImage(VkSemaphore PresentCompleteSemaphore, uint32* ImageIndex);

	/**
	* Queue an image for presentation
	*
	* @param queue Presentation queue for presenting the image
	* @param imageIndex Index of the swapchain image to queue for presentation
	* @param waitSemaphore (Optional) Semaphore that is waited on before the image is presented (only used if != VK_NULL_HANDLE)
	*
	* @return VkResult of the queue presentation
	*/
	VkResult QueuePresent(VkQueue Queue, uint32 ImageIndex, VkSemaphore WaitSemaphore = VK_NULL_HANDLE);


	/**
	* Destroy and free Vulkan resources used for the swapchain
	*/
	void Cleanup();

};

#define VULKAN_SWAPCHAIN
#endif