#version 440

uniform struct LightInformation
{
	vec3 position;
	vec3 ambient;
	vec3 intensity;
	float radius;
} uLight;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout(location = 0) in vec3 in_position;

void main()
{
	mat4 modelMatrix = mat4(vec4(uLight.radius, 0, 0, 0), vec4(0, uLight.radius, 0, 0), vec4(0, 0, uLight.radius, 0), vec4(uLight.position, 1.0));
	gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(in_position, 1.0f);
}