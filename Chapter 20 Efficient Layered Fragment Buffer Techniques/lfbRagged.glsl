/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#extension GL_EXT_gpu_shader4: enable

#define COUNT_USING_BLENDING 0

#if LFB_READONLY
//I would use const here as mentioned in the specs but it segfaults
//NOTE: it is actually faster to read from a samplerBuffer
layout(size1x32) uniform readonly uimageBuffer offsets;
layout(size4x32) uniform readonly imageBuffer data;
#else
layout(size1x32) uniform uimageBuffer offsets;
layout(size4x32) uniform imageBuffer data;
layout(size1x32) uniform uimageBuffer fragIDs;
#endif

uniform int depthOnly;

int fragOffset;
int fragIndex;

#if !LFB_READONLY
void addFragment(vec3 dat, float depth)
{
	ivec2 coord = ivec2(gl_FragCoord.xy);
	fragIndex = coord.y * winSize.x + coord.x;
	if (depthOnly == 1)
	{
		#if !COUNT_USING_BLENDING
		imageAtomicAdd(offsets, fragIndex, 1U);
		#endif
	}
	else
	{
		uint globalOffset = imageAtomicAdd(offsets, fragIndex, 1U);
		imageStore(data, int(globalOffset), vec4(dat, depth));
		
		#if GLOBAL_SORT
		imageStore(fragIDs, int(globalOffset), uvec4(fragIndex));
		#endif
	}
}
#endif

#if LFB_READONLY
void loadFragments(int n)
{
	fragIndex = n;
	fragOffset = 0;
	if (fragIndex > 0)
		fragOffset = int(imageLoad(offsets, fragIndex - 1).r);
	fragCount = int(imageLoad(offsets, fragIndex).r) - fragOffset;
	fragCount = min(fragCount, MAX_FRAGS);
	
	for (int i = 0; i < fragCount; ++i)
		frags[i] = imageLoad(data, fragOffset + i);
}

vec4 blendFragments(int n)
{
	vec4 col = vec4(1,1,1,1);
	fragIndex = n;
	fragOffset = int(imageLoad(offsets, fragIndex).r);
	fragCount = fragOffset - ((fragIndex == 0) ? 0 : int(imageLoad(offsets, fragIndex - 1).r));
	
	fragOffset -= fragCount;
	fragCount = min(fragCount, MAX_FRAGS);
	
	for (int i = 0; i < fragCount; ++i)
	{
		vec4 fragment = imageLoad(data, fragOffset + i);
		vec4 f = vec4(fragment.rgb, 0.2);
		col.rgb = mix(col.rgb, f.rgb, f.a);
	}
	return col;
}
#endif

#if !LFB_READONLY
void writeFragments()
{
	//NOTE: UNTESTED!!!!!!!!!!
	for (int i = 0; i < fragCount; ++i)
		imageStore(data, fragOffset + i, frags[i]);
}
#endif

int iceil(int a, int b)
{
	return (a + b - 1) / b;
}

//this function was created in an attempt to solve the slow local array issue
//essentially an n-way merge of up to 8-size fragment lists
//the speed is almost exactly the same as the standard method
#define CEIL(a, b) (((a) + (b) - 1) / (b))
#define EBMAX CEIL(MAX_FRAGS, 8)
vec4 eightBlend(int n)
{
	//load pixel count and offset
	fragIndex = n;
	fragOffset = 0;
	if (fragIndex > 0)
		fragOffset = int(imageLoad(offsets, fragIndex - 1).r);
	fragCount = int(imageLoad(offsets, fragIndex).r) - fragOffset;
	fragCount = min(fragCount, MAX_FRAGS);
	
	//arrays for n-merge
	int iStart[EBMAX]; //start index into global data
	int iEnd[EBMAX]; //end index into global data
	vec4 next[EBMAX]; //cached data value at iStart
	
	iStart[0] = fragOffset;
	int firstBlock = (iStart[0] / 8) * 8;
	
	//number of blocks to merge
	int blocks = iceil(fragOffset + fragCount - firstBlock, 8);
	
	//populate starts, ends and fill frag cache
	iEnd[0] = firstBlock + 8;
	next[0] = imageLoad(data, iStart[0]);
	for (int i = 1; i < blocks; ++i)
	{
		iStart[i] = iEnd[i-1];
		iEnd[i] = iStart[i] + 8;
		next[i] = imageLoad(data, iStart[i]);
	}
	iEnd[blocks-1] = min(iEnd[blocks-1], fragOffset + fragCount);
	
	//start blending
	vec4 col = vec4(1,1,1,1);
	for (int canary = 0; canary < fragCount; ++canary)
	{
		//find the next maximum-depth fragment
		float dmax = 0.0;
		int idmax;
		for (int i = 0; i < blocks; ++i)
		{
			if (iStart[i] < iEnd[i] && next[i].a > dmax)
			{
				dmax = next[i].a;
				idmax = i;
			}
		}
		
		//blend the fragment with the final colour
		vec4 f = sillyDecode(next[idmax].rg); //extract rgba from rg
		col.rgb = mix(col.rgb, f.rgb, f.a * 0.8);
		
		//increment the block's index and grab the next fragment for the block if it exists
		iStart[idmax] += 1;
		if (iStart[idmax] < iEnd[idmax])
			next[idmax] = imageLoad(data, iStart[idmax]);
		
	}
	return col;
}







