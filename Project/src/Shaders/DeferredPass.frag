#version 440

struct Material 
{
	vec3 kd;
	vec3 ks;
	float alpha;
};

uniform Material uMaterial;

in DeferredData
{
	vec3 position;
	vec3 normal;
	vec2 uv;
} inData;

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;

void main()
{
	color0 = vec4(inData.position, 0);
	color1 = vec4(normalize(inData.normal), 0);
	color2 = vec4(uMaterial.kd, 0);
	color3 = vec4(uMaterial.ks, uMaterial.alpha);
}