#version 440

in vec2 uv;

layout(location = 0) out vec4 fragColor;
 
void main() {
	fragColor = vec4(uv, 0, 1);
}