#version 150 core

uniform vec4 halo_data, cur_time;

in vec2 at_sys;
in vec3 at_pos, at_speed;
out vec2 part_age;

void make_tex_coords();
void part_draw(vec3,float);
vec4 get_child(vec3);


void main()	{
	make_tex_coords();
	gl_ClipDistance[0] = at_sys.x;
	part_age = vec2( cur_time.y - at_sys.x, gl_VertexID );
	vec4 child = get_child(at_speed);
	part_draw( at_pos + child.xyz, halo_data.x * child.w );
}