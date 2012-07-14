#version 150 core

uniform sampler2D unit_light;
uniform vec4 dark;
uniform float k_dark;
// dark.pre, dark.post, low_power, texel_offset


float get_accum(vec3 coord)	{
	return texture(unit_light,coord.xy).r;
}

float get_shadow(vec4 coord)	{
	float occluder = get_accum(coord.xyz);
	//return step(coord.z,occluder);
	float z1 = 1.0+log2(occluder)/k_dark, x = coord.z;
	float z0 = z1 + dark.z/dark.x, z2 = z0 + dark.z/dark.y;
	float R = mix(dark.x*(z1-x), dark.y*(x-z2), step(z0,x));
	return exp2( min(0.0,R) );
	float shadow = occluder * exp2( k_dark*(1.0-coord.z) );
	return clamp(shadow, 0.0, 1.0);
}
