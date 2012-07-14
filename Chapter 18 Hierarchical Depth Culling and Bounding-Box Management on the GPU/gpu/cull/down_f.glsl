#version 150 core

uniform sampler2D unit_input;

void main()	{
	ivec2 tc = ivec2(gl_FragCoord.xy * 2.0);
	vec4 d = vec4(texelFetch(unit_input, tc, 0).r,
		texelFetch(unit_input, tc+ivec2(1,0), 0).r,
		texelFetch(unit_input, tc+ivec2(0,1), 0).r,
		texelFetch(unit_input, tc+ivec2(1,1), 0).r);
	gl_FragDepth = max( max(d.x,d.y), max(d.z,d.w) );
}
