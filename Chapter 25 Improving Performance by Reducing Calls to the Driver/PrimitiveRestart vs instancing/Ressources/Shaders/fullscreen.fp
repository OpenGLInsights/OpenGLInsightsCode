#version 330 core

#define FRAG_COLOR		0

uniform sampler2D texToSample;

in block
{
	vec2 Texcoord;
} In;

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

void main()
{
	Color = texture(texToSample,In.Texcoord);
}
