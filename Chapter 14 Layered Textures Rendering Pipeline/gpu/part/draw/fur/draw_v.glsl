#version 150

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;
uniform vec4 screen_size;


vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);

vec4 project(vec3 v)	{
	return get_proj_cam( trans_inv(v,s_cam) );
}

in vec3 at_prev, at_base, at_pos;
out vec4 va,vb,vc;


void main()	{
	float live = dot(at_base,at_base);
	gl_ClipDistance[0] = step(0.01,live)-0.5;
	
	va = project(at_prev);
	vb = project(at_base);
	vc = project(at_pos);
}
