#version 150 core


vec4 qmul(vec4,vec4);
vec3 qrot(vec4,vec3);

in	vec4	at_vertex, at_quat;
in	vec3	at_normal;

uniform	int	has_data;


vec3	make_normal(vec4 rot)	{
	if((has_data & 2) != 0)
		return qrot( rot, at_normal );
	if((has_data & 4) != 0)	{
		vec4 quat = qmul(rot,at_quat);
		vec3 norm = qrot( quat, vec3(0.0,0.0,1.0) );
		return norm * vec3(at_vertex.w,1.0,1.0);
	}
	return vec3(0.0,0.0,1.0);
}