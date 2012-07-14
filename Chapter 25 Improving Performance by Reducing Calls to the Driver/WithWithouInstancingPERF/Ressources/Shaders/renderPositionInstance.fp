#version 330 core

#define FRAG_COLOR		0

layout(location = FRAG_COLOR, index = 0) out vec4 ColorOut;

void main()
{
	ColorOut = vec4(1.0);	
}

