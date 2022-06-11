#version 460 core
#ifndef VULKAN
#extension GL_KHR_vulkan_glsl : enable
#endif

layout (set=0, binding=1) uniform Trololo {
	mat4 MVP;
	mat4 ModelMatrix;
	mat3 NormalMatrix;
};

layout (location=0) in vec3 aPos;
layout (location=1) in vec2 aUV;
layout (location=2) in vec3 aNormal;

layout (location=0) out vec2 UV;
layout (location=1) out vec3 PosWS;
layout (location=2) out vec3 Normal;

void main()
{
	UV = aUV;
	PosWS = vec3(ModelMatrix * vec4(aPos, 1.0));
	Normal = aNormal * NormalMatrix;
	gl_Position = MVP * vec4(aPos, 1.0);
}
