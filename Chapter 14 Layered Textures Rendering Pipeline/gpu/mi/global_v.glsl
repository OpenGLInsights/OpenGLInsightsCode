#version 150 core

in	vec4	at_vertex;

uniform struct Spatial	{
	vec4 pos,rot;
}s_view;

vec3 trans_for(vec3,Spatial);


vec3 mi_global()	{
	return trans_for( at_vertex.xyz, s_view );
}
