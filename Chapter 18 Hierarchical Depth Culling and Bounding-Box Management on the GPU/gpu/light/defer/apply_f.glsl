#version 150 core
#define FAKE

uniform sampler2DArray unit_light;

// material data
vec4	get_bump();
vec4	get_diffuse();
float	get_emissive();
vec4	get_specular();
float	get_glossiness();

// deferred funcs
vec4 get_harmonics(vec3);

vec2 get_light(vec4 dir, vec3 normal, vec3 reflected)	{
	float len = length(dir.xyz);
	vec2 val = get_harmonics(vec3(1.0)).wx;	// harmonic bonds weights
	vec3 d3 = dir.xyz / len;		// normalized direction
	len /= val.y;				// direction actual power
	float kspec = len*val.x / dir.w;	// how much specular is directed

	return vec2( max(0.0,dot(d3,normal)),
		pow( max(0.0,dot(d3,reflected)), get_glossiness() )*kspec
		) * len;
}


//from axis, angle
vec4 qvec2(vec4 axang)	{
	return vec4( axang.xyz * sin(0.5*axang.w), cos(0.5*axang.w) );
}
vec4 qmul2(vec4 a, vec4 b)	{
	return vec4(cross(a.xyz,b.xyz) + a.xyz*b.w + b.xyz*a.w, a.w*b.w - dot(a.xyz,b.xyz));
}
vec3 qrot2(vec4 q, vec3 v)	{
	return v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
}

vec4 get_weighted(vec3 dir, float angle, float w0)	{
	//return get_harmonics(dir);
	float w1 = 0.25 * (1.0-w0);
	vec4 ax = vec4(0.0,1.0,-1.0,angle);
	//this may not be correct!

	return	w0*get_harmonics( dir )
		+ w1 * get_harmonics( qrot2( qvec2(ax.yxxw) ,dir ))
		+ w1 * get_harmonics( qrot2( qvec2(ax.zxxw) ,dir ))
		+ w1 * get_harmonics( qrot2( qvec2(ax.xyxw) ,dir ))
		+ w1 * get_harmonics( qrot2( qvec2(ax.xzxw) ,dir ))
		;
}


in vec2	tc;		// screen-space tex-coord
in vec4 tan2cam;	// tangent->camera rotation
in vec4 v_cam;		// vertex in camera space
out vec4 rez_color;


void main()	{
	vec4 ca = texture( unit_light, vec3(tc,0.0) );
	vec4 cb = texture( unit_light, vec3(tc,1.0) );
	vec4 cc = texture( unit_light, vec3(tc,2.0) );
	mat4 cm = mat4( ca,cb,cc, vec4(0.0,0.0,0.0,1.0) );
	
	// camera space normal
	vec3 bump = get_bump().xyz * vec3(v_cam.w,1.0,1.0);
	vec3 normal = qrot2(tan2cam,bump);
	vec3 reflected = reflect( normalize(v_cam.xyz), normal );

#	ifdef FAKE
	//extracting an average light direction*power for each component
	mat3x2 lit = mat3x2(	// coumns: diffuse, specular
		get_light(ca,normal,reflected),
		get_light(cb,normal,reflected),
		get_light(cc,normal,reflected)
		);
	const vec2 ax = vec2(0.0,1.0);
	rez_color = get_diffuse() *
		(lit[0][0]*ax.yxxx + lit[1][0]*ax.xyxx + lit[2][0]*ax.xxyx + vec4(get_emissive()) ) +
		(lit[0][1]*ax.yxxx + lit[1][1]*ax.xyxx + lit[2][1]*ax.xxyx) * get_specular();
#	else
	const float Pi = 3.1415926;
	vec4 kn = get_weighted(normal, Pi/6.0, 1.0/3.0);
	const float ang = Pi/20.0;
	float w1x = pow( max(0.0,cos(ang)), get_glossiness() );
	float w0 = 1.0 - 4.0*w1x / (1.0 + 4.0*w1x);
	vec4 kr = get_weighted(reflected, ang, w0);
	
	rez_color =
		+ (kn*cm + vec4(get_emissive())) * get_diffuse()
		+ (kr*cm) * get_specular();
#	endif
}
