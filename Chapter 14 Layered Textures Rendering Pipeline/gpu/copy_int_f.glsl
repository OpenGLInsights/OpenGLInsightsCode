#version 150 core

uniform usampler2D unit_input;
noperspective in vec2 tex_coord;
out vec4 rez_color;

void main()	{
	rez_color = vec4(texture(unit_input, tex_coord).r);
}
