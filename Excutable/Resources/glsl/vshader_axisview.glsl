#version 460
uniform mediump mat4 matrix;
layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 vTexCoord;

void main(void)
{
	vTexCoord = TexCoord;
	gl_Position = matrix * Position;
};