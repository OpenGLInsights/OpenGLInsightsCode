/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

//although depth testing is off, forcing the early fragment
//test correctly clips triangles intersecting the near/far clipping plane
layout(early_fragment_tests) in;

#include "lfb.glsl"

in vec3 esNorm;
in vec3 esFrag;
in vec3 osFrag;
in vec2 coord;

out vec4 fragColour;

uniform sampler2D texColour;
uniform int textured;

vec3 shadeStrips()
{
	vec3 col;
	float i = floor(osFrag.x * 32.0f);
	col.rgb = (fract(i*0.5f) == 0.0f) ? vec3(0.37f, 0.81f, 0.0f) : vec3(1.0f);
	return col;
}

void main()
{
	float diffuse = abs(dot(normalize(esNorm), -normalize(esFrag)));
	vec4 col;
	if (textured == 0)
		col = vec4(shadeStrips(), 1.0);
	else
		col = texture(texColour, coord);
	col.rgb *= diffuse;
	float depth = gl_FragCoord.z;
	
	//encode rgba into rg. necessary for storing depth + textures with alpha
	vec3 dat = vec3(sillyEncode(col), 1.0);
	addFragment(dat, depth);
	
	fragColour = vec4(1.0); //must be one if blending is used
	//don't discard or the stencil test and blending won't work
}
