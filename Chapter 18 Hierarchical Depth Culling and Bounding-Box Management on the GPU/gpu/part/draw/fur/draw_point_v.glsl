#version 150 core

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;
uniform vec4 screen_size;


vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);


in vec3 at_prev, at_base, at_pos;
out vec4 color;


void main()	{
	gl_PointSize = 10.0;
	float live = dot(at_base,at_base);
	gl_ClipDistance[0] = step(0.01,live)-0.5;
	
	vec3 vs = mix(at_prev,at_pos,0.5);
	vec3 v = trans_inv(vs,s_cam);
	gl_Position = get_proj_cam(v);
	
	color = vec4(1.0);
}
