/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#ifndef GLOBAL_SORT
#define GLOBAL_SORT 0
#endif

#ifndef LFB_READONLY
#define LFB_READONLY 0
#endif

#ifndef MAX_FRAGS
#define MAX_FRAGS this_is_defined_by_the_application
#endif

uniform ivec2 winSize;

int fragCount;
vec4 frags[MAX_FRAGS];

//this encode/decode was used to store an alpha channel in addition to rgb + depth
//with proper support for structs in global video memory, this whole process will become much cleaner
//nvidia's NV_shader_buffer_load comes close but in my experience using structs significantly slows performane
vec2 sillyEncode(vec4 v)
{
	return vec2(floor(v.r*256.0) + clamp(v.g*0.9, 0.0, 0.9), floor(v.b*256.0) + clamp(v.a*0.9, 0.0, 0.9));
}
vec4 sillyDecode(vec2 v)
{
	return vec4(floor(v.x)/256.0, fract(v.x)/0.9, floor(v.y)/256.0, fract(v.y)/0.9);
}

void loadFragments(int n); //lfb method must implement this
void loadFragments(ivec2 coord)
{
	int n = coord.y * winSize.x + coord.x;
	n = clamp(n, 0, winSize.x * winSize.y - 1); //shouldnt need to do this
	loadFragments(n);
}

vec4 blendFragments(int n); //lfb method must implement this
vec4 blendFragments(ivec2 coord)
{
	int n = coord.y * winSize.x + coord.x;
	n = clamp(n, 0, winSize.x * winSize.y - 1); //shouldnt need to do this
	return blendFragments(n);
}

#include "sorting.glsl"

#include "LFB_METHOD_H" //Shader::define() will substitute the include string here. thanks, freetype2 for the idea

