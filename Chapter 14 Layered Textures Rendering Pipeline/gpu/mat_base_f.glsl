#version 150 core

uniform vec4 base_color;

vec4	get_diffuse();
float	get_emissive();

out vec4 rez_color;


void main()	{
	rez_color = base_color + get_emissive() * get_diffuse();
}
