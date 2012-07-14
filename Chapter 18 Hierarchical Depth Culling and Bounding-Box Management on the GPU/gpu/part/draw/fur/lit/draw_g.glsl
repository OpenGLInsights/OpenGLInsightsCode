#version 150 core
#define TRIS

layout(points) in;
#ifdef TRIS
layout(triangle_strip, max_vertices = 4) out;
#else
layout(line_strip, max_vertices = 2) out;
#endif
//we can produce more lines/triangles that are given,
//performing careful interpolation


uniform struct Spatial	{
	vec4 pos,rot;
}s_cam,s_lit;
uniform vec4 fur_segment;
uniform vec4 proj_cam, range_lit;


in gl_PerVertex	{
	float gl_PointSize;
	float gl_ClipDistance[];
}gl_in[];
in vec4 va[],vb[],vc[];
in vec4 la[],lb[],lc[];
in vec3 v0[],v1[],v2[];

out gl_PerVertex	{
	vec4 gl_Position;
	float gl_ClipDistance[1];
};
out vec4 color, v_shadow, fur_quat;
out vec3 surf_norm, dir_light, dir_view;
out float lit_depth, fur_x;

vec3 fur_tan = normalize(v2[0]-v1[0]);
const float angle_complete = 0.9;


void emit_vertex(int,vec3);	// input 'strand'
vec2 get_fur_thick(vec2);	// meta 'strand'
vec4 qmake_alt(mat3);		// math lib

vec2 get_fur_segment()	{
	return (fur_segment.yy + vec2(0.0,1.0)) * fur_segment.z;
}
vec3 strand_normal(vec2 tan)	{
	return cross( vec3(normalize(tan),0.0),
		vec3(0.0,0.0,1.0) ) * proj_cam.xyz;
}
void setPos(float tc, vec3 wp, vec3 normal)	{	
	vec3 n = normalize( mix(dir_view,normal,angle_complete) );
	fur_quat = qmake_alt( mat3( n, cross(fur_tan,n), fur_tan ));
	dir_light = s_lit.pos.xyz - wp;
	dir_view  = s_cam.pos.xyz - wp;
	lit_depth =  (range_lit.x + dir_light.z) * range_lit.z;
	emit_vertex( 0, vec3(tc,0.0,0.0) );
}


const vec4 c0 = vec4(0.0,0.0,0.0,0.8);
const vec4 c1 = vec4(1.0,1.0,1.0,0.0);

void main()	{
	if(gl_in[0].gl_ClipDistance[0] < 0.0) return;
	gl_ClipDistance[0] = gl_in[0].gl_ClipDistance[0];
	vec2 seg = get_fur_segment();
	vec4 ca = mix( c0,c1, seg.x );
	vec4 cb = mix( c0,c1, seg.y );

	surf_norm = v1[0]-v0[0]; //incorrect!

	#ifdef TRIS
	vec3 n2 = strand_normal(vc[0].xy - vb[0].xy);
	vec3 n1 = strand_normal(vb[0].xy - va[0].xy);
	vec2 thick = get_fur_thick(seg);

	fur_x = 1.0;
	color = ca;
	v_shadow = lb[0];
	gl_Position = vb[0] + n1.xyzz * thick.x;
	setPos( seg.x, v1[0], n1 );
	color = cb;
	v_shadow = lc[0];
	gl_Position = vc[0] + n2.xyzz * thick.y;
	setPos( seg.y, v2[0], n2 );
	fur_x = -1.0;
	color = ca;
	v_shadow = lb[0];
	gl_Position = vb[0] - n1.xyzz * thick.x;
	setPos( seg.x, v1[0], -n1 );
	color = cb;
	v_shadow = lc[0];
	gl_Position = vc[0] - n2.xyzz * thick.y;
	setPos( seg.y, v2[0], -n2 );

	#else
	color = ca;
	v_shadow = lb[0];
	gl_Position = vb[0];
	setPos( seg.x, v1[0] );
	color = cb;
	v_shadow = lc[0];
	gl_Position = vc[0];
	setPos( seg.y, v2[0] );
	#endif	//TRIS
}