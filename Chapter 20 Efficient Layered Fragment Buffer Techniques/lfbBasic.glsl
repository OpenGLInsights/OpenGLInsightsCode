/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#if LFB_READONLY
layout(r32ui) uniform readonly uimageBuffer restrict counts;
layout(rgba32f) uniform readonly imageBuffer restrict data;
#else
layout(r32ui) coherent uniform uimageBuffer restrict counts;
layout(rgba32f) uniform imageBuffer restrict data;
#endif

uniform int dataDepth;

int fragOffset;
int fragIndex;

//which order to index 3D fragment data (slabs of layers or blocks of pixel lists)
//generally faster to keep pixel data together (eg blocks of pixel lists)

#if !LFB_READONLY
void addFragment(vec3 dat, float depth)
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	fragIndex = coord.y * winSize.x + coord.x;
	uint pixelCounter = imageAtomicAdd(counts, fragIndex, 1U);
	if (pixelCounter < dataDepth)
	{
		uint globalOffset = fragIndex * dataDepth + pixelCounter;
		imageStore(data, int(globalOffset), vec4(dat, depth));
	}
}
#endif

void loadFragments(int n)
{
	fragIndex = n;
	fragCount = int(imageLoad(counts, fragIndex).r);
	fragCount = min(fragCount, min(MAX_FRAGS, dataDepth));
	int globalOffset = fragIndex * dataDepth;
	for (int i = 0; i < fragCount; ++i)
		frags[i] = imageLoad(data, globalOffset + i);
}

#if !LFB_READONLY
void writeFragments()
{
	//NOTE: UNTESTED!!!!!!!!!!
	//int globalOffset = fragIndex * dataDepth;
	//for (int i = 0; i < fragCount; ++i)
	//	imageStore(data, globalOffset + i, frags[i]);
}
#endif

