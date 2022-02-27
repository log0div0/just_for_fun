#version 460 core

in vec3 aPos;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 aLightColor;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	Color = aLightColor;
}
