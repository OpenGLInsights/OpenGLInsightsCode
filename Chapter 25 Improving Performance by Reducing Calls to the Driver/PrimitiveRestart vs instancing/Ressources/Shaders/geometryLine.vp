#version 330 core

#define POSITION		0
#define TEXCOORD		1

layout(location = POSITION) in vec3 Position;
layout(location = TEXCOORD) in vec2 Texcoord;

/*out block
{
//	vec4 Position;
} Out;*/

uniform mat4 mvMat;
uniform mat4 pMat;
uniform float radius;
uniform sampler2D gradientTexture;

void main()
{
	gl_Position = mvMat*vec4(Position, 1.0);
}
