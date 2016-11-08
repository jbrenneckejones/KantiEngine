#include "VulkanDebug.h"

void VulkanDebugLayer::SetupDebugging(VkInstance VulkanInstance, VkDebugReportFlagsEXT Flags, VkDebugReportCallbackEXT CallBack)
{
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(VulkanInstance, "vkCreateDebugReportCallbackEXT");

	VkDebugReportCallbackCreateInfoEXT DebugCreateInfo = { };
	DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	// TODO(Julian): Fix this
	DebugCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugMessageCallback;
	DebugCreateInfo.flags = Flags;

	VkResult Error = CreateDebugReportCallback(
		VulkanInstance,
		&DebugCreateInfo,
		nullptr,
		(CallBack != VK_NULL_HANDLE) ? &CallBack : &MessageCallback);
	Assert(!Error);
	
}

void VulkanDebugLayer::SetupDebug(VkDevice VulkanDevice)
{
	// pfnDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectTagEXT"));
	// pfnDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(device, "vkDebugMarkerSetObjectNameEXT"));
	// pfnCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerBeginEXT"));
	// pfnCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerEndEXT"));
	// pfnCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(device, "vkCmdDebugMarkerInsertEXT"));

	// Set flag if at least one function pointer is present
	// active = (pfnDebugMarkerSetObjectName != VK_NULL_HANDLE);
}
