#version 440

struct Material{
	vec3 kd;
	vec3 ks;
	float alpha;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 intensity;
};

uniform Material uMaterial;
uniform Light uLight;

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 fragColor;
 
void main() {

	vec3 N = normalize(worldNormal);
	vec3 L = normalize(uLight.position - worldPosition);

	//fragColor = vec4((worldPosition/2.0f * 0.5f) + 0.5f, 1);
	fragColor = vec4(uLight.ambient * uMaterial.kd + uLight.intensity * (clamp(dot(N, L), 0,1) * uMaterial.kd), 1);
}