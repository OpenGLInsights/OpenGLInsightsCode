#version 150 core

in	vec4	at_vertex;

uniform struct Spatial	{
	vec4 pos,rot;
}s_model,s_target;

vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);


vec3 mi_object()	{
	vec3 vw = trans_for( at_vertex.xyz, s_model );
	return 0.5*trans_inv(vw,s_target) + vec3(0.5);
}
