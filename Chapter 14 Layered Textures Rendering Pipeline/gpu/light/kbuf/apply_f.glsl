#version 150 core

uniform sampler2DMS unit_dir, unit_color;

// material data
vec4	get_bump();
vec4	get_diffuse();
float	get_emissive();
vec4	get_specular();
float	get_glossiness();


in vec4 tan2cam;	// tangent->camera rotation
in vec4 v_cam;		// vertex in camera space
out vec4 rez_color;

vec3 qrot2(vec4 q, vec3 v)	{
	return v + 2.0*cross(q.xyz, cross(q.xyz,v) + q.w*v);
}


void main()	{
	// camera space normal & reflected vector
	vec3 bump = get_bump().xyz * vec3(v_cam.w,1.0,1.0);
	vec3 normal = qrot2( tan2cam, bump );
	vec3 reflected = reflect( normalize(v_cam.xyz), normal );
	mat2x3 mv = mat2x3(normal,reflected);

	ivec2 itc = ivec2( gl_FragCoord.xy );
	vec4 kd = vec4(0.0), ks = vec4(1.0);
	mat4 color = mat4(0.0);

	for(int i=0; i<4; ++i)	{
		vec3 dir = texelFetch(unit_dir, itc, i).xyz;
		//if(dot(dir,dir) == 0.0) break;
		color[i] = texelFetch(unit_color, itc, i);
		vec2 rez = max( vec2(0.0), dir*mv );
		kd[i] = rez.x;
		ks[i] = rez.y;
	}
	//rez_color = kd; return;
	rez_color = vec4(color[0].x,color[1].x,color[2].x,color[3].x); return;
	// apply glossiness
	ks = pow( ks, vec4(get_glossiness()) );

	rez_color =
		(color*kd + vec4(get_emissive())) * get_diffuse() +
		(color*ks) * get_specular() );
}
