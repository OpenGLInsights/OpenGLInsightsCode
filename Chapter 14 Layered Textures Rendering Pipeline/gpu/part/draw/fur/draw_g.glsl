#version 150 core
//#define TRIS
//#define LINK

layout(points) in;
#ifdef TRIS
layout(triangle_strip, max_vertices = 4) out;
#else
layout(line_strip, max_vertices = 2) out;
#endif
//we can produce more lines/triangles that are given,
//performing careful interpolation

uniform vec4 fur_segment, proj_cam;


in gl_PerVertex	{
	float gl_PointSize;
	float gl_ClipDistance[];
}gl_in[];
in vec4 va[],vb[],vc[];

out gl_PerVertex	{
	vec4 gl_Position;
	float gl_ClipDistance[1];
};
out vec4 color;


void emit_vertex(int,vec3);	// geometry template
vec2 get_fur_thick(vec2);	// meta 'strand'
vec2 get_fur_segment()	{
	return (fur_segment.yy + vec2(0.0,1.0)) * fur_segment.z;
}
vec3 strand_normal(vec2 tan)	{
	return cross( vec3(normalize(tan),0.0),
		vec3(0.0,0.0,1.0) ) * proj_cam.xyz;
}

const vec4 c0 = vec4(0.0,0.0,0.0,0.8);
const vec4 c1 = vec4(1.0,1.0,1.0,0.0);


void main()	{
	if(gl_in[0].gl_ClipDistance[0] < 0.0) return;
	gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
	vec3 seg = vec3( get_fur_segment(), 0.0 );
	vec4 ca = mix( c0,c1, seg.x );
	vec4 cb = mix( c0,c1, seg.y );

	#ifdef TRIS
	vec3 n2 = strand_normal(vc[0].xy - vb[0].xy);
	#ifdef LINK
	vec3 n1 = strand_normal(vb[0].xy - va[0].xy);
	#else
	vec3 n1 = n2;
	#endif	//LINK
	vec2 thick = get_fur_thick( seg.xy );
	color = ca;
	gl_Position = vb[0] + n1.xyzz * thick.x;
	emit_vertex( 0, seg.xzz );
	color = cb;
	gl_Position = vc[0] + n2.xyzz * thick.y;
	emit_vertex( 0, seg.yzz );
	color = ca;
	gl_Position = vb[0] - n1.xyzz * thick.x;
	emit_vertex( 0, seg.xzz );
	color = cb;
	gl_Position = vc[0] - n2.xyzz * thick.y;
	emit_vertex( 0, seg.yzz );
	
	#else
	color = ca;
	gl_Position = vb[0];
	emit_vertex( 0, seg.xzz );
	color = cb;
	gl_Position = vc[0];
	emit_vertex( 0, seg.yzz );
	#endif	//TRIS
}