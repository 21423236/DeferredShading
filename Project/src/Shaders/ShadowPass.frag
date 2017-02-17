#version 440

in vec4 position;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = position;
}