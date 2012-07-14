#version 150 core

uniform	sampler2D	unit_lef;
uniform	sampler2D	unit_rit;

uniform	vec4	mask_lef, mask_rit;

noperspective	in	vec2	tex_coord;
		out	vec4	rez_color;


void main()	{
	vec4 cl = texture(unit_lef,tex_coord);
	vec4 cr = texture(unit_rit,tex_coord);
	rez_color = cl*mask_lef + cr*mask_rit;
}
