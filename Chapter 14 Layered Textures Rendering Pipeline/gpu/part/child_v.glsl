#version 150 core

uniform vec4 part_child;

float random(float);
vec3 random_cube(float);
const vec2 rv = vec2(0.143,0.6424);


//xyz=offset, w=scale
vec4 get_child(vec3 dir)	{
	vec2 vid = vec2(gl_InstanceID, gl_VertexID);
	float r0 = random(dot(rv,vid))*2.0 - 1.0;
	float size = dot(part_child.zw, vec2(1.0,r0));

	vec3 cube = random_cube(gl_InstanceID);
	vec3 off = part_child.x * (2.0*cube - vec3(1.0));
	return vec4( cross( normalize(dir),off ), size );
}
