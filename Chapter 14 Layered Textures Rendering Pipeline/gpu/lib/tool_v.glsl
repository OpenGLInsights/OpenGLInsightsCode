#version 150 core

//	perspective and orhto projection	//

vec4 get_projection(vec3 v, vec4 pr, vec3 off)	{
	//float w = -v.z*pr.w, z1 = (v.z+pr.x)*pr.z;
	//return vec4( v.xy, z1*w, w );
	//return vec4( v.xy * pr.xy, v.z*pr.z + pr.w, -v.z);
	float ortho = step( 0.0, pr.w );
	float w = mix( -v.z, 1.0, ortho );
	return vec4( v.xy * pr.xy,
		v.z*pr.z + (1.0-ortho*2.0)*pr.w,
		w) + vec4(off*w,0.0);
}

vec4 get_projection(vec3 v, vec4 pr)	{
	return get_projection(v,pr,vec3(0.0));
}

//perspective depth only
float get_proj_depth(float d, vec4 pr)	{
	return -pr.z - pr.w/d;
}


//	camera and light projection helpers	//

uniform	vec4	proj_cam, area_cam;
uniform	vec4	proj_lit, area_lit;

vec4 get_proj_cam(vec3 v)	{
	return get_projection(v, proj_cam, area_cam.xyz);
}
vec4 get_proj_lit(vec3 v)	{
	return get_projection(v, proj_lit, area_lit.xyz);
}


//	light attenuation in Blender model	//

//x: energy, y :linear, z: quadratic, w: spherical
uniform	vec4	lit_attenu;

//old Blender equation:
//I = E * max(1-r/D,0) / ((1+L*r/D)(1+Q*r*r/D/D))
//new Blender equation:
//I = max(1-r/D,0) / (C + L*D + Q*D^2)
//cheaters Blender equation:
//Q does not follow the half-energy rule

float get_attenuation(float d)	{
	float r = 1.0 + dot( lit_attenu.yz, vec2(d,d*d) );
	float s = max(0.0, 1.0-lit_attenu.w*d );
	return s * lit_attenu.x / r;
}
