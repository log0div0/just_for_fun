#version 460 core

in vec2 UV;

out vec4 FragColor;

uniform sampler2D Wood;
uniform sampler2D Lambda;
uniform vec3 ObjectColor;

void main()
{
	float ambient_strengh = 0.1;
	// FragColor = mix(texture(Lambda, UV), texture(Wood, UV), 1 - texture(Lambda, UV).w);
	vec3 ambient = ObjectColor * ambient_strengh;
	FragColor = vec4(ambient, 1.0);
}
