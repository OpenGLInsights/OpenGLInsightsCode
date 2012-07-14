#version 150 core

uniform vec4 cur_time;

out	vec3 to_pos, to_speed;


float update_main()	{
	to_pos += cur_time.x * to_speed;
	return 1.0;
}