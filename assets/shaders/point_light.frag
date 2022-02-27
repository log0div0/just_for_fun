#version 460 core

in vec3 Color;

out vec4 Result;

void main()
{
	Result = vec4(Color, 1.0);
}
