#version 460 core
#ifndef VULKAN
#extension GL_KHR_vulkan_glsl : enable
#endif

layout (location=0) in vec2 UV;
layout (location=1) in vec3 PosWS;
layout (location=2) in vec3 Normal;

layout (location=0) out vec4 FragColor;

layout (set=1, binding=0) uniform sampler2D Wood;
layout (set=1, binding=1) uniform sampler2D Lambda;

layout (set=0, binding=0) uniform Tralala {
	vec3 ObjectColor;
	vec3 LightColor;
	vec3 LightPos;
	vec3 CameraPos;
};

void main()
{
	vec3 ToLight = normalize(LightPos - PosWS);
	vec3 FromLight = -ToLight;
	vec3 ToCamera = normalize(CameraPos - PosWS);
	vec3 ReflectDir = reflect(FromLight, Normal);

	float Ambient = 0.1;
	float Diffuse = max(0.0, dot(ToLight, Normal));
	float SpecularStrength = 0.5;
	float Specular = pow(max(0.0, dot(ToCamera, ReflectDir)), 64) * SpecularStrength;


	vec4 SurfaceColor = vec4(ObjectColor, 1.0);
	SurfaceColor = mix(texture(Lambda, UV), texture(Wood, UV), 1 - texture(Lambda, UV).w);
	FragColor = vec4((Ambient + Diffuse + Specular) * LightColor, 1.0f) * SurfaceColor;
}
