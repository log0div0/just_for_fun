#version 460 core

in vec3 vertex_color_2;

out vec4 fragment_color;

void main()
{
	fragment_color = vec4(vertex_color_2, 1.0);
}
