#version 440

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_uv;

out vec2 uv;

const mat4 modelMatrix = mat4(vec4(10, 0, 0, 0), vec4(0, 10, 0, 0), vec4(0, 0, 10, 0), vec4(0, 0, 0, 1));

void main()
{
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(in_position, 1);
	uv = (0.5f * in_normal.xy) + 0.5f;
}