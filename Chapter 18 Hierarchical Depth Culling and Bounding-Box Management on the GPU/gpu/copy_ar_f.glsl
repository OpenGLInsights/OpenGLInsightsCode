#version 150 core

uniform sampler2DArray unit_input;
uniform float layer;

noperspective in vec2 tex_coord;
out vec4 rez_color;

void main()	{
	rez_color = texture( unit_input, vec3(tex_coord,layer) );
	//float dep = texture(unit_input, vec3(tex_coord,layer)).r;
	//rez_color = vec4( pow(dep,50.0) );
}
