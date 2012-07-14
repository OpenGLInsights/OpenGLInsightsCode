#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_model;

uniform samplerBuffer unit_vert, unit_quat;
uniform float vertex_ratio;

void set_base(vec4,vec4);
vec4 qmul(vec4,vec4);
vec3 trans_for(vec3,Spatial);


void main()	{
	int tc = int( (gl_VertexID+0.5) * vertex_ratio );
	vec4 vert = texelFetch(unit_vert,tc);
	vec4 quat = texelFetch(unit_quat,tc);
	vec3 v = trans_for( vert.xyz, s_model );
	vec4 q = qmul( s_model.rot, quat );
	set_base( vec4(v,vert.w), q );
}
