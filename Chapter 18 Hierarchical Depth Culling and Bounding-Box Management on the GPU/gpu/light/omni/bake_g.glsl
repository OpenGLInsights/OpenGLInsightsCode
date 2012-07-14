#version 150 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform	vec4	proj_lit;

in	vec3	pos[];

vec3 qrot2(vec4 q, vec3 v)	{
	return v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
}
vec4 project_lit(vec3 v)	{
	return vec4( v.xy, v.z*proj_lit.z + proj_lit.w, -v.z);
}

void triMake(int lid, vec4 q)	{
	gl_Layer = lid;
	for(int i=0; i<3; ++i)	{
		vec3 v = qrot2(q, pos[i]);
		gl_Position = project_lit(v);
		EmitVertex();
	}
	EndPrimitive();
}

void main()	{
	vec3 v = vec3(1.0,-1.0,0.0);
	triMake(0, v.zzzx );
	triMake(1, v.zzzy );
	triMake(2, v.xzzz );
	triMake(3, v.yzzz );
	triMake(4, v.zxzz );
	triMake(5, v.zyzz );
}