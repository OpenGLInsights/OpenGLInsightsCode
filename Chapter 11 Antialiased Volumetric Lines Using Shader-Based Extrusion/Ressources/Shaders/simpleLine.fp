#version 330 core

#define FRAG_COLOR		0

in block
{
	vec2 Texcoord;
} In;

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

void main()
{
	Color = vec4(1.0,1.0,1.0,1.0);
}
