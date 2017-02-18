#version 440

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uPosition;
uniform float uScale;

layout(location = 0) in vec3 in_position;

void main()
{
	mat4 modelMatrix = mat4(vec4(uScale, 0, 0, 0), vec4(0, uScale, 0, 0), vec4(0, 0, uScale, 0), vec4(uPosition, 1));
	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(in_position, 1.0f);
}