#version 150 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in gl_PerVertex	{
	vec4 gl_Position;
}gl_in[];

in vec2 to_tex[];
in vec4 tog_vertex[], tog_quat[];
out vec4 to_vertex, to_quat;


float kf_uv()	{
	mat3 mv = mat3(
		(tog_vertex[0] - tog_vertex[1]).xyz,
		(tog_vertex[1] - tog_vertex[2]).xyz,
		(tog_vertex[2] - tog_vertex[0]).xyz);
	mat3x2 mt = mat3x2(
		to_tex[0] - to_tex[1],
		to_tex[1] - to_tex[2],
		to_tex[2] - to_tex[0]);
	vec3 div = vec3(
		dot(mv[0],mv[0]), dot(mv[1],mv[1]), dot(mv[2],mv[2])
		) / max( vec3(0.0001), vec3(
		dot(mt[0],mt[0]), dot(mt[1],mt[1]), dot(mt[2],mt[2])
		));
	return dot(vec3(1.0/3.0), div);
}

float kf_id()	{
	return 1.0 + gl_PrimitiveIDIn;
}


void main()	{
	float kf = kf_id();
	for(int i=0; i<gl_in.length(); i++)	{
		to_vertex	= tog_vertex[i];
		to_quat		= tog_quat[i];
		to_vertex.w	*= kf;
		gl_Position	= gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}
