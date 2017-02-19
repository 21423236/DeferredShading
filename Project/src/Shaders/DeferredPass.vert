#version 440 core

struct SceneInformation 
{
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	vec2 WindowSize;
	vec3 SceneSize;
};

layout(std140, binding = 0) uniform SceneBlock 
{
	SceneInformation uScene;
};

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_uv;

out DeferredData
{
	vec3 position;
	vec3 normal;
	vec2 uv;
} outData;

void main()
{
	vec4 worldPosition = uModelMatrix * vec4(in_position, 1.0);
	vec4 worldNormal = uModelMatrix * vec4(in_normal, 0.0);

	outData.position = worldPosition.xyz;
	outData.normal = worldNormal.xyz;
	outData.uv = in_uv;
	gl_Position = uScene.ProjectionMatrix * uScene.ViewMatrix * worldPosition;
}