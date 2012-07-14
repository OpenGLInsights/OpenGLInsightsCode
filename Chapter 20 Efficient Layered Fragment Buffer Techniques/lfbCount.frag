/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

layout(r32ui) coherent uniform uimageBuffer counts;

uniform ivec2 winSize;

out vec4 fragColour;

void main()
{
	if (gl_FragCoord.z < 0.0 || gl_FragCoord.z > 1.0)
	{
		discard;
		return;
	}
	
	ivec2 coord = ivec2(gl_FragCoord.xy);
	int index = coord.y * winSize.x + coord.x;
	imageAtomicAdd(counts, index, 1U);
	fragColour = vec4(1.0);
}
