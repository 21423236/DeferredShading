#version 440

uniform struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 intensity;
	bool isGlobal;
	float radius;
} uLight;

uniform struct ShadowInfo
{
	mat4 shadowMatrix;
	sampler2D shadowTexture;
} uShadow;

uniform sampler2D uColor0;
uniform sampler2D uColor1;
uniform sampler2D uColor2;
uniform sampler2D uColor3;

uniform vec2 uWindowSize;
uniform vec3 uEye;

const mat4 B = mat4(vec4(0.5, 0, 0, 0), vec4(0, 0.5, 0, 0), vec4(0, 0, 0.5, 0), vec4(0.5, 0.5, 0.5, 1));

out vec4 fragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy / uWindowSize;

	vec4 P = texture(uColor0, uv);
	vec3 N = texture(uColor1, uv).rgb;
	vec3 kd = texture(uColor2, uv).rgb;
	vec4 ks = texture(uColor3, uv);

	vec3 V = normalize(uEye - P.xyz);
	vec3 L = normalize(uLight.position - P.xyz);
	vec3 H = normalize(L + V);

	float lambertian = max(dot(L, N), 0.0);
	float specular = 0.0;

	if(lambertian > 0.0)
	{
		float specAngle = max(dot(H, N), 0.0);
		specular = pow(specAngle, ks.a/4.0);
	}

	if(uLight.isGlobal)
	{

		fragColor.xyz = uLight.ambient * kd;
	
		bool inShadow = false;

		vec4 shadowCoord = uShadow.shadowMatrix * P;
		vec2 shadowIndex = shadowCoord.xy / shadowCoord.w;
		
		if(shadowCoord.w > 0 && (shadowIndex.x > 0 && shadowIndex.x < 1) && (shadowIndex.y > 0 && shadowIndex.y < 1))
		{
			float lightDepth = texture(uShadow.shadowTexture, shadowIndex).w;
			float pixelDepth = shadowCoord.w;

			if(lightDepth + 0.0025f < pixelDepth)
				inShadow = true;
		}
	
		if(!inShadow)
			fragColor.xyz += uLight.intensity * (lambertian * kd.rgb + specular * ks.rgb);

	}

	else
	{
		float radiusSquared = uLight.radius * uLight.radius;
		vec3 distance = P.xyz - uLight.position;
		float distanceSquared = (distance.x*distance.x + distance.y * distance.y + distance.z * distance.z);
		if(distanceSquared < radiusSquared)
		{
			fragColor.xyz = uLight.intensity * (lambertian * kd.rgb + specular * ks.rgb);
		}
	}
	
	fragColor.a = 1;
}