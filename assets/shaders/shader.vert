#version 460 core

in vec3 aPos;
in vec3 aColor;
in vec2 aUV;

out vec3 Color;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Color = aColor;
	UV = aUV;
}
