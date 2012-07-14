#version 150 core

uniform vec4 cur_time, gravity;

out	vec3 to_speed;


float update_grav()	{
	to_speed += cur_time.x * gravity.xyz;
	return 1.0;
}
