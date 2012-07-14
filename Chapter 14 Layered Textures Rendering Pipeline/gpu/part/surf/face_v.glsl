#version 150 core

uniform sampler2D unit_vertex, unit_quat;

struct Spatial	{ vec4 pos,rot; }
get_surface(vec2 tc)	{
	vec4 pos = texture(unit_vertex, tc),
		quat = texture(unit_quat, tc);
	return Spatial( pos, 2.0*quat - vec4(1.0) );
}