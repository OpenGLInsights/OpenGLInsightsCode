#version 150 core

in	vec4	at_vertex;
out	vec3	pos;


uniform struct Spatial	{
	vec4 pos,rot;
}s_model,s_lit;

vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);


void main()	{
	vec3 vw = trans_for( at_vertex.xyz, s_model );
	pos = trans_inv(vw,s_lit);
}
