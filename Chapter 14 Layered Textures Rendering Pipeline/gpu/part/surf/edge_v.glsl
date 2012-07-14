#version 150 core

uniform samplerBuffer unit_edge;
uniform float num_segments;

struct Spatial	{
	vec4 pos,rot;
};


Spatial get_surface(vec2 tc)	{
	int cord = int( tc.x * num_segments );
	cord ^= cord&1;
	vec4 pos  = texelFetch(unit_edge, cord+0);
	vec4 quat = texelFetch(unit_edge, cord+1);
	return Spatial( pos, quat );
}