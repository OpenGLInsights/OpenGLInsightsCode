#version 150 core

noperspective in vec2 tex_coord;
out vec4 color;

uniform sampler2D unit_velocity;
uniform float radius;

vec4 blur_result();

vec2 blur_vector()	{
	return radius * texture(unit_velocity,tex_coord).xy;
}

void main()	{
	color = blur_result();
}