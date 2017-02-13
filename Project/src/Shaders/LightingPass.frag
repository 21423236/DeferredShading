#version 440

struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 intensity;
};

uniform Light uLight;

uniform sampler2D uColor0;
uniform sampler2D uColor1;
uniform sampler2D uColor2;
uniform sampler2D uColor3;

uniform vec2 uWindowSize;

uniform vec3 uEye;

out vec4 fragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy / uWindowSize;

	vec3 P = texture(uColor0, uv).rbg;
	vec3 N = texture(uColor1, uv).rgb;
	vec3 kd = texture(uColor2, uv).rgb;
	vec4 ks = texture(uColor3, uv);

	vec3 V = normalize(uEye - P);
	vec3 L = normalize(uLight.position - P);
	vec3 H = normalize(L + V);

	fragColor = vec4(uLight.ambient * kd + uLight.intensity * (max(dot(N,L), 0) * kd.rgb), 1);
}