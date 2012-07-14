#version 150 core

in	vec4 at_vertex,at_quat;


struct Spatial	{
	vec4 pos,rot;
};
struct DualQuat	{
	vec4 re,im;
	float scale;
}dq = DualQuat( vec4(0.0), vec4(0.0), 0.0 );


vec4 qinv(vec4);
vec4 qmul(vec4,vec4);
vec3 trans_for(vec3,Spatial);


void skin_append(float w, Spatial s)	{
	vec4 pos = vec4(0.5 * s.pos.xyz, 0.0);
	dq.re += w * s.rot;
	dq.im += w * qmul(pos,s.rot);
	dq.scale += w * s.pos.w;
}

Spatial normDq()	{
	float k = 1.0 / length(dq.re);
	vec4 tmp = qmul(dq.im, qinv(dq.re));
	tmp *= 2.0*k*k; tmp.w = dq.scale;
	return Spatial( tmp, k * dq.re );
}

Spatial skin_result()	{
	Spatial sp = normDq();
	vec3 v = trans_for( at_vertex.xyz, sp );
	vec4 v4 = vec4( v, at_vertex.w);
	return Spatial( v4, qmul(sp.rot, at_quat) );
}
