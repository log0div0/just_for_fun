#version 460 core

in vec3 Color;
in vec2 UV;

out vec4 Result;

uniform sampler2D Wood;
uniform sampler2D Lambda;

void main()
{
	Result = mix(texture(Lambda, UV), texture(Wood, UV), 1 - texture(Lambda, UV).w);
}
