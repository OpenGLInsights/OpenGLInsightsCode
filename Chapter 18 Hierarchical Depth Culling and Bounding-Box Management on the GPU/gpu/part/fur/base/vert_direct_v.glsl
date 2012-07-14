#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

in vec4 at_vertex, at_quat;

void set_base(vec4,vec4);
vec4 qmul(vec4,vec4);
vec3 trans_for(vec3,Spatial);


void main()	{
	vec3 v = trans_for( at_vertex.xyz, s_model );
	vec4 q = qmul( s_model.rot, at_quat );
	set_base( vec4(v,at_vertex.w), q );
}
