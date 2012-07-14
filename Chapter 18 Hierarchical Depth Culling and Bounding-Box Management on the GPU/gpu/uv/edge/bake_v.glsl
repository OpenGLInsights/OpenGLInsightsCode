#version 150 core

in	vec4 at_vertex, at_quat;
out	vec4 to_vertex, to_quat;

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;


vec3 trans_for(vec3,Spatial);
vec4 qmul(vec4,vec4);
void set_surface(vec4,vec4);


void main()	{
	to_vertex = trans_for( at_vertex.xyz, s_model );
	to_quat = vec4(0.5) + 0.5*qmul( s_model.rot, at_quat );
}