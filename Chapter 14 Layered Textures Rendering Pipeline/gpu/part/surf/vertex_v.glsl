#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

uniform samplerBuffer unit_vertex, unit_quat;
uniform float num_vertices;

vec4 qmul(vec4,vec4);
vec3 trans_for(vec3,Spatial);


Spatial get_surface(vec2 tc)	{	// textureSize doesn't work
	//ivec2 siz  = ivec2( textureSize(unit_vertex), textureSize(unit_quat) );
	//ivec2 cord = ivec2( tc.x * vec2(siz) );
	int cord = int( tc.x * num_vertices );
	vec3 pos  = texelFetch(unit_vertex, cord).xyz;
	vec4 quat = texelFetch(unit_quat,   cord);
	vec3 v = trans_for( pos, s_model );
	vec4 q = qmul( s_model.rot, quat );
	return Spatial( vec4(v,1.0), q );
}