#version 150 core

uniform sampler2D unit_input;
uniform int	min_level;

uniform struct Spatial	{
	vec4 pos,rot;
}s_cam;	

in	vec4	at_pos, at_rot;
in	vec4	at_low, at_hai;
out	int	to_visible;

//we can not use 's_view' because there is no current entity
Spatial s_model = Spatial(at_pos,at_rot);

vec3 trans_for(vec3,Spatial);
vec3 trans_inv(vec3,Spatial);
vec4 get_proj_cam(vec3);


//	transform local coordinate into camera NDC
vec3 to_ndc(vec3 v)	{
	vec3 vw = trans_for(v,s_model);
	vec3 vc = trans_inv(vw,s_cam);
	vec4 p = get_proj_cam(vc);
	return (vec3(1.0) + p.xyz/p.w) * 0.5;
}

//	compute LOD containing our box in the neighbour 2x2 pixels
int get_lod(vec4 bounds)	{
	// first estimation
	vec2 view = vec2( textureSize(unit_input,0) );
	vec2 area = bounds.zw - bounds.xy;
	float square = dot(view,view) * dot(area,area);
	float flod = ceil(0.5*log2(square))-1.0;
	// move to the next level is that's not enough
	vec4 addr = floor(bounds * view.xyxy * exp2(-flod));
	vec2 diff = addr.zw - addr.xy;
	flod += step(3.0, dot(diff,diff));
	return int(flod);
}


const vec2 one = vec2(0.0,1.0);
const vec3 mixer[] = vec3[8](
	one.xxx, one.xxy, one.xyx, one.xyy,
	one.yxx, one.yxy, one.yyx, one.yyy
);


void main()	{
	// get NDC bounding box
	vec3 xmin = vec3(1.0), xmax = vec3(0.0);
	for(int i=0; i<8; ++i)	{
		vec3 pw = mix( at_low.xyz, -at_hai.xyz, mixer[i] );
		vec3 pc = to_ndc(pw);
		xmin = min(xmin,pc);
		xmax = max(xmax,pc);
	}
	// compute LOD
	vec4 xm = vec4( max(one.xx,xmin.xy), min(one.yy,xmax.xy) );
	int lod = get_lod(xm);

	float maxDepth = 0.0;
	if (lod>=min_level)	{
		// get samples finally
		vec4 sam = vec4(
			textureLod( unit_input, xm.xy, lod ).x,
			textureLod( unit_input, xm.xw, lod ).x,
			textureLod( unit_input, xm.zy, lod ).x,
			textureLod( unit_input, xm.zw, lod ).x);
		// compute max depth
		maxDepth = max( max(sam.x,sam.w), max(sam.y,sam.z) );
	} //derive visibility
	to_visible = int(xmin.z < maxDepth);
}
