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


layout(location = 0) in vec3 in_position;

void main()
{
	mat4 modelMatrix = mat4(vec4(uLight.radius, 0, 0, 0), vec4(0, uLight.radius, 0, 0), vec4(0, 0, uLight.radius, 0), vec4(uLight.position, 1.0));
	gl_Position = uScene.ProjectionMatrix * uScene.ViewMatrix * modelMatrix * vec4(in_position, 1.0f);
}