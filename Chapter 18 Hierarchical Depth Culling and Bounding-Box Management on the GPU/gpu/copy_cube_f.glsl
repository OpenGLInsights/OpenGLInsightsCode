#version 150 core

uniform samplerCube unit_input;
uniform float layer;

noperspective in vec2 tex_coord;
out vec4 rez_color;

void main()	{
	//rez_color = texture(unit_input, vec3(tex_coord,layer));
	vec3 tc = vec3(tex_coord*2.0 - vec2(1.0), 1.0);
	float dep = texture(unit_input,tc).r;
	rez_color = vec4( pow(dep,50.0) );
}
