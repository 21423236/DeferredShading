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

uniform vec3 uPosition;
uniform float uScale;

layout(location = 0) in vec3 in_position;

mat4 billboard(mat3 viewMatrix, vec3 translation)
{
	mat4 transform = mat4(transpose(viewMatrix));
	transform[3] = vec4(translation, 1);
	return transform;
}

void main()
{
	mat4 modelMatrix = billboard(mat3(uScene.ViewMatrix), uPosition) * mat4(vec4(uScale, 0, 0, 0), vec4(0, uScale, 0, 0), vec4(0, 0, uScale, 0), vec4(0, 0, 0, 1));
	gl_Position = uScene.ProjectionMatrix * uScene.ViewMatrix * modelMatrix * vec4(in_position, 1);
}