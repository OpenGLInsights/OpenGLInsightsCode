#version 150 core

in	vec4	at_vertex, at_quat;
in	vec2	at_tex0, at_tex1;

out	vec4	to_vertex, to_quat, to_tex;


void main()	{
	to_vertex = at_vertex;
	to_quat = at_quat;
	to_tex = vec4(at_tex0,at_tex1);
}
