#version 460 core

uniform mat4 MVP;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 UV;
out vec3 PosWS;
out vec3 Normal;

void main()
{
	UV = aUV;
	PosWS = vec3(ModelMatrix * vec4(aPos, 1.0));
	Normal = aNormal * NormalMatrix;
	gl_Position = MVP * vec4(aPos, 1.0);
}
