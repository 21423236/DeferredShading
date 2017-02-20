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

uniform vec3 uAmbientIntensity;
uniform sampler2D uColor2;

out vec4 fragColor;

void main()
{
	vec2 uv = gl_FragCoord.xy / uScene.WindowSize;

	fragColor = vec4(uAmbientIntensity * texture(uColor2, uv).rgb, 1.0);
}
