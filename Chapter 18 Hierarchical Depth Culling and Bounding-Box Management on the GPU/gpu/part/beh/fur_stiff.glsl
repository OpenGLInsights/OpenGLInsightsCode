#version 150 core
//#define LIMIT
//todo: use physically correct stiffness

uniform vec4 cur_time, fur_system, fur_segment;
uniform float fur_stiff;

in	vec3 at_prev, at_base;
out	vec3 to_pos, to_speed;


float update_stiff()	{
	vec3 newpos = to_pos;// + 0.5*cur_time.x * to_speed;
	vec3 sof = 2.0*at_base - newpos - at_prev;
	float kt = fur_system.x * fur_stiff * cur_time.x;
	#ifdef LIMIT
	float dlen = 1.0 / max(0.001,length(sof));
	float kmax = dlen - cur_time.x * dot(to_speed,sof)*dlen*dlen;
	kt = min(kmax,kt);
	#endif
	//warning! magic is here!
	float kr = mix(10.0,5.0, fur_segment.y*fur_segment.z );
	to_speed += kt *kr* sof;
	return 1.0;
}
