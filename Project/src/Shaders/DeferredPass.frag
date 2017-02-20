#version 440

struct Material 
{
	vec3 kd;
	vec3 ks;
	float alpha;

	bool hasDiffuseMap;
	bool hasNormalMap;
	bool hasSpecularMap;

	sampler2D diffuseMap;
	sampler2D normalMap;
	sampler2D specularMap;
};

uniform Material uMaterial;

in DeferredData
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec2 uv;
} inData;

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;

void main()
{
	color0 = vec4(inData.position, 1);

	vec3 N = normalize(inData.normal);
	if(uMaterial.hasNormalMap)
	{
		vec3 T = normalize(inData.tangent);
		vec3 B = normalize(cross(T, N));
		vec3 normalMap = texture(uMaterial.normalMap, inData.uv).xyz * 2.0f - vec3(1, 1, 1);
		color1 = vec4(normalMap.x * T + normalMap.y * B + normalMap.z * N, 1);
	}
	else
	{
		color1 = vec4(N, 1);
	}
	
	color2 = vec4(uMaterial.kd, 1);
	if(uMaterial.hasDiffuseMap)
	{
		color2 *= texture(uMaterial.diffuseMap, inData.uv);
	}

	color3 = vec4(uMaterial.ks, uMaterial.alpha);
	if(uMaterial.hasSpecularMap)
		color3.rgb *= texture(uMaterial.specularMap, inData.uv).rgb;
}