#version 440

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_uv;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
	vec4 worldPosition4 = uModelMatrix * vec4(in_position, 1);
	vec4 worldNormal4 = uModelMatrix * vec4(in_normal, 0);
	
	worldPosition = worldPosition4.xyz;
	worldNormal = worldNormal4.xyz;

	gl_Position = uProjectionMatrix * uViewMatrix * worldPosition4;
}