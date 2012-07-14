#version 150 core

uniform vec4 shape_value;

in vec4 at_pos;
in vec3 at_pos1;
in vec3 at_pos2;
out vec4 to_pos;


void main()	{
	to_pos = vec4(
		shape_value.x * at_pos1 +
		shape_value.y * at_pos2,
		at_pos.w );
}
