/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

#define LFB_READONLY 1
#include "lfb.glsl"

out vec4 fragColour;
uniform float alpha;

void main()
{
#if GLOBAL_SORT

	//lfbRagged only
	ivec2 coord = ivec2(gl_FragCoord.xy);
	int n = coord.x + winSize.x * coord.y;
	fragColour = eightBlend(n);
	return;
	
#else

	ivec2 coord = ivec2(gl_FragCoord.xy);
	loadFragments(coord);
	
	//show depth complexity
	#if 0
	fragColour.rgb = vec3(fragCount/32.0);
	fragColour.a = 1.0;
	return;
	#endif
	
	sortFragments();
	
	//resolve transparency
	fragColour = vec4(1.0);
	for (int i = 0; i < fragCount; ++i)
	{
		vec4 f = vec4(frags[fragCount-i-1].rgb, alpha);
		vec4 col = sillyDecode(f.xy); //extract rgba from rg
		fragColour.rgb = mix(fragColour.rgb, col.rgb, col.a * alpha);
	}
#endif
}
