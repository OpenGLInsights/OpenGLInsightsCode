#version 150 core

in	vec2 at_tex0;
in	vec4 at_vertex, at_quat;
out	vec2 to_tex;

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;


vec3 trans_for(vec3,Spatial);
vec4 qmul(vec4,vec4);
void set_surface(vec4,vec4);


void main()	{
	to_tex = at_tex0;
	vec3 v = trans_for( at_vertex.xyz, s_model );
	vec4 q = vec4(0.5) + 0.5*qmul( s_model.rot, at_quat );
	set_surface( vec4(v,at_vertex.w), q );
	gl_Position = vec4( 2.0*at_tex0 - vec2(1.0), 0.0,1.0 );
}