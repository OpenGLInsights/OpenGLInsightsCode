#version 150 core

in	vec4	at_vertex;

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;

vec3 dir_world(vec3);
vec3 trans_for(vec3,Spatial);


vec3 mi_reflection()	{
	vec3 vc = trans_for(at_vertex.xyz, s_view);
	vec3 vn = dir_world( vec3(0.0,0.0,1.0) );
	return reflect( -normalize(vc), vn );
}
