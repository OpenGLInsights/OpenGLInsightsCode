#version 150 core

in	vec4	at_vertex;
in	vec3	at_normal;
out	vec3	to_normal;
out	vec4	to_vertex;

struct Spatial	{
	vec4	pos,rot;
};


vec3 qrot(vec4,vec3);
vec3 trans_for(vec3,Spatial);
Spatial skin_append_all();


void skin_append(float w, Spatial bone)	{
	to_vertex.xyz	+= w * trans_for(at_vertex.xyz, bone);
	to_normal	+= w * qrot(bone.rot, at_normal);
}
Spatial skin_result()	{
	return Spatial( vec4(0.0), vec4(0.0) );
}


void main()	{
	//to_vertex = at_vertex; to_normal = at_normal; return;
	to_vertex = vec4(0.0,0.0,0.0,1.0); to_normal = vec3(0.0);
	skin_append_all();
	to_normal = normalize(to_normal);
}
