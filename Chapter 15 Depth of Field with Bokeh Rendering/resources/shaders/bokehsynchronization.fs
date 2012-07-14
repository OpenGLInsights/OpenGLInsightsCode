#version 420 core

layout(binding = 0, offset = 0) uniform atomic_uint BokehCounter;
layout(size1x32) writeonly uniform uimage1D IndirectBufferTex;
out vec4 FragColor;

void main()
{
	imageStore(IndirectBufferTex,1,uvec4(atomicCounter(BokehCounter),0,0,0));
	FragColor = vec4(0,0,0,0);
}
