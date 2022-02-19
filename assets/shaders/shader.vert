#version 460 core

in vec3 aPos;
in vec3 aColor;
in vec2 aUV;

out vec3 Color;
out vec2 UV;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	Color = aColor;
	UV = aUV;
}
