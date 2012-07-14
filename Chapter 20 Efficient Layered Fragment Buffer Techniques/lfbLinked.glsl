/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#if LFB_READONLY
layout(r32ui) uniform readonly uimageBuffer restrict headPtrs;
layout(r32ui) uniform readonly uimageBuffer restrict nextPtrs;
layout(rgba32f) uniform readonly imageBuffer restrict data;
#else
layout(binding = 0, offset = 0) uniform atomic_uint allocOffset;
layout(r32ui) uniform uimageBuffer restrict headPtrs;
layout(r32ui) uniform uimageBuffer restrict nextPtrs;
layout(rgba32f) uniform imageBuffer restrict data;
#endif

uniform int fragAlloc;

int fragOffset;
int fragIndex;

#if !LFB_READONLY
void addFragment(vec3 dat, float depth)
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	fragIndex = coord.y * winSize.x + coord.x;
	uint nodeAlloc = atomicCounterIncrement(allocOffset);
	if (nodeAlloc < fragAlloc) //don't overflow
	{
		uint currentHead = imageAtomicExchange(headPtrs, fragIndex, nodeAlloc).r;
		imageStore(nextPtrs, int(nodeAlloc), uvec4(currentHead));
		imageStore(data, int(nodeAlloc), vec4(dat, depth));
	}
}
#endif

void loadFragments(int n)
{
	fragIndex = n;
	uint node = imageLoad(headPtrs, fragIndex).r;
	while (node != 0 && fragCount < MAX_FRAGS)
	{
		frags[fragCount++] = imageLoad(data, int(node));
		node = imageLoad(nextPtrs, int(node)).r;
	}
}

vec4 blendFragments(int n)
{
	vec4 col = vec4(1,1,1,1);
	fragIndex = n;
	uint node = imageLoad(headPtrs, fragIndex).r;
	while (node != 0 && fragCount < MAX_FRAGS)
	{
		vec4 fragment = imageLoad(data, int(node));
		vec4 f = vec4(fragment.rgb, 0.2);
		col.rgb = mix(col.rgb, f.rgb, f.a);
		node = imageLoad(nextPtrs, int(node)).r;
	}
	return col;
}

#if !LFB_READONLY
void writeFragments()
{
	//NOTE: UNTESTED!!!!!!!!!!
	int writer = 0;
	uint node = imageLoad(nextPtrs, fragIndex).r;
	while (node != 0 && writer < fragCount)
	{
		imageStore(data, int(node), frags[writer++]);
		node = imageLoad(nextPtrs, int(node)).r;
	}
}
#endif
