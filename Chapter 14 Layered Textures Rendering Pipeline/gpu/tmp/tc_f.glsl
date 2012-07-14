#version 150 core

/*	Dictionary:
v,g,f: vertex/geometry/fragment map-ins
p: parallax affected map-ins
t,T: target meta -> map-in
*/


in vec3 mr_%v;	vec3 tr_%v = mr_%v;
in vec3 mr_%g;	vec3 tr_%g = mr_%g;

vec3 mi_%f();	vec3 tr_%f = mi_%f();


void apply_tex_offset(vec3 off)	{
	tr_%p += off;
}

uniform vec4 offset_%t, scale_%t;

vec4 tc_%t()	{ return offset_%t + vec4(0.5) + scale_%t * vec4(tr_%T-vec3(0.5),1.0); }
