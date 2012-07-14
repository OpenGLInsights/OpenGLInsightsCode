/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#version 420

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in int pglyph[];
in ivec2 ppos[];

out vec2 coord;
out vec2 pix;

uniform ivec2 glyphsDim;
uniform ivec2 glyphsSize;
uniform ivec2 texSize;

layout(std140) uniform GlyphInfo
{
	ivec4 glyphInfo[256];
};

uniform mat4 projectionMat;

void main()
{
	//get glyph
	int glyph = pglyph[0];
	#if USE_IMAGE_BUFFER
	ivec4 info = imageLoad(glyphInfo, glyph);
	#else
	ivec4 info = glyphInfo[glyph];
	#endif
	ivec2 topLeft = ppos[0] + info.zw;
	ivec2 size = info.xy;
	
	//calc tex data
	ivec2 glyphPos = ivec2(glyph % glyphsDim.x, glyph / glyphsDim.x)*glyphsSize;
	ivec2 glyphPos2 = glyphPos + size;
	vec2 ca = vec2(glyphPos) / vec2(texSize);
	vec2 cb = vec2(glyphPos2) / vec2(texSize);

#if 0
	coord = vec2(ca.x, ca.y);
	gl_Position = vec4(-1,-1,0,1); EmitVertex();
	coord = vec2(cb.x, ca.y);
	gl_Position = vec4(1,-1,0,1); EmitVertex();
	coord = vec2(ca.x, cb.y);
	gl_Position = vec4(-1,1,0,1); EmitVertex();
	coord = vec2(cb.x, cb.y);
	gl_Position = vec4(1,1,0,1); EmitVertex();
	EndPrimitive();
#endif
	
	//emit quad
	pix = vec2(0, 1);
	coord = vec2(ca.x, cb.y);
	gl_Position = projectionMat * vec4(topLeft, 0, 1);
	EmitVertex();
	
	pix = vec2(0, 0);
	coord = vec2(ca.x, ca.y);
	gl_Position = projectionMat * vec4(topLeft.x, topLeft.y - size.y, 0, 1);
	EmitVertex();
	
	pix = vec2(1, 1);
	coord = vec2(cb.x, cb.y);
	gl_Position = projectionMat * vec4(topLeft.x + size.x, topLeft.y, 0, 1);
	EmitVertex();

	pix = vec2(1, 0);
	coord = vec2(cb.x, ca.y);	
	gl_Position = projectionMat * vec4(topLeft.x + size.x, topLeft.y - size.y, 0, 1);
	EmitVertex();
	EndPrimitive();
}
