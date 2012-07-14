#version 150 core

		in	vec2	at_vertex;
noperspective	out	vec2	tex_coord;

void main()	{
	gl_Position = vec4(at_vertex,0.0,1.0);
	tex_coord = at_vertex*0.5 + vec2(0.5);
}
