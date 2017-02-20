#version 440

struct SceneInformation 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	vec2 WindowSize;
	vec3 SceneSize;
	vec3 EyePosition;
};

layout(std140, binding = 0) uniform SceneBlock 
{
	SceneInformation uScene;
};

uniform struct LightInformation
{
	vec3 position;
	vec3 ambient;
	vec3 intensity;
	float radius;
} uLight;

uniform sampler2D uColor0;
uniform sampler2D uColor1;
uniform sampler2D uColor2;
uniform sampler2D uColor3;

out vec4 fragColor;

const float PI   = 3.14159265358979323846f;
const float PI_2 = 1.57079632679489661923f;

float D(vec3 N, vec3 H, float alpha)
{
	return ((alpha + 2.0f)/PI_2)*pow(max(dot(N,H),0.0f), alpha);
}

float G(vec3 L, vec3 H)
{
	return 1.0f / pow(max(dot(L,H),0.0), 2);
}

vec3 F(vec3 Ks, vec3 L, vec3 H)
{
	return Ks + (1.0f-Ks) * pow(1.0f - max(dot(L,H),0.0f), 5);
}

vec3 BRDF(vec3 L, vec3 N, vec3 H, vec3 Ks, vec3 Kd, float alpha)
{
	return (Kd / PI) + D(N, H, alpha) * F(Ks, L, H) * G(L, H) / 4.0f;
}

void main()
{
	vec2 uv = gl_FragCoord.xy / uScene.WindowSize;

	vec4 P = texture(uColor0, uv);
	vec3 N = texture(uColor1, uv).rgb;
	vec3 kd = texture(uColor2, uv).rgb;
	vec4 ks = texture(uColor3, uv);

	vec3 V = normalize(uScene.EyePosition - P.xyz);
	vec3 L = normalize(uLight.position - P.xyz);
	vec3 H = normalize(L + V);

	float lambertian = max(dot(L, N), 0.0);


	float radiusSquared = uLight.radius * uLight.radius;
	vec3 distance = P.xyz - uLight.position;
	float distanceSquared = (distance.x*distance.x + distance.y * distance.y + distance.z * distance.z);
	if(distanceSquared < radiusSquared)
	{
		float attenuation = ((radiusSquared - distanceSquared)/uLight.radius);
		fragColor =  vec4(attenuation * uLight.intensity * lambertian * BRDF(L, N, H, ks.rgb, kd, ks.w), 1);
	}
	else
	{
		fragColor = vec4(0, 0, 0, 1);
	}
}