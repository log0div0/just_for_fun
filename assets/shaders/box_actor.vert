#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 UV;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

void main()
{
	gl_Position = MVP * vec4(aPos, 1.0);
	UV = aUV;
	FragPos = vec3(ModelMatrix * vec4(aPos, 1.0));
	Normal = aNormal * NormalMatrix;
}
