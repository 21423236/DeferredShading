#version 440

uniform struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 intensity;
	bool isGlobal;
	float radius;
} uLight;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;

layout(location = 0) in vec3 in_position;

void main()
{
	if(uLight.isGlobal)
		gl_Position = vec4(in_position.xy, 0, 1);
	else
	{
		mat4 modelMatrix = mat4(vec4(uLight.radius, 0, 0, 0), vec4(0, uLight.radius, 0, 0), vec4(0, 0, uLight.radius, 0), vec4(uLight.position, 1.0));
		gl_Position = uProjectionMatrix * uViewMatrix * modelMatrix * vec4(in_position, 1.0f);
	}
}