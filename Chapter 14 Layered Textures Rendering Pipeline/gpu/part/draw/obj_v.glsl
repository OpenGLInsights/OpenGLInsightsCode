#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;

void make_tex_coords();
vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);

in vec2 ghost_sys, ghost_sub;
in vec3 ghost_pos;
in vec4 ghost_rot;
in vec4 at_vertex, at_quat;


void main()	{
	if( ghost_sys.x >= 0.0 )	{
		make_tex_coords();
		Spatial sp = Spatial(
			vec4( ghost_pos, ghost_sub.x ),
			ghost_rot );
		vec3 v = trans_for( at_vertex.xyz, sp );
		vec3 vc = trans_inv(v, s_cam);
		gl_Position = get_proj_cam(vc);
	}else	gl_Position = vec4(0.0,0.0,-2.0,1.0);
}
