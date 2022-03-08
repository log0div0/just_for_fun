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
uniform vec3 CameraPos;

void main()
{
	vec3 ToLight = normalize(LightPos - FragPos);
	vec3 FromLight = -ToLight;
	vec3 ToCamera = normalize(CameraPos - FragPos);
	vec3 ReflectDir = reflect(FromLight, Normal);

	float Ambient = 0.1;
	float Diffuse = max(0.0, dot(ToLight, Normal));
	float SpecularStrength = 0.5;
	float Specular = pow(max(0.0, dot(ToCamera, ReflectDir)), 64) * SpecularStrength;


	vec4 SurfaceColor = vec4(ObjectColor, 1.0);
	SurfaceColor = mix(texture(Lambda, UV), texture(Wood, UV), 1 - texture(Lambda, UV).w);
	FragColor = vec4((Ambient + Diffuse + Specular) * LightColor, 1.0f) * SurfaceColor;
}
