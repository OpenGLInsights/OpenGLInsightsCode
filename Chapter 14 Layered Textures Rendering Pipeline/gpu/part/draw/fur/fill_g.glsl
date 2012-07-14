#version 150 core

layout(points) in;
layout(line_strip, max_vertices = 2) out;


in vec4 vb[],vc[];
in vec3 dep[];

out float depth;


void main()	{
	if(dep[0].z < 0.1) return;
	depth = dep[0].x;
	gl_Position = vb[0];
	EmitVertex();
	depth = dep[0].y;
	gl_Position = vc[0];
	EmitVertex();
}