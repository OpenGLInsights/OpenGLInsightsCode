#version 150 core

out	vec4 to_vertex,to_quat;

struct Spatial	{
	vec4 pos,rot;
};

Spatial skin_append_all();
void skin_finish(vec3);

void main()	{
	//to_vertex = at_vertex; to_quat = at_quat; return;
	Spatial sp = skin_append_all();
	to_vertex = sp.pos;
	to_quat = sp.rot;
	skin_finish( to_vertex.xyz );
}