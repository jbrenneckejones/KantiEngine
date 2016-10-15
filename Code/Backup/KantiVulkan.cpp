#include "KantiVulkan.h"
#include "KantiAllocator.h"
#include "KantiIntrinsics.h"
#include "KantiMath.h"

// TODO(Julian): Remove this eventually
#include <vector>
#include "D:/Tools/SDK/VulkanSDK/1.0.26.0/glslang/SPIRV/GlslangToSpv.h"

VkResult VulkanInitializeGlobalExtensionProperties(layer_properties& LayerProperties)
{
	VkExtensionProperties* Extensions;
	uint32 ExtensionCount;
	VkResult Result;
	char* LayerName = LayerProperties.properties.layerName;

	do
	{
		Result = vkEnumerateInstanceExtensionProperties(
			LayerName, &ExtensionCount, NULL);
		if(Result)
		{
			return Result;
		}

		if(ExtensionCount == 0)
		{
			return VK_SUCCESS;
		}

		// LayerProperties.extensions = (VkExtensionProperties *)MemAlloc(sizeof(VkExtensionProperties) + ExtensionCount, 8);
		LayerProperties.extensions.resize(ExtensionCount);
		Extensions = LayerProperties.extensions.data();
		Result = vkEnumerateInstanceExtensionProperties(
			LayerName, &ExtensionCount, Extensions);
	} while(Result == VK_INCOMPLETE);

	return Result;
}

VkResult VulkanInitializeGlobalLayerProperties(renderer_data& Info)
{
	uint32 InstanceLayerCount;
	VkLayerProperties* LayerProperties = NULL;
	VkResult Result;

	/*
	* It's possible, though very rare, that the number of
	* instance layers could change. For example, installing something
	* could include new layers that the loader would pick up
	* between the initial query for the count and the
	* request for VkLayerProperties. The loader indicates that
	* by returning a VK_INCOMPLETE status and will update the
	* the count parameter.
	* The count parameter will be updated with the number of
	* entries loaded into the data pointer - in case the number
	* of layers went down or is smaller than the size given.
	*/
	do
	{
		Result = vkEnumerateInstanceLayerProperties(&InstanceLayerCount, NULL);

		if(Result)
		{
			return Result;
		}

		if(InstanceLayerCount == 0)
		{
			return VK_SUCCESS;
		}

		LayerProperties = (VkLayerProperties *)MemAlloc(InstanceLayerCount * sizeof(VkLayerProperties), 8);

		Result =
			vkEnumerateInstanceLayerProperties(&InstanceLayerCount, LayerProperties);
	} while(Result == VK_INCOMPLETE);

	/*
	* Now gather the extension list for each instance layer.
	*/

	// Info.instance_layer_properties = (layer_properties *)MemAlloc(sizeof(layer_properties) + InstanceLayerCount, 8);
	Info.instance_layer_properties.resize(0);
	for(uint32 Index = 0; Index < InstanceLayerCount; ++Index)
	{
		layer_properties FoundProperties;
		FoundProperties.properties = LayerProperties[Index];
		Result = VulkanInitializeGlobalExtensionProperties(FoundProperties);

		if(Result)
		{
			return Result;
		}

		// Info.instance_layer_properties[Index] = FoundProperties;
		Info.instance_layer_properties.push_back(FoundProperties);
	}

	MemDealloc(LayerProperties);

	return Result;
}

void VulkanInitializeInstanceExtensionNames(renderer_data& Info)
{
	// int32 Count = 2;

	// Info.instance_extension_names = (k_string *)MemAlloc(sizeof(k_string) * Count, 8);

	// Info.instance_extension_names[0] = StringI(VK_KHR_SURFACE_EXTENSION_NAME, sizeof(VK_KHR_SURFACE_EXTENSION_NAME));
	// Info.instance_extension_names[1] = StringI(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, sizeof(VK_KHR_WIN32_SURFACE_EXTENSION_NAME));

	Info.instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	Info.instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
}

void VulkanInitializeDeviceExtensionNames(renderer_data& Info)
{
	// const int32 Count = 1;

	// Info.device_extension_names = (k_string *)MemAlloc(sizeof(k_string) * Count, 8);

	// Info.device_extension_names[0] = StringI(VK_KHR_SWAPCHAIN_EXTENSION_NAME, sizeof(VK_KHR_SWAPCHAIN_EXTENSION_NAME));

	Info.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void VulkanInitializeInstance(renderer_data& Info)
{
	/*
	typedef struct VkInstanceCreateInfo {
	VkStructureType             sType;					// The sType field indicates the type of the structure. In this case,
	// you set it to VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, since it is an
	// VkInstanceCreateInfo structure. This may seem redundant since only a
	// structure of this type can be passed as the first argument of
	// vkCreateInstance(). But it has some value for the following reasons:
	//
	// A driver, validation layer, or other consumer of the structure can perform
	// a simple validity check and perhaps fail the request if the sType is not as expected.
	//
	// Structures can be passed via type-less (void) pointers to consumers via interfaces that
	// are not fully defined to the point where the interface arguments are typed.
	// For example, if a driver supports an extension to instance creation, it could look
	// at such a structure passed via the type-less pNext pointer (discussed next).
	// In this case, the sType member would be set to a value that the extension recognizes.
	//
	// Since this member is always the first member in the structure, the consumer can easily
	// determine the type of the structure and decide how to handle it.

	const void*                 pNext;					// You set pNext to NULL more often than not. This void pointer is sometimes used to
	// pass extension-specific information in a typed structure where the sType member is
	// set to an extension-defined value. As discussed above, extensions can analyze any
	// structures that are passed in along this chain of pNext pointers to find structures
	// that they recognize.

	VkInstanceCreateFlags       flags;					// There are currently no flags defined, so set this to zero.

	const VkApplicationInfo*    pApplicationInfo;		// This is a pointer to another structure that you also need to fill out.
	// You will come back to this one in a moment.

	uint32_t                    enabledLayerCount;		// The samples in this tutorial do not use layers, so these members are cleared.
	const char* const*          ppEnabledLayerNames;

	uint32_t                    enabledExtensionCount;	// At this point, the samples in this tutorial do not use extensions.
	// Later on, another sample will show the usage of extensions.
	const char* const*          ppEnabledExtensionNames;
	}					   VkInstanceCreateInfo;
	*/

	// initialize the VkApplicationInfo structure
	VkApplicationInfo ApplicationInfo = {};
	ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	ApplicationInfo.pNext = NULL;
	ApplicationInfo.pApplicationName = "KantiEngine";
	ApplicationInfo.applicationVersion = 1;
	ApplicationInfo.pEngineName = "KantiEngine";
	ApplicationInfo.engineVersion = 1;
	ApplicationInfo.apiVersion = VK_API_VERSION_1_0;

	/*
	typedef struct VkApplicationInfo {
	VkStructureType    sType;			  // These have the same meanings as those in the vkInstanceCreateInfo structure.
	const void*        pNext;
	// These are free-form fields that the application may provide if desired.
	// Some implementations of tools, loaders, layers, or drivers may use these
	// fields to provide information while debugging or collecting information for reports,
	// etc. It may even be possible for drivers to change their behaviors depending on the
	// application that is running.
	const char*        pApplicationName;
	uint32_t           applicationVersion;
	const char*        pEngineName;
	uint32_t           engineVersion;
	// This field communicates the major, minor, and patch levels of the Vulkan API header used
	// to compile the application. If you are using Vulkan 1.0, major should be 1 and minor should be 0.
	// Using the VK_API_VERSION_1_0 macro from vulkan.h accomplishes this, with a patch level of 0.
	// Differences in the patch level should not affect the full compatibility between versions that
	// differ only in the patch level. Generally, you should set this field to VK_API_VERSION_1_0 unless
	// you have a good reason to do otherwise.
	uint32_t           apiVersion;
	}					   VkApplicationInfo;
	*/

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo InstanceInfo = {};
	InstanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceInfo.pNext = NULL;
	InstanceInfo.flags = 0;
	InstanceInfo.pApplicationInfo = &ApplicationInfo;

	// TODO(Julian): Do this a better way
	/*
	char** Extensions = NULL;
	int32 ExtensionCount = 0;
	if(Info.instance_extension_names)
	{
		for(k_string* String = Info.instance_extension_names; *String->Buffer; ++String)
		{
			ExtensionCount++;
		}

		Extensions = (char **)MemAlloc(sizeof(char) * ExtensionCount, 8);

		for(int32 Index = 0; Index < ExtensionCount; ++Index)
		{
			CharArrayCopyTo(&Extensions[Index], Info.instance_extension_names[Index].Buffer);
		}
	}
	*/

	InstanceInfo.enabledExtensionCount = (uint32)Info.instance_extension_names.size();
	InstanceInfo.ppEnabledExtensionNames = Info.instance_extension_names.data();

	/*
	char** Layers = NULL;
	int32 LayerCount = 0;
	if(Info.instance_layer_names)
	{
		for(k_string* String = Info.instance_layer_names; *String->Buffer; ++String)
		{
			LayerCount++;
		}

		Layers = (char **)MemAlloc(sizeof(char) * LayerCount, 8);

		for(int32 Index = 0; Index < LayerCount; ++Index)
		{
			CharArrayCopyTo(&Layers[Index], Info.instance_layer_names[Index].Buffer);
		}
	}
	*/

	InstanceInfo.enabledLayerCount = (uint32)Info.instance_layer_names.size();
	InstanceInfo.ppEnabledLayerNames = Info.instance_layer_names.data();

	/*
	VkResult		   // Return status of function
	vkCreateInstance(
	const VkInstanceCreateInfo*                 pCreateInfo,	// This structure contains any additional information that is needed to create the instance.
	// Since this is an important item, you will take a closer look at it later.

	const VkAllocationCallbacks*                pAllocator,		// Functions that allocate host memory are equipped with an argument
	// that allows the application to perform its own host memory management.
	// Otherwise, the Vulkan implementation uses the default system memory management facilities.
	// An application might want to manage its own host memory in order to log memory allocations, for example.

	VkInstance*                                 pInstance);		// This is simply a handle returned by this function if the instance creation was successful.
	// This is an opaque handle, so do not try to de-reference it.
	// Many Vulkan functions that create objects return handles for the objects they create in this manner.
	*/

	VkResult Result;

	Result = vkCreateInstance(&InstanceInfo, NULL, &Info.inst);
	if(Result == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		CheckVKResult(Result, "cannot find a compatible Vulkan ICD");
		exit(-1);
	}
	else if(Result)
	{
		CheckVKResult(Result, "UKNOWN");
		exit(-1);
	}
}

void VulkanInitializeEnumerateDevice(renderer_data& Info)
{
	uint32 GPUCount = 1;
	VkResult U_ASSERT_ONLY Result =
		vkEnumeratePhysicalDevices(Info.inst, &GPUCount, NULL);
	Assert(GPUCount);
	// Info.gpus = (VkPhysicalDevice *)MemAlloc(GPUCount, 8);
	Info.gpus.resize(GPUCount);
	Result = vkEnumeratePhysicalDevices(Info.inst, &GPUCount, Info.gpus.data());
	Assert(!Result && GPUCount >= 1);

	vkGetPhysicalDeviceQueueFamilyProperties(Info.gpus[0],
		&Info.queue_family_count, NULL);
	Assert(Info.queue_family_count >= 1);

	// Info.queue_props = (VkQueueFamilyProperties *)MemAlloc(sizeof(VkQueueFamilyProperties) * Info.queue_family_count, 8);
	Info.queue_props.resize(Info.queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		Info.gpus[0], &Info.queue_family_count, Info.queue_props.data());
	Assert(Info.queue_family_count >= 1);

	/* This is as good a place as any to do this */
	vkGetPhysicalDeviceMemoryProperties(Info.gpus[0], &Info.memory_properties);
	vkGetPhysicalDeviceProperties(Info.gpus[0], &Info.gpu_props);
}

void VulkanInitializeWindowSize(renderer_data& Info, int32 Width, int32 Height)
{
	Info.width = Width;
	Info.height = Height;
}

void VulkanInitializeConnection(renderer_data& Info, void* Handle, void* Window)
{
	Info.connection = (HINSTANCE)Handle;
	Info.window = (HWND)Window;
}

void VulkanInitializeWindow(renderer_data& Info)
{
}

void VulkanInitializeDevice(renderer_data& Info)
{
	VkResult Result;
	VkDeviceQueueCreateInfo QueueInfo = {};

	float QueuePriorities[1] = {0.0};
	QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueueInfo.pNext = NULL;
	QueueInfo.queueCount = 1;
	QueueInfo.pQueuePriorities = QueuePriorities;
	QueueInfo.queueFamilyIndex = Info.graphics_queue_family_index;

	VkDeviceCreateInfo DeviceInfo = {};
	DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceInfo.pNext = NULL;
	DeviceInfo.queueCreateInfoCount = 1;
	DeviceInfo.pQueueCreateInfos = &QueueInfo;
	DeviceInfo.enabledExtensionCount = (uint32)Info.device_extension_names.size();
	DeviceInfo.ppEnabledExtensionNames =
		DeviceInfo.enabledExtensionCount ? Info.device_extension_names.data()
		: NULL;
	DeviceInfo.pEnabledFeatures = NULL;

	Result = vkCreateDevice(Info.gpus[0], &DeviceInfo, NULL, &Info.device);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeDevice2(renderer_data& Info)
{
	VkDeviceQueueCreateInfo QueueInfo = {};

	vkGetPhysicalDeviceQueueFamilyProperties(Info.gpus[0],
		&Info.queue_family_count, NULL);

	Assert(Info.queue_family_count >= 1);

	// TODO(Julian): May need to call this with sizeof item too besides just how much
	// Info.queue_props = (VkQueueFamilyProperties *)MemAlloc(Info.queue_family_count, 8);
	Info.queue_props.resize(Info.queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(
		Info.gpus[0], &Info.queue_family_count, Info.queue_props.data());
	Assert(Info.queue_family_count >= 1);

	bool FoundDevice = false;
	for(unsigned int Index = 0; Index < Info.queue_family_count; ++Index)
	{
		if(Info.queue_props[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			QueueInfo.queueFamilyIndex = Index;
			FoundDevice = true;
			break;
		}
	}
	Assert(FoundDevice);
	Assert(Info.queue_family_count >= 1);

	float QueuePriorities[1] = { 0.0 };
	QueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueueInfo.pNext = NULL;
	QueueInfo.queueCount = 1;
	QueueInfo.pQueuePriorities = QueuePriorities;

	VkDeviceCreateInfo DeviceInfo = {};
	DeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceInfo.pNext = NULL;
	DeviceInfo.queueCreateInfoCount = 1;
	DeviceInfo.pQueueCreateInfos = &QueueInfo;
	DeviceInfo.enabledExtensionCount = 0;
	DeviceInfo.ppEnabledExtensionNames = NULL;
	DeviceInfo.enabledLayerCount = 0;
	DeviceInfo.ppEnabledLayerNames = NULL;
	DeviceInfo.pEnabledFeatures = NULL;

	VkResult U_ASSERT_ONLY Result =
		vkCreateDevice(Info.gpus[0], &DeviceInfo, NULL, &Info.device);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeCommandBuffers(renderer_data& Info)
{
	VkResult Result;

	/* Create the command buffer from the command pool */
	VkCommandBufferAllocateInfo Command = {};
	Command.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	Command.pNext = NULL;
	Command.commandPool = Info.cmd_pool;
	Command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	Command.commandBufferCount = 1;

	Result = vkAllocateCommandBuffers(Info.device, &Command, &Info.cmd);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeCommandPool(renderer_data& Info) 
{
	/* DEPENDS on init_swapchain_extension() */
	VkResult U_ASSERT_ONLY Result;

	VkCommandPoolCreateInfo CommandPoolInfo = {};
	CommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolInfo.pNext = NULL;
	CommandPoolInfo.queueFamilyIndex = Info.graphics_queue_family_index;
	CommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	Result =
		vkCreateCommandPool(Info.device, &CommandPoolInfo, NULL, &Info.cmd_pool);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeSwapChainExtensions(renderer_data& Info)
{
	/* DEPENDS on init_connection() and init_window() */

	VkResult U_ASSERT_ONLY Result;

	// Construct the surface description:
	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.pNext = NULL;
	SurfaceCreateInfo.hinstance = Info.connection;
	SurfaceCreateInfo.hwnd = Info.window;
	Result = vkCreateWin32SurfaceKHR(Info.inst, &SurfaceCreateInfo,
		NULL, &Info.surface);

	Assert(Result == VK_SUCCESS);

	// Iterate over each queue to learn whether it supports presenting:
	VkBool32* SupportsPresent =
		(VkBool32 *)MemAlloc(Info.queue_family_count * sizeof(VkBool32));
	for (uint32 Index = 0; Index < Info.queue_family_count; Index++) 
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(Info.gpus[0], Index, Info.surface,
			&SupportsPresent[Index]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	Info.graphics_queue_family_index = UINT32_MAX;
	Info.present_queue_family_index = UINT32_MAX;
	for (uint32 Index = 0; Index < Info.queue_family_count; ++Index) 
	{
		if ((Info.queue_props[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
		{
			if(Info.graphics_queue_family_index == UINT32_MAX)
			{
				Info.graphics_queue_family_index = Index;
			}

			if (SupportsPresent[Index] == VK_TRUE) 
			{
				Info.graphics_queue_family_index = Index;
				Info.present_queue_family_index = Index;
				break;
			}
		}
	}

	if (Info.present_queue_family_index == UINT32_MAX) 
	{
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for(memory_index Index = 0; Index < Info.queue_family_count; ++Index)
		{
			if(SupportsPresent[Index] == VK_TRUE)
			{
				Info.present_queue_family_index = (uint32)Index;
				break;
			}
		}
	}
	MemDealloc(SupportsPresent);

	// Generate error if could not find queues that support graphics
	// and present
	if (Info.graphics_queue_family_index == UINT32_MAX ||
		Info.present_queue_family_index == UINT32_MAX) 
	{
		OutputDebugStringA("Could not find a queues for both graphics and present");
		exit(-1);
	}

	// Get the list of VkFormats that are supported:
	uint32 FormatCount;
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(Info.gpus[0], Info.surface,
		&FormatCount, NULL);
	Assert(Result == VK_SUCCESS);
	VkSurfaceFormatKHR *SurfaceFormats =
		(VkSurfaceFormatKHR *)MemAlloc(FormatCount * sizeof(VkSurfaceFormatKHR), 8);
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(Info.gpus[0], Info.surface,
		&FormatCount, SurfaceFormats);
	Assert(Result == VK_SUCCESS);
	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (FormatCount == 1 && SurfaceFormats[0].format == VK_FORMAT_UNDEFINED) 
	{
		Info.format = VK_FORMAT_B8G8R8A8_UNORM;
	} 
	else 
	{
		Assert(FormatCount >= 1);
		Info.format = SurfaceFormats[0].format;
	}
	MemDealloc(SurfaceFormats);
}

void VulkanInitializeSwapChain(renderer_data& Info, VkImageUsageFlags UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
	VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
{
	/*
	* Set up swapchain:
	* - Get supported uses for all queues
	* - Try to find a queue that supports both graphics and present
	* - If no queue supports both, find a present queue and make sure we have a
	*   graphics queue
	* - Get a list of supported formats and use the first one
	* - Get surface properties and present modes and use them to create a swap
	*   chain
	* - Create swap chain buffers
	* - For each buffer, create a color attachment view and set its layout to
	*   color attachment
	*/

	// Construct the surface description:

	VkResult Result;

	// Potentially old
	/*
			VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo = {};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.pNext = NULL;
	SurfaceCreateInfo.hinstance = Info.connection;
	SurfaceCreateInfo.hwnd = Info.window;
	Result = vkCreateWin32SurfaceKHR(Info.inst, &SurfaceCreateInfo,
		NULL, &Info.surface);

	Assert(Result == VK_SUCCESS);

	// Iterate over each queue to learn whether it supports presenting:
	VkBool32* SupportsPresent =
		(VkBool32 *)MemAlloc(Info.queue_family_count * sizeof(VkBool32), 8);
	for(uint32 Index = 0; Index < Info.queue_family_count; Index++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(Info.gpus[0], Index, Info.surface,
			&SupportsPresent[Index]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	Info.graphics_queue_family_index = UINT32_MAX;
	Info.present_queue_family_index = UINT32_MAX;

	for(uint32 Index = 0; Index < Info.queue_family_count; ++Index)
	{
		if((Info.queue_props[Index].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
		{
			if(Info.graphics_queue_family_index == UINT32_MAX)
			{
				Info.graphics_queue_family_index = Index;
			}

			if(SupportsPresent[Index] == VK_TRUE)
			{
				Info.graphics_queue_family_index = Index;
				Info.present_queue_family_index = Index;
				break;
			}
		}
	}

	if(Info.present_queue_family_index == UINT32_MAX)
	{
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for(memory_index Index = 0; Index < Info.queue_family_count; ++Index)
		{
			if(SupportsPresent[Index] == VK_TRUE)
			{
				Info.present_queue_family_index = (uint32)Index;
				break;
			}
		}
	}
	MemDealloc(SupportsPresent);

	// Generate error if could not find queues that support graphics
	// and present
	if(Info.graphics_queue_family_index == UINT32_MAX ||
		Info.present_queue_family_index == UINT32_MAX)
	{
		OutputDebugStringA("Could not find a queues for graphics and present\n");
		exit(-1);
	}

	// init_device(Info);
	Assert(Info.device);

	// Get the list of VkFormats that are supported:
	uint32 FormatCount;
	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(Info.gpus[0], Info.surface,
		&FormatCount, NULL);
	Assert(Result == VK_SUCCESS);

	VkSurfaceFormatKHR* SurfaceFormats =
		(VkSurfaceFormatKHR *)MemAlloc(FormatCount * sizeof(VkSurfaceFormatKHR), 8);

	Result = vkGetPhysicalDeviceSurfaceFormatsKHR(Info.gpus[0], Info.surface,
		&FormatCount, SurfaceFormats);
	Assert(Result == VK_SUCCESS);

	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if(FormatCount == 1 && SurfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		Info.format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		Assert(FormatCount >= 1);
		Info.format = SurfaceFormats[0].format;
	}
	MemDealloc(SurfaceFormats);
	*/

	VkSurfaceCapabilitiesKHR SurfaceCapabilities;

	Result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Info.gpus[0], Info.surface,
		&SurfaceCapabilities);
	Assert(Result == VK_SUCCESS);

	uint32 PresentModeCount;
	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(Info.gpus[0], Info.surface,
		&PresentModeCount, NULL);
	Assert(Result == VK_SUCCESS);

	VkPresentModeKHR* PresentModes =
		(VkPresentModeKHR *)MemAlloc(PresentModeCount * sizeof(VkPresentModeKHR), 8);

	Result = vkGetPhysicalDeviceSurfacePresentModesKHR(
		Info.gpus[0], Info.surface, &PresentModeCount, PresentModes);
	Assert(Result == VK_SUCCESS);

	VkExtent2D SwapchainExtent;
	// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
	if(SurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
	{
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		SwapchainExtent.width = Info.width;
		SwapchainExtent.height = Info.height;

		if(SwapchainExtent.width < SurfaceCapabilities.minImageExtent.width)
		{
			SwapchainExtent.width = SurfaceCapabilities.minImageExtent.width;
		}
		else if(SwapchainExtent.width >
			SurfaceCapabilities.maxImageExtent.width)
		{
			SwapchainExtent.width = SurfaceCapabilities.maxImageExtent.width;
		}

		if(SwapchainExtent.height < SurfaceCapabilities.minImageExtent.height)
		{
			SwapchainExtent.height = SurfaceCapabilities.minImageExtent.height;
		}
		else if(SwapchainExtent.height >
			SurfaceCapabilities.maxImageExtent.height)
		{
			SwapchainExtent.height = SurfaceCapabilities.maxImageExtent.height;
		}
	}
	else
	{
		// If the surface size is defined, the swap chain size must match
		SwapchainExtent = SurfaceCapabilities.currentExtent;
	}

	// If mailbox mode is available, use it, as is the lowest-latency non-
	// tearing mode.  If not, try IMMEDIATE which will usually be available,
	// and is fastest (though it tears).  If not, fall back to FIFO which is
	// always available.
	VkPresentModeKHR SwapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for(memory_index Index = 0; Index < PresentModeCount; Index++)
	{
		if(PresentModes[Index] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			SwapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if((SwapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
			(PresentModes[Index] == VK_PRESENT_MODE_IMMEDIATE_KHR))
		{
			SwapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of VkImage's to use in the swap chain.
	// We need to acquire only 1 presentable image at at time.
	// Asking for minImageCount images ensures that we can acquire
	// 1 presentable image as long as we present it before attempting
	// to acquire another.
	uint32 DesiredNumberOfSwapChainImages = SurfaceCapabilities.minImageCount;

	VkSurfaceTransformFlagBitsKHR PreTransform;
	if(SurfaceCapabilities.supportedTransforms &
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		PreTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		PreTransform = SurfaceCapabilities.currentTransform;
	}

	VkSwapchainCreateInfoKHR SwapChainCreationInfo = {};
	SwapChainCreationInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	SwapChainCreationInfo.pNext = NULL;
	SwapChainCreationInfo.surface = Info.surface;
	SwapChainCreationInfo.minImageCount = DesiredNumberOfSwapChainImages;
	SwapChainCreationInfo.imageFormat = Info.format;
	SwapChainCreationInfo.imageExtent.width = SwapchainExtent.width;
	SwapChainCreationInfo.imageExtent.height = SwapchainExtent.height;
	SwapChainCreationInfo.preTransform = PreTransform;
	SwapChainCreationInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	SwapChainCreationInfo.imageArrayLayers = 1;
	SwapChainCreationInfo.presentMode = SwapchainPresentMode;
	SwapChainCreationInfo.oldSwapchain = VK_NULL_HANDLE;
	SwapChainCreationInfo.clipped = true;
	SwapChainCreationInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	SwapChainCreationInfo.imageUsage = UsageFlags;
	SwapChainCreationInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	SwapChainCreationInfo.queueFamilyIndexCount = 0;
	SwapChainCreationInfo.pQueueFamilyIndices = NULL;

	uint32 QueueFamilyIndices[2] =
	{
		(uint32)Info.graphics_queue_family_index,
		(uint32)Info.present_queue_family_index
	};

	if(Info.graphics_queue_family_index != Info.present_queue_family_index)
	{
		// If the graphics and present queues are from different queue families,
		// we either have to explicitly transfer ownership of images between
		// the queues, or we have to create the swapchain with imageSharingMode
		// as VK_SHARING_MODE_CONCURRENT
		SwapChainCreationInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		SwapChainCreationInfo.queueFamilyIndexCount = 2;
		SwapChainCreationInfo.pQueueFamilyIndices = QueueFamilyIndices;
	}

	Result = vkCreateSwapchainKHR(Info.device, &SwapChainCreationInfo, NULL,
		&Info.swap_chain);
	Assert(Result == VK_SUCCESS);

	Result = vkGetSwapchainImagesKHR(Info.device, Info.swap_chain,
		&Info.swapchainImageCount, NULL);
	Assert(Result == VK_SUCCESS);

	VkImage* SwapchainImages =
		(VkImage *)MemAlloc(Info.swapchainImageCount * sizeof(VkImage), 8);
	Assert(SwapchainImages);

	Result = vkGetSwapchainImagesKHR(Info.device, Info.swap_chain,
		&Info.swapchainImageCount, SwapchainImages);
	Assert(Result == VK_SUCCESS);

	// Info.buffers = (swap_chain_buffer *)MemAlloc(Info.swapchainImageCount, 8);
	// Info.buffers.resize(Info.swapchainImageCount);

	for(uint32 Index = 0; Index < Info.swapchainImageCount; Index++)
	{
		swap_chain_buffer SwapChainBuffer;

		VkImageViewCreateInfo ColorImageView = {};
		ColorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ColorImageView.pNext = NULL;
		ColorImageView.flags = 0;
		ColorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ColorImageView.format = Info.format;
		ColorImageView.components.r = VK_COMPONENT_SWIZZLE_R;
		ColorImageView.components.g = VK_COMPONENT_SWIZZLE_G;
		ColorImageView.components.b = VK_COMPONENT_SWIZZLE_B;
		ColorImageView.components.a = VK_COMPONENT_SWIZZLE_A;
		ColorImageView.subresourceRange.aspectMask =
			VK_IMAGE_ASPECT_COLOR_BIT;
		ColorImageView.subresourceRange.baseMipLevel = 0;
		ColorImageView.subresourceRange.levelCount = 1;
		ColorImageView.subresourceRange.baseArrayLayer = 0;
		ColorImageView.subresourceRange.layerCount = 1;

		SwapChainBuffer.image = SwapchainImages[Index];

		ColorImageView.image = SwapChainBuffer.image;

		Result = vkCreateImageView(Info.device, &ColorImageView, NULL,
			&SwapChainBuffer.view);

		// Info.buffers[Index] = SwapChainBuffer;
		Info.buffers.push_back(SwapChainBuffer);

		Assert(Result == VK_SUCCESS);
	}

	MemDealloc(SwapchainImages);
	Info.current_buffer = 0;

	if (NULL != PresentModes) 
	{
		MemDealloc(PresentModes);
	}
}

bool VulkanCheckMemoryTypes(struct renderer_data &Info, uint32 TypeBits,
	VkFlags RequirementsMask,
	uint32 *TypeIndex) 
{
	// Search memtypes to find first index with those properties
	for (uint32 Index = 0; Index < Info.memory_properties.memoryTypeCount; Index++) 
	{
		if ((TypeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((Info.memory_properties.memoryTypes[Index].propertyFlags &
				RequirementsMask) == RequirementsMask) 
			{
				*TypeIndex = Index;
				return true;
			}
		}
		TypeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

void VulkanInitializeDepthBuffer(renderer_data& Info)
{
	/*
	* Make a depth buffer:
	* - Create an Image to be the depth buffer
	* - Find memory requirements
	* - Allocate and bind memory
	* - Set the image layout
	* - Create an attachment view
	*/

	VkResult U_ASSERT_ONLY Result;
	bool U_ASSERT_ONLY DidPass;

	VkImageCreateInfo ImageInfo = {};
	const VkFormat DepthFormat = VK_FORMAT_D16_UNORM;
	VkFormatProperties FormatProperties;
	vkGetPhysicalDeviceFormatProperties(Info.gpus[0], DepthFormat, &FormatProperties);

	if (FormatProperties.linearTilingFeatures &
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
	{
		ImageInfo.tiling = VK_IMAGE_TILING_LINEAR;
	} 
	else if (FormatProperties.optimalTilingFeatures &
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) 
	{
		ImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	} 
	else 
	{
		/* Try other depth formats? */
		OutputDebugStringA("VK_FORMAT_D16_UNORM Unsupported.\n");
		exit(-1);
	}

	ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	ImageInfo.pNext = NULL;
	ImageInfo.imageType = VK_IMAGE_TYPE_2D;
	ImageInfo.format = DepthFormat;
	ImageInfo.extent.width = Info.width;
	ImageInfo.extent.height = Info.height;
	ImageInfo.extent.depth = 1;
	ImageInfo.mipLevels = 1;
	ImageInfo.arrayLayers = 1;
	ImageInfo.samples = NUM_SAMPLES;
	ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	ImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	ImageInfo.queueFamilyIndexCount = 0;
	ImageInfo.pQueueFamilyIndices = NULL;
	ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	ImageInfo.flags = 0;

	VkMemoryAllocateInfo MemoryAllocInfo = {};
	MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocInfo.pNext = NULL;
	MemoryAllocInfo.allocationSize = 0;
	MemoryAllocInfo.memoryTypeIndex = 0;

	VkImageViewCreateInfo ImageViewInfo = {};
	ImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	ImageViewInfo.pNext = NULL;
	ImageViewInfo.image = VK_NULL_HANDLE;
	ImageViewInfo.format = DepthFormat;
	ImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	ImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	ImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	ImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	ImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	ImageViewInfo.subresourceRange.baseMipLevel = 0;
	ImageViewInfo.subresourceRange.levelCount = 1;
	ImageViewInfo.subresourceRange.baseArrayLayer = 0;
	ImageViewInfo.subresourceRange.layerCount = 1;
	ImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewInfo.flags = 0;

	VkMemoryRequirements MemoryRequirements;

	Info.depth.format = DepthFormat;

	/* Create image */
	Result = vkCreateImage(Info.device, &ImageInfo, NULL, &Info.depth.image);
	Assert(Result == VK_SUCCESS);

	vkGetImageMemoryRequirements(Info.device, Info.depth.image, &MemoryRequirements);

	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	/* Use the memory properties to determine the type of memory required */
	DidPass = VulkanCheckMemoryTypes(Info, MemoryRequirements.memoryTypeBits,
		0, /* No Requirements */
		&MemoryAllocInfo.memoryTypeIndex);
	Assert(DidPass);

	/* Allocate memory */
	Result = vkAllocateMemory(Info.device, &MemoryAllocInfo, NULL, &Info.depth.mem);
	Assert(Result == VK_SUCCESS);

	/* Bind memory */
	Result = vkBindImageMemory(Info.device, Info.depth.image, Info.depth.mem, 0);
	Assert(Result == VK_SUCCESS);

	/* Create image view */
	ImageViewInfo.image = Info.depth.image;
	Result = vkCreateImageView(Info.device, &ImageViewInfo, NULL, &Info.depth.view);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeUniformBuffer(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;
	bool U_ASSERT_ONLY DidPass;

	real32 FOV = ToRadians(45.0f);
	real32 AspectRatio = static_cast<float>(Info.height) / static_cast<float>(Info.width);
	if (Info.width > Info.height) 
	{
		FOV *= AspectRatio;
	}

	Info.Projection = PerspectiveMatrix(FOV, AspectRatio, 0.1f, 100.0f);
	Info.View = LookAt(
		Vector3(-5, 5, -10), // Camera is at (0,3,10), in World Space
		Vector3(0, 0, 0),  // and looks at the origin
		Vector3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	Info.Model = Matrix4x4(1.0f);
	// Vulkan clip space has inverted Y and half Z.
	Info.Clip = matrix4x4
	{ 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f,  0.0f, 0.5f, 0.0f,
		0.0f,  0.0f, 0.5f, 1.0f 
	};

	Info.MVP = Info.Clip * Info.Projection * Info.View * Info.Model;


	VkBufferCreateInfo BufferInfo = {};
	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.pNext = NULL;
	BufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	BufferInfo.size = sizeof(Info.MVP);
	BufferInfo.queueFamilyIndexCount = 0;
	BufferInfo.pQueueFamilyIndices = NULL;
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferInfo.flags = 0;
	Result = vkCreateBuffer(Info.device, &BufferInfo, NULL, &Info.uniform_data.buf);
	Assert(Result == VK_SUCCESS);

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(Info.device, Info.uniform_data.buf,
		&MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocInfo = {};
	MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocInfo.pNext = NULL;
	MemoryAllocInfo.memoryTypeIndex = 0;

	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	DidPass = VulkanCheckMemoryTypes(Info, MemoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&MemoryAllocInfo.memoryTypeIndex);
	Assert(DidPass && "No mappable, coherent memory");

	Result = vkAllocateMemory(Info.device, &MemoryAllocInfo, NULL,
		&(Info.uniform_data.mem));
	Assert(Result == VK_SUCCESS);

	uint8 *PointerData;
	Result = vkMapMemory(Info.device, Info.uniform_data.mem, 0, MemoryRequirements.size, 0,
		(void **)&PointerData);
	Assert(Result == VK_SUCCESS);

	MemCopy(PointerData, &Info.MVP, sizeof(Info.MVP));

	vkUnmapMemory(Info.device, Info.uniform_data.mem);

	Result = vkBindBufferMemory(Info.device, Info.uniform_data.buf,
		Info.uniform_data.mem, 0);
	Assert(Result == VK_SUCCESS);

	Info.uniform_data.buffer_info.buffer = Info.uniform_data.buf;
	Info.uniform_data.buffer_info.offset = 0;
	Info.uniform_data.buffer_info.range = sizeof(Info.MVP);
}

void VulkanInitializeDescriptorAndPipelineLayout(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	/* Start with just our uniform buffer that has our transformation matrices
	* (for the vertex shader). The fragment shader we intend to use needs no
	* external resources, so nothing else is necessary
	*/

	/* Note that when we start using textures, this is where our sampler will
	* need to be specified
	*/
	VkDescriptorSetLayoutBinding LayoutBinding = {};
	LayoutBinding.binding = 0;
	LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LayoutBinding.descriptorCount = 1;
	LayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	LayoutBinding.pImmutableSamplers = NULL;

	/* Next take layout bindings and use them to create a descriptor set layout
	*/
	VkDescriptorSetLayoutCreateInfo DescriptorLayout = {};
	DescriptorLayout.sType =
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	DescriptorLayout.pNext = NULL;
	DescriptorLayout.bindingCount = 1;
	DescriptorLayout.pBindings = &LayoutBinding;

	// Info.desc_layout = (VkDescriptorSetLayout *)MemAlloc(sizeof(VkDescriptorSetLayout) * NUM_DESCRIPTOR_SETS, 8);
	Info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
	Result = vkCreateDescriptorSetLayout(Info.device, &DescriptorLayout, NULL,
		Info.desc_layout.data());

	Assert(Result == VK_SUCCESS);

	/* Now use the descriptor layout to create a pipeline layout */
	VkPipelineLayoutCreateInfo PiplelineLayoutInfo = {};
	PiplelineLayoutInfo.sType =
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PiplelineLayoutInfo.pNext = NULL;
	PiplelineLayoutInfo.pushConstantRangeCount = 0;
	PiplelineLayoutInfo.pPushConstantRanges = NULL;
	PiplelineLayoutInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
	PiplelineLayoutInfo.pSetLayouts = Info.desc_layout.data();

	Result = vkCreatePipelineLayout(Info.device, &PiplelineLayoutInfo, NULL,
		&Info.pipeline_layout);

	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeDescriptorPool(renderer_data& Info, bool32 UseTexture) 
{
	/* DEPENDS on init_uniform_buffer() and
	* init_descriptor_and_pipeline_layouts() */

	VkResult U_ASSERT_ONLY Result;
	VkDescriptorPoolSize TypeCount[2];
	TypeCount[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	TypeCount[0].descriptorCount = 1;
	if (UseTexture) 
	{
		TypeCount[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		TypeCount[1].descriptorCount = 1;
	}

	VkDescriptorPoolCreateInfo DescriptorPool = {};
	DescriptorPool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	DescriptorPool.pNext = NULL;
	DescriptorPool.maxSets = 1;
	DescriptorPool.poolSizeCount = UseTexture ? 2 : 1;
	DescriptorPool.pPoolSizes = TypeCount;

	Result = vkCreateDescriptorPool(Info.device, &DescriptorPool, NULL,
		&Info.desc_pool);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeDescriptorSet(renderer_data& Info, bool32 UseTexture)
{
	/* DEPENDS on init_descriptor_pool() */

	VkResult U_ASSERT_ONLY Result;

	VkDescriptorSetAllocateInfo DescriptorAllocInfo[1];
	DescriptorAllocInfo[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	DescriptorAllocInfo[0].pNext = NULL;
	DescriptorAllocInfo[0].descriptorPool = Info.desc_pool;
	DescriptorAllocInfo[0].descriptorSetCount = NUM_DESCRIPTOR_SETS;
	DescriptorAllocInfo[0].pSetLayouts = Info.desc_layout.data();

	Info.desc_set = (VkDescriptorSet *)MemAlloc(sizeof(VkDescriptorSet) * NUM_DESCRIPTOR_SETS, 8);
	Result =
		vkAllocateDescriptorSets(Info.device, DescriptorAllocInfo, Info.desc_set);
	Assert(Result == VK_SUCCESS);

	VkWriteDescriptorSet DescriptorWrites[2];

	DescriptorWrites[0] = {};
	DescriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	DescriptorWrites[0].pNext = NULL;
	DescriptorWrites[0].dstSet = Info.desc_set[0];
	DescriptorWrites[0].descriptorCount = 1;
	DescriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	DescriptorWrites[0].pBufferInfo = &Info.uniform_data.buffer_info;
	DescriptorWrites[0].dstArrayElement = 0;
	DescriptorWrites[0].dstBinding = 0;

	if (UseTexture) 
	{
		DescriptorWrites[1] = {};
		DescriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		DescriptorWrites[1].dstSet = Info.desc_set[0];
		DescriptorWrites[1].dstBinding = 1;
		DescriptorWrites[1].descriptorCount = 1;
		DescriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		DescriptorWrites[1].pImageInfo = &Info.texture_data.image_info;
		DescriptorWrites[1].dstArrayElement = 0;
	}

	vkUpdateDescriptorSets(Info.device, UseTexture ? 2 : 1, DescriptorWrites, 0, NULL);
}

void VulkanSetImageLayout(struct renderer_data& Info, VkImage Image,
	VkImageAspectFlags AspectMask,
	VkImageLayout OldImageLayout,
	VkImageLayout NewImageLayout) 
{
	/* DEPENDS on info.cmd and info.queue initialized */

	Assert(Info.cmd != VK_NULL_HANDLE);
	Assert(Info.graphics_queue != VK_NULL_HANDLE);

	VkImageMemoryBarrier ImageMemoryBarrier = {};
	ImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	ImageMemoryBarrier.pNext = NULL;
	ImageMemoryBarrier.srcAccessMask = 0;
	ImageMemoryBarrier.dstAccessMask = 0;
	ImageMemoryBarrier.oldLayout = OldImageLayout;
	ImageMemoryBarrier.newLayout = NewImageLayout;
	ImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	ImageMemoryBarrier.image = Image;
	ImageMemoryBarrier.subresourceRange.aspectMask = AspectMask;
	ImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	ImageMemoryBarrier.subresourceRange.levelCount = 1;
	ImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	ImageMemoryBarrier.subresourceRange.layerCount = 1;

	if (OldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
	{
		ImageMemoryBarrier.srcAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (NewImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (NewImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) 
	{
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (OldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (OldImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED) 
	{
		ImageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	}

	if (NewImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		ImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	if (NewImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
	{
		ImageMemoryBarrier.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (NewImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	{
		ImageMemoryBarrier.dstAccessMask =
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	VkPipelineStageFlags SourcePipelineStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags DestinationPipelineStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(Info.cmd, SourcePipelineStages, DestinationPipelineStages, 0, 0, NULL, 0, NULL,
		1, &ImageMemoryBarrier);
}

void VulkanExecuteBeginCommandBuffer(renderer_data &Info) 
{
	/* DEPENDS on init_command_buffer() */
	VkResult U_ASSERT_ONLY Result;

	VkCommandBufferBeginInfo CommandBufferInfo = {};
	CommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CommandBufferInfo.pNext = NULL;
	CommandBufferInfo.flags = 0;
	CommandBufferInfo.pInheritanceInfo = NULL;

	Result = vkBeginCommandBuffer(Info.cmd, &CommandBufferInfo);
	Assert(Result == VK_SUCCESS);
}

void VulkanExecuteEndCommandBuffer(struct renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	Result = vkEndCommandBuffer(Info.cmd);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeDeviceQueue(renderer_data& Info) 
{
	/* DEPENDS on init_swapchain_extension() */

	vkGetDeviceQueue(Info.device, Info.graphics_queue_family_index, 0,
		&Info.graphics_queue);
	if (Info.graphics_queue_family_index == Info.present_queue_family_index) 
	{
		Info.present_queue = Info.graphics_queue;
	} 
	else {
		vkGetDeviceQueue(Info.device, Info.present_queue_family_index, 0,
			&Info.present_queue);
	}
}

void VulkanInitializeRenderPass(renderer_data& Info)

{
	VkResult U_ASSERT_ONLY Result;

	// A semaphore (or fence) is required in order to acquire a
	// swapchain image to prepare it for use in a render pass.
	// The semaphore is normally used to hold back the rendering
	// operation until the image is actually available.
	// But since this sample does not render, the semaphore
	// ends up being unused.
	VkSemaphore ImageAcquiredSemaphore;
	VkSemaphoreCreateInfo ImageAcquiredSemaphoreCreateInfo;
	ImageAcquiredSemaphoreCreateInfo.sType =
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	ImageAcquiredSemaphoreCreateInfo.pNext = NULL;
	ImageAcquiredSemaphoreCreateInfo.flags = 0;

	Result = vkCreateSemaphore(Info.device, &ImageAcquiredSemaphoreCreateInfo,
		NULL, &ImageAcquiredSemaphore);
	Assert(Result == VK_SUCCESS);

	// Acquire the swapchain image in order to set its layout
	Result = vkAcquireNextImageKHR(Info.device, Info.swap_chain, UINT64_MAX,
		ImageAcquiredSemaphore, VK_NULL_HANDLE,
		&Info.current_buffer);
	Assert(Result >= 0);

	// Set the layout for the color buffer, transitioning it from
	// undefined to an optimal color attachment to make it usable in
	// a render pass.
	// The depth buffer layout has already been set by init_depth_buffer().
	VulkanSetImageLayout(Info, Info.buffers[Info.current_buffer].image,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	// Stop recording the command buffer here since this sample will not
	// actually put the render pass in the command buffer (via vkCmdBeginRenderPass).
	// An actual application might leave the command buffer in recording mode
	// and insert a BeginRenderPass command after the image layout transition
	// memory barrier commands.
	// This sample simply creates and defines the render pass.
	VulkanExecuteEndCommandBuffer(Info);

	/* Need attachments for render target and depth buffer */
	VkAttachmentDescription Attachments[2];
	Attachments[0].format = Info.format;
	Attachments[0].samples = NUM_SAMPLES;
	Attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	Attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	Attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	Attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	Attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	Attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	Attachments[0].flags = 0;

	Attachments[1].format = Info.depth.format;
	Attachments[1].samples = NUM_SAMPLES;
	Attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	Attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	Attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	Attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	Attachments[1].initialLayout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	Attachments[1].finalLayout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	Attachments[1].flags = 0;

	VkAttachmentReference AttachmentColorReference = {};
	AttachmentColorReference.attachment = 0;
	AttachmentColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference AttachmentDepthReference = {};
	AttachmentDepthReference.attachment = 1;
	AttachmentDepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription SubpassDescription = {};
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.flags = 0;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = NULL;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &AttachmentColorReference;
	SubpassDescription.pResolveAttachments = NULL;
	SubpassDescription.pDepthStencilAttachment = &AttachmentDepthReference;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = NULL;

	VkRenderPassCreateInfo RenderPassInfo = {};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.attachmentCount = 2;
	RenderPassInfo.pAttachments = Attachments;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &SubpassDescription;
	RenderPassInfo.dependencyCount = 0;
	RenderPassInfo.pDependencies = NULL;

	Result = vkCreateRenderPass(Info.device, &RenderPassInfo, NULL, &Info.render_pass);
	Assert(Result == VK_SUCCESS);
}

void VulkanInitializeRenderPass(renderer_data& Info, bool32 IncludeDepth, bool32 ClearRender = true,
	VkImageLayout FinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) 
{
	/* DEPENDS on init_swap_chain() and init_depth_buffer() */

	VkResult U_ASSERT_ONLY Result;
	/* Need attachments for render target and depth buffer */
	VkAttachmentDescription RenderAttachments[2];
	RenderAttachments[0].format = Info.format;
	RenderAttachments[0].samples = NUM_SAMPLES;
	RenderAttachments[0].loadOp =
		ClearRender ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	RenderAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	RenderAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	RenderAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	RenderAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	RenderAttachments[0].finalLayout = FinalLayout;
	RenderAttachments[0].flags = 0;

	if (IncludeDepth) 
	{
		RenderAttachments[1].format = Info.depth.format;
		RenderAttachments[1].samples = NUM_SAMPLES;
		RenderAttachments[1].loadOp = ClearRender ? VK_ATTACHMENT_LOAD_OP_CLEAR
			: VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		RenderAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		RenderAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		RenderAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
		RenderAttachments[1].initialLayout =
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		RenderAttachments[1].finalLayout =
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		RenderAttachments[1].flags = 0;
	}

	VkAttachmentReference ColorReference = {};
	ColorReference.attachment = 0;
	ColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference DepthReference = {};
	DepthReference.attachment = 1;
	DepthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription SubpassDescription = {};
	SubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	SubpassDescription.flags = 0;
	SubpassDescription.inputAttachmentCount = 0;
	SubpassDescription.pInputAttachments = NULL;
	SubpassDescription.colorAttachmentCount = 1;
	SubpassDescription.pColorAttachments = &ColorReference;
	SubpassDescription.pResolveAttachments = NULL;
	SubpassDescription.pDepthStencilAttachment = IncludeDepth ? &DepthReference : NULL;
	SubpassDescription.preserveAttachmentCount = 0;
	SubpassDescription.pPreserveAttachments = NULL;

	VkRenderPassCreateInfo RenderPassInfo = {};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	RenderPassInfo.pNext = NULL;
	RenderPassInfo.attachmentCount = IncludeDepth ? 2 : 1;
	RenderPassInfo.pAttachments = RenderAttachments;
	RenderPassInfo.subpassCount = 1;
	RenderPassInfo.pSubpasses = &SubpassDescription;
	RenderPassInfo.dependencyCount = 0;
	RenderPassInfo.pDependencies = NULL;

	Result = vkCreateRenderPass(Info.device, &RenderPassInfo, NULL, &Info.render_pass);
	Assert(Result == VK_SUCCESS);
}

EShLanguage VulkanFindLanguage(const VkShaderStageFlagBits ShaderType) 
{
	switch (ShaderType) 
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return EShLangVertex;

		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return EShLangTessControl;

		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return EShLangTessEvaluation;

		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return EShLangGeometry;

		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return EShLangFragment;

		case VK_SHADER_STAGE_COMPUTE_BIT:
			return EShLangCompute;

		default:
			return EShLangVertex;
	}
}

void VulkanInitializeShaderResources(TBuiltInResource &Resources) 
{
	Resources.maxLights = 32;
	Resources.maxClipPlanes = 6;
	Resources.maxTextureUnits = 32;
	Resources.maxTextureCoords = 32;
	Resources.maxVertexAttribs = 64;
	Resources.maxVertexUniformComponents = 4096;
	Resources.maxVaryingFloats = 64;
	Resources.maxVertexTextureImageUnits = 32;
	Resources.maxCombinedTextureImageUnits = 80;
	Resources.maxTextureImageUnits = 32;
	Resources.maxFragmentUniformComponents = 4096;
	Resources.maxDrawBuffers = 32;
	Resources.maxVertexUniformVectors = 128;
	Resources.maxVaryingVectors = 8;
	Resources.maxFragmentUniformVectors = 16;
	Resources.maxVertexOutputVectors = 16;
	Resources.maxFragmentInputVectors = 15;
	Resources.minProgramTexelOffset = -8;
	Resources.maxProgramTexelOffset = 7;
	Resources.maxClipDistances = 8;
	Resources.maxComputeWorkGroupCountX = 65535;
	Resources.maxComputeWorkGroupCountY = 65535;
	Resources.maxComputeWorkGroupCountZ = 65535;
	Resources.maxComputeWorkGroupSizeX = 1024;
	Resources.maxComputeWorkGroupSizeY = 1024;
	Resources.maxComputeWorkGroupSizeZ = 64;
	Resources.maxComputeUniformComponents = 1024;
	Resources.maxComputeTextureImageUnits = 16;
	Resources.maxComputeImageUniforms = 8;
	Resources.maxComputeAtomicCounters = 8;
	Resources.maxComputeAtomicCounterBuffers = 1;
	Resources.maxVaryingComponents = 60;
	Resources.maxVertexOutputComponents = 64;
	Resources.maxGeometryInputComponents = 64;
	Resources.maxGeometryOutputComponents = 128;
	Resources.maxFragmentInputComponents = 128;
	Resources.maxImageUnits = 8;
	Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	Resources.maxCombinedShaderOutputResources = 8;
	Resources.maxImageSamples = 0;
	Resources.maxVertexImageUniforms = 0;
	Resources.maxTessControlImageUniforms = 0;
	Resources.maxTessEvaluationImageUniforms = 0;
	Resources.maxGeometryImageUniforms = 0;
	Resources.maxFragmentImageUniforms = 8;
	Resources.maxCombinedImageUniforms = 8;
	Resources.maxGeometryTextureImageUnits = 16;
	Resources.maxGeometryOutputVertices = 256;
	Resources.maxGeometryTotalOutputComponents = 1024;
	Resources.maxGeometryUniformComponents = 1024;
	Resources.maxGeometryVaryingComponents = 64;
	Resources.maxTessControlInputComponents = 128;
	Resources.maxTessControlOutputComponents = 128;
	Resources.maxTessControlTextureImageUnits = 16;
	Resources.maxTessControlUniformComponents = 1024;
	Resources.maxTessControlTotalOutputComponents = 4096;
	Resources.maxTessEvaluationInputComponents = 128;
	Resources.maxTessEvaluationOutputComponents = 128;
	Resources.maxTessEvaluationTextureImageUnits = 16;
	Resources.maxTessEvaluationUniformComponents = 1024;
	Resources.maxTessPatchComponents = 120;
	Resources.maxPatchVertices = 32;
	Resources.maxTessGenLevel = 64;
	Resources.maxViewports = 16;
	Resources.maxVertexAtomicCounters = 0;
	Resources.maxTessControlAtomicCounters = 0;
	Resources.maxTessEvaluationAtomicCounters = 0;
	Resources.maxGeometryAtomicCounters = 0;
	Resources.maxFragmentAtomicCounters = 8;
	Resources.maxCombinedAtomicCounters = 8;
	Resources.maxAtomicCounterBindings = 1;
	Resources.maxVertexAtomicCounterBuffers = 0;
	Resources.maxTessControlAtomicCounterBuffers = 0;
	Resources.maxTessEvaluationAtomicCounterBuffers = 0;
	Resources.maxGeometryAtomicCounterBuffers = 0;
	Resources.maxFragmentAtomicCounterBuffers = 1;
	Resources.maxCombinedAtomicCounterBuffers = 1;
	Resources.maxAtomicCounterBufferSize = 16384;
	Resources.maxTransformFeedbackBuffers = 4;
	Resources.maxTransformFeedbackInterleavedComponents = 64;
	Resources.maxCullDistances = 8;
	Resources.maxCombinedClipAndCullDistances = 8;
	Resources.maxSamples = 4;
	Resources.limits.nonInductiveForLoops = 1;
	Resources.limits.whileLoops = 1;
	Resources.limits.doWhileLoops = 1;
	Resources.limits.generalUniformIndexing = 1;
	Resources.limits.generalAttributeMatrixVectorIndexing = 1;
	Resources.limits.generalVaryingIndexing = 1;
	Resources.limits.generalSamplerIndexing = 1;
	Resources.limits.generalVariableIndexing = 1;
	Resources.limits.generalConstantMatrixVectorIndexing = 1;
}

void VulkanInitializeGLSL()
{
	glslang::InitializeProcess();
}

bool VulkanGLSLtoSPV(VkShaderStageFlagBits ShaderType, const char* PointerShader,
	std::vector<uint32> &SpirvData) 
{
	EShLanguage Stage = VulkanFindLanguage(ShaderType);
	glslang::TShader Shader(Stage);
	glslang::TProgram Program;
	const char* ShaderStrings[1];
	TBuiltInResource Resources;
	VulkanInitializeShaderResources(Resources);

	// Enable SPIR-V and Vulkan rules when parsing GLSL
	EShMessages ShaderMessages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

	ShaderStrings[0] = PointerShader;
	Shader.setStrings(ShaderStrings, 1);

	if (!Shader.parse(&Resources, 100, false, ShaderMessages)) 
	{
		puts(Shader.getInfoLog());
		puts(Shader.getInfoDebugLog());

		return false; // something didn't work
	}

	Program.addShader(&Shader);

	//
	// Program-level processing...
	//

	if (!Program.link(ShaderMessages)) 
	{
		puts(Shader.getInfoLog());
		puts(Shader.getInfoDebugLog());
		fflush(stdout);
		return false;
	}

	glslang::GlslangToSpv(*Program.getIntermediate(Stage), SpirvData);

	return true;
}

void VulkanInitializeShaders(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;
	bool32 U_ASSERT_ONLY ReturnValue;

	local_persist const char* VertexShaderText =
		"#version 400\n"
		"#extension GL_ARB_separate_shader_objects : enable\n"
		"#extension GL_ARB_shading_language_420pack : enable\n"
		"layout (std140, binding = 0) uniform bufferVals {\n"
		"    mat4 mvp;\n"
		"} myBufferVals;\n"
		"layout (location = 0) in vec4 pos;\n"
		"layout (location = 1) in vec4 inColor;\n"
		"layout (location = 0) out vec4 outColor;\n"
		"out gl_PerVertex { \n"
		"    vec4 gl_Position;\n"
		"};\n"
		"void main() {\n"
		"   outColor = inColor;\n"
		"   gl_Position = myBufferVals.mvp * pos;\n"
		"}\n";

	local_persist const char* FragmentShaderText =
		"#version 400\n"
		"#extension GL_ARB_separate_shader_objects : enable\n"
		"#extension GL_ARB_shading_language_420pack : enable\n"
		"layout (location = 0) in vec4 color;\n"
		"layout (location = 0) out vec4 outColor;\n"
		"void main() {\n"
		"   outColor = color;\n"
		"}\n";

	local_persist const char* EightiesShaderText =
		"#version 400\n"
		"#extension GL_ARB_separate_shader_objects : enable\n"
		"#extension GL_ARB_shading_language_420pack : enable\n"
		"precision mediump float;\n"
		"uniform float time;\n"
		"uniform vec2 resolution;\n"
		"//80s Sci-Fi style thing.\n"
		"float linstep(float x0, float x1, float xn)\n"
		"{\n"
		"return (xn - x0) / (x1 - x0);\n"
		"}\n"
		"float cdist(vec2 v0, vec2 v1)\n"
		"{\n"
		"v0 = abs(v0-v1);\n"
		"return max(v0.x,v0.y);\n"
		"}\n"
		"void main( void )\n"
		"{\n"
		"//vec2 aspect = (resolution.xy / cos(sin(time)/2.0) ) / (resolution.y / sin(cos(time)/10.0) );\n"
		"vec2 aspect = (resolution.xy ) / (resolution.y  );\n"
		"vec2 _uv = gl_FragCoord.xy / resolution.y;\n"
		"vec2 cen = aspect/2.0;\n"
		"vec3 color = vec3(0);\n"
		"vec2 gruv = _uv-cen;\n"
		"gruv = vec2(gruv.x * abs(1.4/gruv.y), abs(1.0/gruv.y) + ( mod(time, 1.0 ) ) );\n"
		"gruv.y = clamp(gruv.y,-1e2,1e2);\n"
		"float grid = 2.0 * cdist(vec2(0.5), mod((gruv)*1.0,vec2(1)));\n"
		"float gridmix = max(pow(grid,6.0) * 1.2, smoothstep(0.93,0.98,grid) * 3.0);\n"
		"vec3 gridcol = (mix(vec3(0.00, 0.00, 0.90), vec3(0.90, 0.00, 0.90), _uv.y*2.0) + 1.2) * gridmix;\n"
		"gridcol *= linstep(0.1,1.5,abs(_uv.y - cen.y));\n"
		"color = mix(gridcol, gridcol, gridcol);\n"
		"gl_FragColor = vec4( color, 1.0 );\n"
		"}\n";

	std::vector<uint32> VertexSPIRV;
	Info.shaderStages[0].sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	Info.shaderStages[0].pNext = NULL;
	Info.shaderStages[0].pSpecializationInfo = NULL;
	Info.shaderStages[0].flags = 0;
	Info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	Info.shaderStages[0].pName = "main";


	VulkanInitializeGLSL();
	// ReturnValue = VulkanGLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, VertexShaderText, VertexSPIRV);
	ReturnValue = VulkanGLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, VertexShaderText, VertexSPIRV);
	Assert(ReturnValue);

	VkShaderModuleCreateInfo ModuleCreateInfo;
	ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ModuleCreateInfo.pNext = NULL;
	ModuleCreateInfo.flags = 0;
	ModuleCreateInfo.codeSize = VertexSPIRV.size() * sizeof(unsigned int);
	ModuleCreateInfo.pCode = VertexSPIRV.data();
	Result = vkCreateShaderModule(Info.device, &ModuleCreateInfo, NULL,
		&Info.shaderStages[0].module);
	Assert(Result == VK_SUCCESS);

	std::vector<uint32> FragmentSPV;
	Info.shaderStages[1].sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	Info.shaderStages[1].pNext = NULL;
	Info.shaderStages[1].pSpecializationInfo = NULL;
	Info.shaderStages[1].flags = 0;
	Info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	Info.shaderStages[1].pName = "main";

	ReturnValue = VulkanGLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, FragmentShaderText, FragmentSPV);
	Assert(ReturnValue);

	ModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	ModuleCreateInfo.pNext = NULL;
	ModuleCreateInfo.flags = 0;
	ModuleCreateInfo.codeSize = FragmentSPV.size() * sizeof(unsigned int);
	ModuleCreateInfo.pCode = FragmentSPV.data();
	Result = vkCreateShaderModule(Info.device, &ModuleCreateInfo, NULL,
		&Info.shaderStages[1].module);
	Assert(Result == VK_SUCCESS);

	glslang::FinalizeProcess();
}

void VulkanExecuteEndCommandbuffer(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	Result = vkEndCommandBuffer(Info.cmd);
	Assert(Result == VK_SUCCESS);
}

void VulkanExecuteQueueCommandbuffer(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	/* Queue the command buffer for execution */
	const VkCommandBuffer CommandBuffers[] = {Info.cmd};
	VkFenceCreateInfo FenceInfo;
	VkFence DrawFence;
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.pNext = NULL;
	FenceInfo.flags = 0;
	vkCreateFence(Info.device, &FenceInfo, NULL, &DrawFence);

	VkPipelineStageFlags PipelineStageFlags =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo SubmitInfo[1] = {};
	SubmitInfo[0].pNext = NULL;
	SubmitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo[0].waitSemaphoreCount = 0;
	SubmitInfo[0].pWaitSemaphores = NULL;
	SubmitInfo[0].pWaitDstStageMask = &PipelineStageFlags;
	SubmitInfo[0].commandBufferCount = 1;
	SubmitInfo[0].pCommandBuffers = CommandBuffers;
	SubmitInfo[0].signalSemaphoreCount = 0;
	SubmitInfo[0].pSignalSemaphores = NULL;

	Result = vkQueueSubmit(Info.graphics_queue, 1, SubmitInfo, DrawFence);
	Assert(Result == VK_SUCCESS);

	do {
		Result =
			vkWaitForFences(Info.device, 1, &DrawFence, VK_TRUE, FENCE_TIMEOUT);
	} while (Result == VK_TIMEOUT);
	Assert(Result == VK_SUCCESS);

	vkDestroyFence(Info.device, DrawFence, NULL);
}

void VulkanInitializeFramebuffers(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	VkImageView ImageViews[2];
	ImageViews[1] = Info.depth.view;

	VkFramebufferCreateInfo FramebufferInfo = {};
	FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	FramebufferInfo.pNext = NULL;
	FramebufferInfo.renderPass = Info.render_pass;
	FramebufferInfo.attachmentCount = 2;
	FramebufferInfo.pAttachments = ImageViews;
	FramebufferInfo.width = Info.width;
	FramebufferInfo.height = Info.height;
	FramebufferInfo.layers = 1;

	Info.framebuffers = (VkFramebuffer *)MemAlloc(Info.swapchainImageCount *
		sizeof(VkFramebuffer));
	Assert(Info.framebuffers);

	for (uint32 Index = 0; Index < Info.swapchainImageCount; Index++) 
	{
		ImageViews[0] = Info.buffers[Index].view;
		Result = vkCreateFramebuffer(Info.device, &FramebufferInfo, NULL,
			&Info.framebuffers[Index]);
		Assert(Result == VK_SUCCESS);
	}
}

bool VulkanGetMemoryType(renderer_data& Info, uint32 TypeBits,
	VkFlags RequirementsMask,
	uint32* TypeIndex) 
{
	// Search memtypes to find first index with those properties
	for (uint32 Index = 0; Index < Info.memory_properties.memoryTypeCount; Index++) 
	{
		if ((TypeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((Info.memory_properties.memoryTypes[Index].propertyFlags &
				RequirementsMask) == RequirementsMask) 
			{
				*TypeIndex = Index;
				return true;
			}
		}
		TypeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}

void VulkanInitializeVertexBuffer(renderer_data& Info, const void* VertexData,
	uint32 DataSize, uint32 DataStride,
	bool32 UseTexture)
{
	/*
	* Set up a vertex buffer:
	* - Create a buffer
	* - Map it and write the vertex data into it
	* - Bind it using vkCmdBindVertexBuffers
	* - Later, at pipeline creation,
	* -      fill in vertex input part of the pipeline with relevent data
	*/

	VkResult U_ASSERT_ONLY Result;
	bool32 U_ASSERT_ONLY DidPass;

	VkBufferCreateInfo BufferInfo = {};
	BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	BufferInfo.pNext = NULL;
	BufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	BufferInfo.size = DataSize;
	BufferInfo.queueFamilyIndexCount = 0;
	BufferInfo.pQueueFamilyIndices = NULL;
	BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	BufferInfo.flags = 0;
	Result = vkCreateBuffer(Info.device, &BufferInfo, NULL, &Info.vertex_buffer.buf);
	Assert(Result == VK_SUCCESS);

	VkMemoryRequirements MemoryRequirements;
	vkGetBufferMemoryRequirements(Info.device, Info.vertex_buffer.buf,
		&MemoryRequirements);

	VkMemoryAllocateInfo MemoryAllocInfo = {};
	MemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	MemoryAllocInfo.pNext = NULL;
	MemoryAllocInfo.memoryTypeIndex = 0;

	MemoryAllocInfo.allocationSize = MemoryRequirements.size;
	DidPass = VulkanCheckMemoryTypes(Info, MemoryRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&MemoryAllocInfo.memoryTypeIndex);
	Assert(DidPass && "No mappable, coherent memory");

	Result = vkAllocateMemory(Info.device, &MemoryAllocInfo, NULL,
		&(Info.vertex_buffer.mem));
	Assert(Result == VK_SUCCESS);
	Info.vertex_buffer.buffer_info.range = MemoryRequirements.size;
	Info.vertex_buffer.buffer_info.offset = 0;

	uint8 *PointerData;
	Result = vkMapMemory(Info.device, Info.vertex_buffer.mem, 0, MemoryRequirements.size, 0,
		(void **)&PointerData);
	Assert(Result == VK_SUCCESS);

	memcpy(PointerData, VertexData, DataSize);

	vkUnmapMemory(Info.device, Info.vertex_buffer.mem);

	Result = vkBindBufferMemory(Info.device, Info.vertex_buffer.buf,
		Info.vertex_buffer.mem, 0);
	Assert(Result == VK_SUCCESS);

	Info.vi_binding.binding = 0;
	Info.vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	Info.vi_binding.stride = DataStride;

	Info.vi_attribs[0].binding = 0;
	Info.vi_attribs[0].location = 0;
	Info.vi_attribs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	Info.vi_attribs[0].offset = 0;
	Info.vi_attribs[1].binding = 0;
	Info.vi_attribs[1].location = 1;
	Info.vi_attribs[1].format =
		UseTexture ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
	Info.vi_attribs[1].offset = 16;
}

void VulkanInitializePiplineCache(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	VkPipelineCacheCreateInfo PipelineCache;
	PipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	PipelineCache.pNext = NULL;
	PipelineCache.initialDataSize = 0;
	PipelineCache.pInitialData = NULL;
	PipelineCache.flags = 0;
	Result = vkCreatePipelineCache(Info.device, &PipelineCache, NULL,
		&Info.pipelineCache);

	Assert(Result == VK_SUCCESS);
}

void VulkanInitializePipeline(renderer_data& Info, VkBool32 IncludeDepth,
	VkBool32 IncludeVertexInput = true)
{
	VkResult U_ASSERT_ONLY Result;

	VkDynamicState DynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo DynamicState = {};
	memset(DynamicStateEnables, 0, sizeof DynamicStateEnables);
	DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	DynamicState.pNext = NULL;
	DynamicState.pDynamicStates = DynamicStateEnables;
	DynamicState.dynamicStateCount = 0;

	VkPipelineVertexInputStateCreateInfo VertexInput;
	memset(&VertexInput, 0, sizeof(VertexInput));
	VertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	if (IncludeVertexInput) 
	{
		VertexInput.pNext = NULL;
		VertexInput.flags = 0;
		VertexInput.vertexBindingDescriptionCount = 1;
		VertexInput.pVertexBindingDescriptions = &Info.vi_binding;
		VertexInput.vertexAttributeDescriptionCount = 2;
		VertexInput.pVertexAttributeDescriptions = Info.vi_attribs;
	}

	VkPipelineInputAssemblyStateCreateInfo AssemblyInput;
	AssemblyInput.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	AssemblyInput.pNext = NULL;
	AssemblyInput.flags = 0;
	AssemblyInput.primitiveRestartEnable = VK_FALSE;
	AssemblyInput.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineRasterizationStateCreateInfo RasterInput;
	RasterInput.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	RasterInput.pNext = NULL;
	RasterInput.flags = 0;
	RasterInput.polygonMode = VK_POLYGON_MODE_FILL;
	RasterInput.cullMode = VK_CULL_MODE_BACK_BIT;
	RasterInput.frontFace = VK_FRONT_FACE_CLOCKWISE;
	RasterInput.depthClampEnable = IncludeDepth;
	RasterInput.rasterizerDiscardEnable = VK_FALSE;
	RasterInput.depthBiasEnable = VK_FALSE;
	RasterInput.depthBiasConstantFactor = 0;
	RasterInput.depthBiasClamp = 0;
	RasterInput.depthBiasSlopeFactor = 0;
	RasterInput.lineWidth = 1.0f;

	VkPipelineColorBlendStateCreateInfo ColorBlendInput;
	ColorBlendInput.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	ColorBlendInput.flags = 0;
	ColorBlendInput.pNext = NULL;
	VkPipelineColorBlendAttachmentState ColorBlendAttachments[1];
	ColorBlendAttachments[0].colorWriteMask = 0xf;
	ColorBlendAttachments[0].blendEnable = VK_FALSE;
	ColorBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachments[0].colorBlendOp = VK_BLEND_OP_ADD;
	ColorBlendAttachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendAttachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendAttachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendAttachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorBlendInput.attachmentCount = 1;
	ColorBlendInput.pAttachments = ColorBlendAttachments;
	ColorBlendInput.logicOpEnable = VK_FALSE;
	ColorBlendInput.logicOp = VK_LOGIC_OP_NO_OP;
	ColorBlendInput.blendConstants[0] = 1.0f;
	ColorBlendInput.blendConstants[1] = 1.0f;
	ColorBlendInput.blendConstants[2] = 1.0f;
	ColorBlendInput.blendConstants[3] = 1.0f;

	VkPipelineViewportStateCreateInfo ViewportInput = {};
	ViewportInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	ViewportInput.pNext = NULL;
	ViewportInput.flags = 0;

	ViewportInput.viewportCount = NUM_VIEWPORTS;
	DynamicStateEnables[DynamicState.dynamicStateCount++] =
		VK_DYNAMIC_STATE_VIEWPORT;
	ViewportInput.scissorCount = NUM_SCISSORS;
	DynamicStateEnables[DynamicState.dynamicStateCount++] =
		VK_DYNAMIC_STATE_SCISSOR;
	ViewportInput.pScissors = NULL;
	ViewportInput.pViewports = NULL;

	VkPipelineDepthStencilStateCreateInfo DepthStencilInput;
	DepthStencilInput.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	DepthStencilInput.pNext = NULL;
	DepthStencilInput.flags = 0;
	DepthStencilInput.depthTestEnable = IncludeDepth;
	DepthStencilInput.depthWriteEnable = IncludeDepth;
	DepthStencilInput.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	DepthStencilInput.depthBoundsTestEnable = VK_FALSE;
	DepthStencilInput.stencilTestEnable = VK_FALSE;
	DepthStencilInput.back.failOp = VK_STENCIL_OP_KEEP;
	DepthStencilInput.back.passOp = VK_STENCIL_OP_KEEP;
	DepthStencilInput.back.compareOp = VK_COMPARE_OP_ALWAYS;
	DepthStencilInput.back.compareMask = 0;
	DepthStencilInput.back.reference = 0;
	DepthStencilInput.back.depthFailOp = VK_STENCIL_OP_KEEP;
	DepthStencilInput.back.writeMask = 0;
	DepthStencilInput.minDepthBounds = 0;
	DepthStencilInput.maxDepthBounds = 0;
	DepthStencilInput.stencilTestEnable = VK_FALSE;
	DepthStencilInput.front = DepthStencilInput.back;

	VkPipelineMultisampleStateCreateInfo MultisampleInput;
	MultisampleInput.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	MultisampleInput.pNext = NULL;
	MultisampleInput.flags = 0;
	MultisampleInput.pSampleMask = NULL;
	MultisampleInput.rasterizationSamples = NUM_SAMPLES;
	MultisampleInput.sampleShadingEnable = VK_FALSE;
	MultisampleInput.alphaToCoverageEnable = VK_FALSE;
	MultisampleInput.alphaToOneEnable = VK_FALSE;
	MultisampleInput.minSampleShading = 0.0;

	VkGraphicsPipelineCreateInfo PipelineInfo;
	PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	PipelineInfo.pNext = NULL;
	PipelineInfo.layout = Info.pipeline_layout;
	PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	PipelineInfo.basePipelineIndex = 0;
	PipelineInfo.flags = 0;
	PipelineInfo.pVertexInputState = &VertexInput;
	PipelineInfo.pInputAssemblyState = &AssemblyInput;
	PipelineInfo.pRasterizationState = &RasterInput;
	PipelineInfo.pColorBlendState = &ColorBlendInput;
	PipelineInfo.pTessellationState = NULL;
	PipelineInfo.pMultisampleState = &MultisampleInput;
	PipelineInfo.pDynamicState = &DynamicState;
	PipelineInfo.pViewportState = &ViewportInput;
	PipelineInfo.pDepthStencilState = &DepthStencilInput;
	PipelineInfo.pStages = Info.shaderStages;
	PipelineInfo.stageCount = 2;
	PipelineInfo.renderPass = Info.render_pass;
	PipelineInfo.subpass = 0;

	Result = vkCreateGraphicsPipelines(Info.device, Info.pipelineCache, 1,
		&PipelineInfo, NULL, &Info.pipeline);
	Assert(Result == VK_SUCCESS);
}

void VulkanDraw(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	VkClearValue ClearValues[2];
	ClearValues[0].color.float32[0] = 0.2f;
	ClearValues[0].color.float32[1] = 0.2f;
	ClearValues[0].color.float32[2] = 0.2f;
	ClearValues[0].color.float32[3] = 0.2f;
	ClearValues[1].depthStencil.depth = 1.0f;
	ClearValues[1].depthStencil.stencil = 0;

	VkSemaphoreCreateInfo ImageAcquiredSemaphoreCreateInfo;
	ImageAcquiredSemaphoreCreateInfo.sType =
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	ImageAcquiredSemaphoreCreateInfo.pNext = NULL;
	ImageAcquiredSemaphoreCreateInfo.flags = 0;

	Result = vkCreateSemaphore(Info.device, &ImageAcquiredSemaphoreCreateInfo,
		NULL, &Info.imageAcquiredSemaphore);
	Assert(Result == VK_SUCCESS);

	// Get the index of the next available swapchain image:
	Result = vkAcquireNextImageKHR(Info.device, Info.swap_chain, UINT64_MAX,
		Info.imageAcquiredSemaphore, VK_NULL_HANDLE,
		&Info.current_buffer);
	// TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
	// return codes
	Assert(Result == VK_SUCCESS);

	VulkanSetImageLayout(Info, Info.buffers[Info.current_buffer].image,
		VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	VkRenderPassBeginInfo RenderPassBegin;
	RenderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassBegin.pNext = NULL;
	RenderPassBegin.renderPass = Info.render_pass;
	RenderPassBegin.framebuffer = Info.framebuffers[Info.current_buffer];
	RenderPassBegin.renderArea.offset.x = 0;
	RenderPassBegin.renderArea.offset.y = 0;
	RenderPassBegin.renderArea.extent.width = Info.width;
	RenderPassBegin.renderArea.extent.height = Info.height;
	RenderPassBegin.clearValueCount = 2;
	RenderPassBegin.pClearValues = ClearValues;

	vkCmdBeginRenderPass(Info.cmd, &RenderPassBegin, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(Info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, Info.pipeline);
	vkCmdBindDescriptorSets(Info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
		Info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
		Info.desc_set, 0, NULL);

	const VkDeviceSize offsets[1] = {0};
	vkCmdBindVertexBuffers(Info.cmd, 0, 1, &Info.vertex_buffer.buf, offsets);

	Info.viewport.height = (float)Info.height;
	Info.viewport.width = (float)Info.width;
	Info.viewport.minDepth = (float)0.0f;
	Info.viewport.maxDepth = (float)1.0f;
	Info.viewport.x = 0;
	Info.viewport.y = 0;
	vkCmdSetViewport(Info.cmd, 0, NUM_VIEWPORTS, &Info.viewport);


	Info.scissor.extent.width = Info.width;
	Info.scissor.extent.height = Info.height;
	Info.scissor.offset.x = 0;
	Info.scissor.offset.y = 0;
	vkCmdSetScissor(Info.cmd, 0, NUM_SCISSORS, &Info.scissor);

	vkCmdDraw(Info.cmd, 12 * 3, 1, 0, 0);
	vkCmdEndRenderPass(Info.cmd);

	Result = vkEndCommandBuffer(Info.cmd);
	const VkCommandBuffer CommandBuffers[] = {Info.cmd};
	VkFenceCreateInfo FenceInfo;
	VkFence DrawFence;
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.pNext = NULL;
	FenceInfo.flags = 0;
	vkCreateFence(Info.device, &FenceInfo, NULL, &DrawFence);

	VkPipelineStageFlags PipelineStageFlags =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo SubmitInfo[1] = {};
	SubmitInfo[0].pNext = NULL;
	SubmitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo[0].waitSemaphoreCount = 1;
	SubmitInfo[0].pWaitSemaphores = &Info.imageAcquiredSemaphore;
	SubmitInfo[0].pWaitDstStageMask = &PipelineStageFlags;
	SubmitInfo[0].commandBufferCount = 1;
	SubmitInfo[0].pCommandBuffers = CommandBuffers;
	SubmitInfo[0].signalSemaphoreCount = 0;
	SubmitInfo[0].pSignalSemaphores = NULL;

	/* Queue the command buffer for execution */
	Result = vkQueueSubmit(Info.graphics_queue, 1, SubmitInfo, DrawFence);
	Assert(Result == VK_SUCCESS);

	/* Now present the image in the window */

	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Info.swap_chain;
	PresentInfo.pImageIndices = &Info.current_buffer;
	PresentInfo.pWaitSemaphores = NULL;
	PresentInfo.waitSemaphoreCount = 0;
	PresentInfo.pResults = NULL;

	/* Make sure command buffer is finished before presenting */
	do 
	{
		Result =
			vkWaitForFences(Info.device, 1, &DrawFence, VK_TRUE, FENCE_TIMEOUT);
	} while (Result == VK_TIMEOUT);

	Assert(Result == VK_SUCCESS);
	Result = vkQueuePresentKHR(Info.present_queue, &PresentInfo);
	Assert(Result == VK_SUCCESS);

	vkDestroySemaphore(Info.device, Info.imageAcquiredSemaphore, NULL);
	vkDestroyFence(Info.device, DrawFence, NULL);
}

void VulkanOtherDraw(renderer_data& Info)
{
	VkResult Result;

	// Acquire the next image from the swap chaing
	Result = (vkAcquireNextImageKHR(Info.device, Info.swap_chain, UINT64_MAX,
		Info.imageAcquiredSemaphore, VK_NULL_HANDLE,
		&Info.current_buffer));

	// Submit post present image barrier to transform the image back to a color attachment that our render pass can write to
	VkSubmitInfo SubmitInfo = {};
	SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo.pNext = NULL;
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &Info.cmd;
	Result = (vkQueueSubmit(Info.graphics_queue, 1, &SubmitInfo, VK_NULL_HANDLE));

	// Command buffer to be sumitted to the queue
	SubmitInfo.commandBufferCount = 1;
	SubmitInfo.pCommandBuffers = &Info.cmd;

	// Submit to queue
	Result = (vkQueueSubmit(Info.graphics_queue, 1, &SubmitInfo, VK_NULL_HANDLE));

	// Submit pre present image barrier to transform the image from color attachment to present(khr) for presenting to the swap chain
	VkSubmitInfo SubmitInfoPresent = {};
	SubmitInfoPresent.commandBufferCount = 1;
	SubmitInfoPresent.pCommandBuffers = &Info.cmd;
	Result = (vkQueueSubmit(Info.graphics_queue, 1, &SubmitInfoPresent, VK_NULL_HANDLE));

	VkPresentInfoKHR PresentInfo = {};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Info.swap_chain;
	PresentInfo.pImageIndices = &Info.current_buffer;
	// Check if a wait semaphore has been specified to wait for before presenting the image
	if (Info.imageAcquiredSemaphore != VK_NULL_HANDLE)
	{
		PresentInfo.pWaitSemaphores = &Info.imageAcquiredSemaphore;
		PresentInfo.waitSemaphoreCount = 1;
	}
	Result = vkQueuePresentKHR(Info.present_queue, &PresentInfo);

	Result = (vkQueueWaitIdle(Info.present_queue));

#if 0
	VkResult Result;

	VkSemaphoreCreateInfo ImageAcquiredSemaphoreCreateInfo;
	ImageAcquiredSemaphoreCreateInfo.sType =
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	ImageAcquiredSemaphoreCreateInfo.pNext = NULL;
	ImageAcquiredSemaphoreCreateInfo.flags = 0;

	Result = vkCreateSemaphore(Info.device, &ImageAcquiredSemaphoreCreateInfo,
		NULL, &Info.imageAcquiredSemaphore);
	Assert(Result == VK_SUCCESS);

	// Get the index of the next available swapchain image:
	Result = vkAcquireNextImageKHR(Info.device, Info.swap_chain, UINT64_MAX,
		Info.imageAcquiredSemaphore, VK_NULL_HANDLE,
		&Info.current_buffer);
	// TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
	// return codes
	Assert(Result == VK_SUCCESS);

	VkFenceCreateInfo FenceInfo;
	VkFence DrawFence;
	FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceInfo.pNext = NULL;
	FenceInfo.flags = 0;
	vkCreateFence(Info.device, &FenceInfo, NULL, &DrawFence);

	VkPipelineStageFlags PipelineStageFlags =
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo SubmitInfo[1] = {};
	SubmitInfo[0].pNext = NULL;
	SubmitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	SubmitInfo[0].waitSemaphoreCount = 1;
	SubmitInfo[0].pWaitSemaphores = &Info.imageAcquiredSemaphore;
	SubmitInfo[0].pWaitDstStageMask = &PipelineStageFlags;
	SubmitInfo[0].commandBufferCount = 1;
	SubmitInfo[0].pCommandBuffers = &Info.cmd;
	SubmitInfo[0].signalSemaphoreCount = 0;
	SubmitInfo[0].pSignalSemaphores = NULL;

	/* Queue the command buffer for execution */
	Result = vkQueueSubmit(Info.graphics_queue, 1, SubmitInfo, DrawFence);
	Assert(Result == VK_SUCCESS);

	/* Now present the image in the window */

	VkPresentInfoKHR PresentInfo;
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = NULL;
	PresentInfo.swapchainCount = 1;
	PresentInfo.pSwapchains = &Info.swap_chain;
	PresentInfo.pImageIndices = &Info.current_buffer;
	PresentInfo.pWaitSemaphores = NULL;
	PresentInfo.waitSemaphoreCount = 0;
	PresentInfo.pResults = NULL;

	/* Make sure command buffer is finished before presenting */
	do 
	{
		Result =
			vkWaitForFences(Info.device, 1, &DrawFence, VK_TRUE, FENCE_TIMEOUT);
	} while (Result == VK_TIMEOUT);

	Assert(Result == VK_SUCCESS);
	Result = vkQueuePresentKHR(Info.present_queue, &PresentInfo);
	Assert(Result == VK_SUCCESS);
#endif
}

void VulkanUpdateUniformBuffers(renderer_data& Info)
{
	VkResult U_ASSERT_ONLY Result;

	local_persist int32 Index = 5;
	local_persist int32 CounterMax = 10000;
	local_persist int32 Count = 0;

	Count++;

	if(Count > CounterMax)
	{
		Index++;
		Count = 0;
	}

	Info.View = LookAt(
		Vector3(-5, Index, -10), // Camera is at (0,3,10), in World Space
		Vector3(0, 0, 0),  // and looks at the origin
		Vector3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	Info.MVP = Info.Clip * Info.Projection * Info.View * Info.Model;

	uint8 *PointerData;
	Result = vkMapMemory(Info.device, Info.uniform_data.mem, 0, sizeof(Info.MVP), 0,
		(void **)&PointerData);
	Assert(Result == VK_SUCCESS);

	MemCopy(PointerData, &Info.MVP, sizeof(Info.MVP));

	vkUnmapMemory(Info.device, Info.uniform_data.mem);
}

void VulkanCleanup(renderer_data& Info)
{
	for (uint32 Index = 0; Index < Info.swapchainImageCount; Index++) 
	{
		vkDestroyFramebuffer(Info.device, Info.framebuffers[Index], NULL);
	}
	MemDealloc(Info.framebuffers);

	vkDestroyShaderModule(Info.device, Info.shaderStages[0].module, NULL);
	vkDestroyShaderModule(Info.device, Info.shaderStages[1].module, NULL);

	vkDestroyRenderPass(Info.device, Info.render_pass, NULL);
	// vkDestroySemaphore(Info.device, ImageAcquiredSemaphore, NULL);

	vkDestroyDescriptorPool(Info.device, Info.desc_pool, NULL);

	for(int Index = 0; Index < NUM_DESCRIPTOR_SETS; Index++)
	{
		vkDestroyDescriptorSetLayout(Info.device, Info.desc_layout[Index], NULL);
	}

	vkDestroyPipelineLayout(Info.device, Info.pipeline_layout, NULL);

	vkDestroyBuffer(Info.device, Info.uniform_data.buf, NULL);
	vkFreeMemory(Info.device, Info.uniform_data.mem, NULL);

	vkDestroyImageView(Info.device, Info.depth.view, NULL);
	vkDestroyImage(Info.device, Info.depth.image, NULL);
	vkFreeMemory(Info.device, Info.depth.mem, NULL);

	vkDestroySwapchainKHR(Info.device, Info.swap_chain, NULL);

	for(uint32_t Index = 0; Index < Info.swapchainImageCount; ++Index)
	{
		vkDestroyImageView(Info.device, Info.buffers[Index].view, NULL);
	}

	vkFreeCommandBuffers(Info.device, Info.cmd_pool, 1, &Info.cmd);

	vkDestroyCommandPool(Info.device, Info.cmd_pool, NULL);

	vkDestroyDevice(Info.device, NULL);

	vkDestroyInstance(Info.inst, NULL);
}

void VulkanInitialize(void* Handle, void* Window, renderer_data& Renderer)
{
	struct Vertex {
		vector4 Position;          // Position data
		vector4 Color;             // Color
	};

#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f

	static const Vertex SolidFaceCube[] = {
		//red face
		{XYZ1(-1,-1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1( 1,-1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1( 1,-1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		{XYZ1( 1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
		//green face
		{XYZ1(-1,-1,-1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1( 1,-1,-1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(-1, 1,-1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1(-1, 1,-1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1( 1,-1,-1), XYZ1(0.f, 1.f, 0.f)},
		{XYZ1( 1, 1,-1), XYZ1(0.f, 1.f, 0.f)},
		//blue face
		{XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1,-1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, 1,-1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1, 1,-1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1,-1, 1), XYZ1(0.f, 0.f, 1.f)},
		{XYZ1(-1,-1,-1), XYZ1(0.f, 0.f, 1.f)},
		//yellow face
		{XYZ1( 1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1( 1, 1,-1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1( 1,-1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1( 1,-1, 1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1( 1, 1,-1), XYZ1(1.f, 1.f, 0.f)},
		{XYZ1( 1,-1,-1), XYZ1(1.f, 1.f, 0.f)},
		//magenta face
		{XYZ1( 1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1( 1, 1,-1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1( 1, 1,-1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
		{XYZ1(-1, 1,-1), XYZ1(1.f, 0.f, 1.f)},
		//cyan face
		{XYZ1( 1,-1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1( 1,-1,-1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1,-1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1,-1, 1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1( 1,-1,-1), XYZ1(0.f, 1.f, 1.f)},
		{XYZ1(-1,-1,-1), XYZ1(0.f, 1.f, 1.f)},
	};

	// Device association

	VulkanInitializeGlobalLayerProperties(Renderer);

	VulkanInitializeInstanceExtensionNames(Renderer);

	VulkanInitializeDeviceExtensionNames(Renderer);

	VulkanInitializeInstance(Renderer);

	VulkanInitializeEnumerateDevice(Renderer);

	VulkanInitializeWindowSize(Renderer, 500, 500);

	VulkanInitializeConnection(Renderer, Handle, Window);

	VulkanInitializeWindow(Renderer);

	VulkanInitializeSwapChainExtensions(Renderer);

	VulkanInitializeDevice(Renderer);

	// Rendering commands

	VulkanInitializeCommandPool(Renderer);

	VulkanInitializeCommandBuffers(Renderer);

	VulkanExecuteBeginCommandBuffer(Renderer);

	VulkanInitializeDeviceQueue(Renderer);

	VulkanInitializeSwapChain(Renderer);

	VulkanInitializeDepthBuffer(Renderer);

	VulkanInitializeUniformBuffer(Renderer);

	VulkanInitializeDescriptorAndPipelineLayout(Renderer);

	// VulkanInitializeRenderPass(Renderer);
	VulkanInitializeRenderPass(Renderer, true);

	VulkanInitializeShaders(Renderer);

	VulkanInitializeFramebuffers(Renderer);

	VulkanInitializeVertexBuffer(Renderer, SolidFaceCube,
		sizeof(SolidFaceCube),
		sizeof(SolidFaceCube[0]), false);

	VulkanInitializeDescriptorPool(Renderer, false);

	VulkanInitializeDescriptorSet(Renderer, false);

	VulkanInitializePiplineCache(Renderer);

	VulkanInitializePipeline(Renderer, true);

	VulkanDraw(Renderer);

	// VulkanCleanup(Info);
}

void VulkanUpdateAndRender(real32 Time, renderer_data& Renderer)
{
	VulkanUpdateUniformBuffers(Renderer);

	VulkanOtherDraw(Renderer);
}