#version 150 core

uniform sampler2D unit_input;
uniform	vec4	proj_cam, range_cam;

noperspective in vec2 tex_coord;
out vec4 rez_color;

vec3 unproject(vec3,vec4);


void main()	{
	float d = texture(unit_input, tex_coord).r;
	vec3 vc = unproject( vec3(tex_coord,d), proj_cam );
	rez_color = vec4( (vc.z-range_cam.x) * range_cam.z );
}
