#version 150 core

uniform vec4 cur_time;
uniform float part_spin;

in	vec4 at_rot;
out	vec3 to_speed;
out	vec4 to_rot;


vec4 qvec(vec3,float);
vec4 qmul(vec4,vec4);


void init_rot()	{
	to_rot = vec4(0.0);
}

float reset_rot()	{
	to_rot = vec4(0.0,0.0,0.0,1.0);
	return 1.0;
}

float update_rot()	{
	float angle = cur_time.x * part_spin;
	vec4 cr = qvec( normalize(to_speed), angle );
	to_rot = qmul( at_rot, cr );
	return 1.0;
}
