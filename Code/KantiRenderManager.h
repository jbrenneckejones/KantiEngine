#ifndef KANTI_RENDER_MANAGER

#include "KantiEngine.h"

static const char* vertShaderText =
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

static const char* fragShaderText =
"#version 400\n"
"#extension GL_ARB_separate_shader_objects : enable\n"
"#extension GL_ARB_shading_language_420pack : enable\n"
"layout (location = 0) in vec4 color;\n"
"layout (location = 0) out vec4 outColor;\n"
"void main() {\n"
"   outColor = color;\n"
"}\n";

#include "Vulkan/vulkan.h"
#include "KantiVulkan/VulkanBuffer.h"

struct render_matrices
{
	Matrix4x4 Projection;
	Matrix4x4 Model;
	Matrix4x4 View;
};

struct uniform_data 
{
	VkBuffer Buffer;
	VkDeviceMemory Memory;
	VkDescriptorBufferInfo Descriptor;
	uint32 AllocSize;
	void* Mapped = nullptr;
};

class KantiRenderObject
{
	public:

	render_matrices RenderMatrices;
	Vector3 Position;
	Vector3 Color;

	VulkanBuffer VertexBuffer;
	VulkanBuffer IndexBuffer;
	uint32 IndexCount;

	uniform_data UniformData;

	
	VkDescriptorSet DescriptorSet;

	void PrepareUniformBuffer(class VulkanEncapsulatedDevice* Device);

	void Draw(VkCommandBuffer CommandBuffer, VkPipelineLayout PipelineLayout);
	void UpdateUniformBuffer(class VulkanEncapsulatedDevice* Device, class KantiCameraManager* Camera, real32 Timer);

	void SetupDescriptorSet(class VulkanEncapsulatedDevice* Device, VkDescriptorPool DescriptorPool, VkDescriptorSetLayout DescriptorSetLayout);

	void Generate(class VulkanEncapsulatedDevice* Device, VkQueue Queue);

};

class KantiRenderManager
{
	public:

	platform_game_render_initialize* InitializeRenderer;
	platform_game_update_and_render* UpdateRenderer;
};

#define KANTI_RENDER_MANAGER
#endif

