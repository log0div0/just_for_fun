#version 460 core

layout (location=0) out vec4 FragColor;

layout (set=0, binding=0) uniform Tralala {
	vec3 LightColor;
};

void main()
{
	FragColor = vec4(LightColor, 1.0);
}
