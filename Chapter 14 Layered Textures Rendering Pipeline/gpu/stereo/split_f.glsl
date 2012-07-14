#version 150 core

uniform	sampler2D	unit_depth;
uniform	sampler2D	unit_color;

in	vec4	center, mask;
out	vec4	rez_color;


void main()	{
	//normalize into [0,1]
	vec2 tc = 0.5*(center.xy/center.w + vec2(1.0));
	float depth	= texture(unit_depth,tc).r;
	if (depth<gl_FragCoord.z)	discard;
	vec4 color	= texture(unit_color,tc);
	rez_color = color * mask;
}
