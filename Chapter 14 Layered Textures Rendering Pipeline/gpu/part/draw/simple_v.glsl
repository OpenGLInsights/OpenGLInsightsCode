#version 150 core

in vec2 at_sys;
in vec3 at_pos;

void part_draw(vec3,float);

void main()	{
	gl_ClipDistance[0] = at_sys.x;
	part_draw(at_pos,0.1);
}