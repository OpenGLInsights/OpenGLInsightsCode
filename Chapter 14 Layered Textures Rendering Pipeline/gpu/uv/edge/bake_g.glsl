#version 150 core
layout(triangles) in;
layout(points) out;

in	vec4	to_vertex[], to_quat[];
out	vec4	rez_vertex, rez_quat;

void main()	{
	for(int i=0; i<3; i++)	{
		rez_vertex	= to_vertex[i];
		rez_quat	= to_quat[i];
		EmitVertex();
	}
}
