#version 330 core

#define FRAG_COLOR		0

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

in block
{
	vec2 Texcoord;
} In;

uniform mat4 MVP;
uniform float radius;
uniform float invScrRatio;
uniform vec3 lineColor;

void main()
{
	//just sample the texture
	Color = vec4(lineColor,1.0);	
}

