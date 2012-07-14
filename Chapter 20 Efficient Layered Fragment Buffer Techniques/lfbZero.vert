/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

layout(r32ui) uniform writeonly uimageBuffer tozero;

void main()
{
	int index = gl_VertexID;
	imageStore(tozero, index, uvec4(0));
}
