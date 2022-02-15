#version 460 core

in vec3 vertex_pos;
in vec3 vertex_color;

out vec3 vertex_color_2;

void main()
{
	gl_Position = vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z, 1.0);
	vertex_color_2 = vertex_color;
}
