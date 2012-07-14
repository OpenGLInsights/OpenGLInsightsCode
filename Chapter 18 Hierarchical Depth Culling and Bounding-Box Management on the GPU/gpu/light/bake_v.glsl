#version 150 core

in	vec4	at_vertex;
out	float	depth;


uniform struct Spatial	{
	vec4 pos,rot;
}s_model,s_lit;
uniform vec4 lit_data, range_lit;

vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);
vec4 get_proj_lit(vec3);


void main()	{
	vec3 vw = trans_for( at_vertex.xyz, s_model );
	vec3 vl = trans_inv(vw,s_lit);
	depth = (vl.z + range_lit.x) * range_lit.z;
	vec4 pos = get_proj_lit(vl);
	gl_Position = vec4(pos.xyz, mix(1.0, pos.w, lit_data.y));
}