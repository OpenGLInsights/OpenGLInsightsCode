#version 150 core

out	vec3 to_pos, to_speed;

uniform vec4 coord_sphere;
uniform float reflect_koef;

float update_bounce_sphere()	{
	vec3 off = to_pos - coord_sphere.xyz;
	float r2 = coord_sphere.w * coord_sphere.w;
	float bad = step( r2, dot(off,off) ) *
		step( 0.0, dot(off,to_speed) );
	to_speed *= mix( 1.0, -reflect_koef, bad );
	return 1.0;
}
