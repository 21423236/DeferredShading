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

uniform sampler2D uFrameTexture;
uniform float uGamma;
uniform float uExposure;
uniform int uMethod;

out vec4 fragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy / uScene.WindowSize;

	vec3 hdrColor = texture(uFrameTexture, uv).rgb;

	vec3 mapped = vec3(0, 0, 0);
	
	//apply tone-mapping
	if(uMethod == 0)
	{
		mapped = hdrColor * uExposure/(1.0 + hdrColor / uExposure);
		mapped = hdrColor / (hdrColor + vec3(1.0));
	}
	else if(uMethod == 1)
	{
		mapped = vec3(1.0) - exp(-hdrColor * uExposure);
	}

	//apply gamma correction
	mapped = pow(mapped, vec3(1.0/uGamma));

	fragColor = vec4(mapped, 1.0);
}