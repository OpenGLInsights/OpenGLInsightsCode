#version 150 core

in vec4 at_vertex, at_quat;


struct Spatial	{
	vec4 pos,rot;
}s = Spatial( vec4(0.0), vec4(0.0) );


vec4 qmul(vec4,vec4);
vec3 trans_for(vec3,Spatial);


void skin_append(float w, Spatial bone)	{
	s.pos.xyz += w * trans_for(at_vertex.xyz, bone);
	s.rot += w * qmul(bone.rot, at_quat);
}
Spatial skin_result()	{
	s.pos.w = at_vertex.w;
	return s;
}
