#version 150 core

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in	vec4	pl[],pc[],pr[];
out	vec4	center, mask;

void emit_poly(mat4 cd)	{
	mask = cd[3];
	for(int i=0; i<3; ++i)	{
		gl_Position = cd[i];
		center = pc[i];
		EmitVertex();
	}
	EndPrimitive();
}

uniform	vec4	mask_lef, mask_rit;


void main()	{
	//ATI behaves para-normal when trying to pass vec4[] as a function parameter
	//workaround: encoding in matrix container together with the mask
	emit_poly(mat4( pl[0],pl[1],pl[2], mask_lef ));
	emit_poly(mat4( pr[0],pr[1],pr[2], mask_rit ));
	//emit_poly(mat4( pc[0],pc[1],pc[2], vec4(1.0) ));
}