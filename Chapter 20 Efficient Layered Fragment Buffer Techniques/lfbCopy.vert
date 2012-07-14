/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

uniform int width;

uniform sampler2D blendCount;
layout(r32ui) writeonly uniform uimageBuffer offsets;

void main()
{
	int index = gl_VertexID;
	ivec2 coord = ivec2(index % width, index / width);
	float count = texelFetch(blendCount, coord, 0).r;
	imageStore(offsets, index, uvec4(uint(count+0.5)));
}
