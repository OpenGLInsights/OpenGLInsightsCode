/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

layout(r32ui) uniform uimageBuffer tex;

void main()
{
	int index = gl_VertexID;
	imageStore(tex, index, uvec4(0));
}
