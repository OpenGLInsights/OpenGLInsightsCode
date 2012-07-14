#version 150 core

layout(points) in;
layout(points, max_vertices = 2) out;

in	vec3	position[];
out	vec4	color;


void main()	{
	gl_Position = vec4(-0.5,0.0,0.0,1.0);
	color = vec4(+position[0],1.0);
	EmitVertex();
	gl_Position = vec4(+0.5,0.0,0.0,1.0);
	color = vec4(-position[0],1.0);
	EmitVertex();
}