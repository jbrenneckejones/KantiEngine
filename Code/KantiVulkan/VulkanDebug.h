#ifndef VULKAN_DEBUG

#include "KantiVulkan.h"

inline VkBool32
DebugMessageCallback(
	VkDebugReportFlagsEXT Flags,
	VkDebugReportObjectTypeEXT ObjectType,
	uint64 Object,
	memory_index Location,
	int32 MessageCode,
	const char* LayerPrefix,
	const char* Message,
	void* UserData)
{
	// Select prefix depending on flags passed to the callback
	// Note that multiple flags may be set for a single validation message
	k_string Prefix;

	uint32 CountPrefix = 0;
	for(const char* Character = LayerPrefix; *Character; ++Character)
	{
		CountPrefix++;
	}

	k_string LayerPrefixString((char *)LayerPrefix, CountPrefix);

	uint32 CountMessage = 0;
	for(const char* Character = Message; *Character; ++Character)
	{
		CountMessage++;
	}

	k_string MessageString((char *)Message, CountMessage);

	// Error that may result in undefined behaviour
	if (Flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		Prefix = "ERROR:";
	};
	// Warnings may hint at unexpected / non-spec API usage
	if (Flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		Prefix = "WARNING:";
	};
	// May indicate sub-optimal usage of the API
	if (Flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		Prefix = "PERFORMANCE:";
	};
	// Informal messages that may become handy during debugging
	if (Flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
	{
		Prefix = "INFO:";
	}
	// Diagnostic info from the Vulkan loader and layers
	// Usually not helpful in terms of API usage, but may help to debug layer and loader problems 
	if (Flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
	{
		Prefix = "DEBUG:";
	}

	// Display message to default output (console if activated)

	DebugMessage(Prefix);
	DebugMessage("[");
	DebugMessage(LayerPrefixString);
	DebugMessage("] Code ");

	k_string Code = '0' + MessageCode;
	DebugMessage(Code);
	DebugMessage(" : ");
	DebugMessage(MessageString);
	DebugMessage("\n");

	// The return value of this callback controls wether the Vulkan call that caused
	// the validation message will be aborted or not
	// We return VK_FALSE as we DON'T want Vulkan calls that cause a validation message 
	// (and return a VkResult) to abort
	// If you instead want to have calls abort, pass in VK_TRUE and the function will 
	// return VK_ERROR_VALIDATION_FAILED_EXT 
	return VK_FALSE;
}

class VulkanDebugLayer
{
	public:

	VkDebugReportCallbackEXT MessageCallback;

	void SetupDebugging(VkInstance VulkanRendererInstance, VkDebugReportFlagsEXT Flags, VkDebugReportCallbackEXT CallBack);


	void SetupDebug(VkDevice VulkanDevice);
};


#define VULKAN_DEBUG
#endif