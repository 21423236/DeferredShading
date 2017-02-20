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

mat4 billboard(mat3 viewMatrix, vec3 translation)
{
	mat4 transform = mat4(transpose(viewMatrix));
	transform[3] = vec4(translation, 1);
	return transform;
}

void main()
{
	float scale = Lights[gl_InstanceID].radius;
	mat4 modelMatrix = billboard(mat3(uScene.ViewMatrix), Lights[gl_InstanceID].position) * mat4(vec4(scale, 0, 0, 0), vec4(0, scale, 0, 0), vec4(0, 0, scale, 0), vec4(0, 0, 0, 1));
	gl_Position = uScene.ProjectionMatrix * uScene.ViewMatrix * modelMatrix * vec4(in_position, 1);
}