#version 440

in vec4 position;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(position.w, position.w, position.w, position.w);
}