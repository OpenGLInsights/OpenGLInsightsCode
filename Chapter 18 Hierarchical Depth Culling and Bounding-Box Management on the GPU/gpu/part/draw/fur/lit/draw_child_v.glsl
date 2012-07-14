#version 150

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam,s_lit;
uniform vec4 screen_size;


vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);
vec4 get_proj_lit(vec3);
vec4 get_child(vec3);

void save_all(vec3 v, out vec4 vcam, out vec4 vlit)	{
	vcam = get_proj_cam(trans_inv( v,s_cam ));
	vlit = get_proj_lit(trans_inv( v,s_lit ));
}


in vec3 at_prev, at_base, at_pos;
in vec3 at_root_prev, at_root_base;
out vec4 va,vb,vc;
out vec4 la,lb,lc;
out vec3 v0,v1,v2;


void main()	{
	float live = dot(at_base,at_base);
	gl_ClipDistance[0] = step(0.01,live)-0.5;
	v0 = at_prev; v1 = at_base; v2 = at_pos;

	vec3 off = get_child(at_root_base-at_root_prev).xyz;

	save_all(at_prev+off,va,la);
	save_all(at_base+off,vb,lb);
	save_all(at_pos	+off,vc,lc);
}
