#version 150 core

uniform sampler2D	unit_g0;

noperspective in vec2 tex_coord;
out vec4 rez_color;

void main()	{
	vec4 diff = texture( unit_g0, tex_coord );
	rez_color = diff.w * diff;
}