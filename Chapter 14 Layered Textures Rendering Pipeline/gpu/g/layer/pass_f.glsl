#version 150 core

uniform	sampler2D	unit_texture;
uniform vec4		user_color;

out	vec4	c_diffuse;
out	vec4	c_specular;

vec4	tc_unit();
vec2	make_offset(vec2);
const	vec3	luminance = vec3(0.3,0.5,0.2);

void main()	{
	vec4	tc4	= tc_unit();
	vec2	tc	= make_offset( tc4.xy );
	vec4	value	= texture( unit_texture, tc );
	if (value.w<0.01)	discard;

	float	single	= dot(value.xyz,luminance);
	vec3	alt	= single * user_color.xyz;
	vec3	color	= mix( value.xyz, alt, user_color.w );
	c_diffuse = c_specular = vec4( color, value.w );
	//warning: alpha data modification is not supported,
	//because alpha is used for blending. Looking for a workaround
}
