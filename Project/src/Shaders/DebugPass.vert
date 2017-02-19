#version 440

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
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
	mat4 modelMatrix = billboard(mat3(uViewMatrix), uPosition) * mat4(vec4(uScale, 0, 0, 0), vec4(0, uScale, 0, 0), vec4(0, 0, uScale, 0), vec4(0, 0, 0, 1));
	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(in_position, 1);
}