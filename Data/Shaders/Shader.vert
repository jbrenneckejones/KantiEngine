#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform KMatrix4x4 {
    mat4 Model;
    mat4 View;
    mat4 Projection;
} Matrices;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = Matrices.Projection * Matrices.View * Matrices.Model * vec4(inPosition, 1.0);
    fragColor = inColor;
	fragUV = inUV;
}