#version 150 core

in	vec4	at_pos, at_rot;
in	vec4	at_low, at_hai;

//pos,rot,low,hai
out	mat4	b;


void main()	{
	b = mat4( at_pos, at_rot, at_low, -at_hai );
}
