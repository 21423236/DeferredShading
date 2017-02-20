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

struct LightInformation
{
	vec3 position;
	vec3 intensity;
	float radius;
};

layout(std140, binding=1) buffer LightInformationBuffer
{
	LightInformation Lights[];
};


layout(location = 0) in vec3 in_position;

flat out int instanceId;

void main()
{
	instanceId = gl_InstanceID;

	vec3 lightPosition = Lights[instanceId].position;
	float lightRadius = Lights[instanceId].radius;

	mat4 modelMatrix = mat4(vec4(lightRadius, 0, 0, 0), vec4(0, lightRadius, 0, 0), vec4(0, 0, lightRadius, 0), vec4(lightPosition, 1.0));
	gl_Position = uScene.ProjectionMatrix * uScene.ViewMatrix * modelMatrix * vec4(in_position, 1.0f);
}