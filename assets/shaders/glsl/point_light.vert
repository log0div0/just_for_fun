#version 460 core

layout (location = 0) in vec3 aPos;

layout (binding = 1) uniform Trololo {
	mat4 MVP;
};

void main()
{
	gl_Position = MVP * vec4(aPos, 1.0);
}
