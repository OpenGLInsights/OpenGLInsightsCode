#version 150 core

in	vec4	at_vertex, at_quat;
in	vec3	at_normal;
out	vec4	to_vertex, to_quat;
out	vec3	to_normal;


struct Spatial	{
	vec4 pos,rot;
};

vec4 qmul(vec4,vec4);
vec3 qrot(vec4,vec3);
vec3 trans_for(vec3,Spatial);

Spatial	skin_append_all();


uniform	int	has_data;	//vertex,normal,quaternion
vec3	pos = vec3(0.0,0.0,0.0), normal = vec3(0.0,0.0,0.0);
vec4	quat = vec4(0.0,0.0,0.0,0.0);

void skin_append(float w, Spatial bone)	{
	pos	+= w * trans_for(at_vertex.xyz, bone);
	if((has_data & 2)!=0)
		normal	+= w * qrot(bone.rot, at_normal);
	if((has_data & 4)!=0)
		quat	+= w * qmul(bone.rot, at_quat);
}
Spatial skin_result()	{
	return Spatial( vec4(0.0), vec4(0.0) );
}


void main()	{
	//to_vertex = at_vertex; to_normal = at_normal; to_quat = at_quat; return;
	skin_append_all();
	if((has_data & 2)==0)	normal.z += 1.0;
	if((has_data & 4)==0)	quat.w += 1.0;
	to_vertex = vec4( pos, at_vertex.w );
	to_normal	= normalize(normal);
	to_quat		= normalize(quat);
}