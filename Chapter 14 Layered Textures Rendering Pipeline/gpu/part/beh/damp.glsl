#version 150 core
//proven to be frame-independent

uniform vec4 cur_time;
uniform float speed_damp;

out	vec3 to_speed;

const float kt = -7.0;

float update_damp()	{
	to_speed *= exp( kt*speed_damp*cur_time.x );
	return 1.0;
}
