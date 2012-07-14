#version 150 core
#define COMPENSATE

in	vec3 at_prev, at_base;
out	vec3 to_pos, to_speed;

uniform vec4 cur_time;


float update_norm()	{
	vec3 old = to_pos;
	float dist = length(at_base - at_prev);
	vec3 dir = normalize(to_pos - at_base);
	to_pos = at_base + dist * dir;
	#ifdef COMPENSATE
	vec3 add = (to_pos - old) * cur_time.w;
	//float k = dot( add, to_speed ) / max(0.001,length(to_speed));
	to_speed += add;
	#endif
	return 1.0;
}
