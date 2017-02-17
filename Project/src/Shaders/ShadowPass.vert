#version 440

uniform mat4 uShadowMatrix;
uniform mat4 uModelMatrix;

layout(location = 0) in vec3 in_position;

out vec4 position;

void main()
{
	gl_Position = uShadowMatrix * uModelMatrix * vec4(in_position, 1.0);
	position = gl_Position;
}