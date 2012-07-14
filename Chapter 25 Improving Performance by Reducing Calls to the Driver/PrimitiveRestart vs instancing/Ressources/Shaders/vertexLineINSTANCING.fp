#version 330 core

#define FRAG_COLOR		0

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

in block
{
	vec2 Texcoord;
	flat vec3 lineColor3;
} In;

uniform mat4 MVP;
uniform float radius;

void main()
{
	//just sample the texture
	Color = vec4(In.lineColor3,1.0);	
}

