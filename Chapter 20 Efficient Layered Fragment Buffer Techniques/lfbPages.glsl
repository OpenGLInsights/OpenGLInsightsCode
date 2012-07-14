/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#extension GL_EXT_shader_image_load_store : enable

#if LFB_READONLY
layout(r32ui) uniform readonly uimageBuffer restrict headPtrs;
layout(r32ui) uniform readonly uimageBuffer restrict nextPtrs;
layout(r32ui) uniform readonly uimageBuffer restrict counts;
layout(rgba32f) uniform readonly imageBuffer restrict data;
#else
layout(binding = 0, offset = 0) uniform atomic_uint allocOffset;
layout(r32ui) coherent uniform uimageBuffer restrict headPtrs;
layout(r32ui) uniform uimageBuffer restrict nextPtrs;
layout(r32ui) coherent uniform uimageBuffer restrict counts;
layout(r32ui) coherent uniform uimageBuffer restrict semaphores;
layout(rgba32f) uniform imageBuffer restrict data;
#endif

uniform int fragAlloc;

#define LFB_PAGE_SIZE 4

int fragOffset;
int fragIndex;

#if !LFB_READONLY
void addFragment(vec3 dat, float depth)
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	fragIndex = coord.y * winSize.x + coord.x;
	
	uint curPage = 0;
	uint pageOffset;
	uint pageIndex;
	int canary = 0;
	bool waiting = true;
	while (waiting && canary++ < 10000)
	{
		//acquire semaphore
		if (imageAtomicExchange(semaphores, fragIndex, 81723U) != 81723U)
		{
			curPage = imageLoad(headPtrs, fragIndex).r;

			//get number of fragments at this pixel
			//ran into stupid errors with imageLoad here. should be fine with
			//memoryBarrier() but it looks like it's not working (yet?)
			//pageIndex = imageLoad(counts, fragIndex).r;
			pageIndex = imageAtomicAdd(counts, fragIndex, 1U); //the alternative

			//get the current page to write to or create one
			pageOffset = pageIndex % LFB_PAGE_SIZE;

			//create a new page if needed
			if (pageOffset == 0)
			{
				int nodeAlloc = int(atomicCounterIncrement(allocOffset));
				if (nodeAlloc * LFB_PAGE_SIZE < fragAlloc)
				{
					imageStore(nextPtrs, nodeAlloc, uvec4(curPage));
					imageStore(headPtrs, fragIndex, uvec4(nodeAlloc));
					curPage = nodeAlloc;
				}
				else
					curPage = 0;
			}

			//since imageAtomicAdd is used earlier, this is not needed
			//if (curPage > 0)
			//	imageStore(counts, fragIndex, uvec4(pageIndex + 1));

			//release semaphore
			waiting = false;
			memoryBarrier();
			imageStore(semaphores, fragIndex, uvec4(0U));
		}
	}

	//write data to the page
	if (curPage > 0)
		imageStore(data, int(curPage * LFB_PAGE_SIZE + pageOffset), vec4(dat, depth));
}
#endif

void loadFragments(int n)
{
	fragIndex = n;
	
	//need frag count as the head page may not be full
	fragCount = int(imageLoad(counts, fragIndex).r);
	
	//i iterates over page offsets
	int i = fragCount;
	
	//if we have hit MAX_FRAGS, we still need to keep the offsets intact (head points to the tail page which may not be full)
	//i % LFB_PAGE_SIZE (the offset) cannot change so instead, drop fragments from the list tail using imin
	fragCount = min(fragCount, MAX_FRAGS);
	int imin = i - fragCount;
	
	//grab the head node and start traversing the list
	int f = 0;
	int node = int(imageLoad(headPtrs, fragIndex).r);
	while (i > imin && node > 0)
	{
		--i;
		int offset = i % LFB_PAGE_SIZE;
		frags[i-imin] = imageLoad(data, node * LFB_PAGE_SIZE + offset);
		if (offset == 0) //hit the end of the page, grab the next one
			node = int(imageLoad(nextPtrs, node).r);
	}
}

#if !LFB_READONLY
void writeFragments()
{
	//NOTE: UNTESTED!!!!!!!!!!
	int allocCount = int(imageLoad(counts, fragIndex).r);
	int i = allocCount;
	allocCount = min(allocCount, fragCount);
	int imin = i - allocCount;
	int node = int(imageLoad(headPtrs, fragIndex).r);
	while (i > imin && node != 0)
	{
		--i;
		int offset = i % LFB_PAGE_SIZE;
		imageStore(data, node * LFB_PAGE_SIZE + offset, frags[i-imin]);
		if (offset == 0)
			node = int(imageLoad(nextPtrs, node).r);
	}
}
#endif
