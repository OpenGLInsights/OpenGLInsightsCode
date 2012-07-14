#version 150 core

layout(points) in;
layout(points, max_vertices = 1) out;

in gl_PerVertex	{
	vec4 gl_Position;
}gl_in[];
in float dep[];

out gl_PerVertex	{
	vec4 gl_Position;
};
out float depth;


void main()	{
	depth = dep[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
}