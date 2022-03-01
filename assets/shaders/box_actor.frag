#version 460 core

in vec2 UV;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D Wood;
uniform sampler2D Lambda;
uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPos;

void main()
{
	// ObjectColor = mix(texture(Lambda, UV), texture(Wood, UV), 1 - texture(Lambda, UV).w);

	float Ambient = 0.1;
	vec3 ToLight = normalize(LightPos - FragPos);
	float Diffuse = dot(ToLight, Normal);

	FragColor = vec4((Ambient + Diffuse) * LightColor * ObjectColor, 1.0);
}
