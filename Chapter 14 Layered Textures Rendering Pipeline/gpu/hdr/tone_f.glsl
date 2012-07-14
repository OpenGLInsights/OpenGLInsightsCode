#version 150 core

uniform sampler2D unit_input;
uniform float exposure;

noperspective in vec2 tex_coord;
out vec4 tone_color;


void main()	{
	vec3 color = texture(unit_input,tex_coord).xyz;
	tone_color = vec4(1.0) - vec4(exp(-exposure*color),0.0);
}
