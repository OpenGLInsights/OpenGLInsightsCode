#version 420 core

layout(size1x32) coherent uniform uimage1D 	BokehCountTex;
out vec4 FragColor;

void main()
{
	imageStore(BokehCountTex,1,uvec4(0));
	FragColor = vec4(0,0,0,0);
}
