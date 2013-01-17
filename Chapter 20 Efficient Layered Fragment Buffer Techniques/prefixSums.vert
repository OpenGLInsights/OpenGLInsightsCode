/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

layout(r32ui) uniform uimageBuffer  sums;
layout(r32ui) writeonly uniform uimageBuffer bs;

uniform int stepSize;
uniform int pass;

#define PASS_SUMUP 0
#define PASS_SUMDOWN 1

void main()
{
	int index = gl_VertexID;
	index = (index + 1) * stepSize - 1;
	int otherIndex = index - (stepSize>>1);

	uint a = imageLoad(sums, index).r;
	uint b = imageLoad(sums, otherIndex).r;
	imageStore(sums, index, uvec4(a + b));
	if (pass == PASS_SUMDOWN)
		imageStore(sums, otherIndex, uvec4(a));
}
