#version 150 core
// Simplex Noise

uniform sampler1D unit_perm, unit_grad;

float ONE = 1.0 / textureSize(unit_perm,0);
float GRA = 1.0 / textureSize(unit_grad,0);
const float BYTE = 256.0;


//	2D Simplex Noise	//

float get_val2(vec2 pf, vec2 pi)	{
	float gi = texture(unit_perm, pi.x + texture(unit_perm,pi.y).x ).x;
	vec2 grad = texture(unit_grad, (gi*BYTE+0.5)*GRA ).xy * 2.0 - vec2(1.0);
	float t = 0.5 - dot(pf,pf), t2 = t*t;
	return step(0.0,t) * t2*t2 * dot(grad,pf);
}

float snoise(vec2 P)	{
	// (un)skewing factors
	const float F2 = (sqrt(3.0)-1.0)/2.0;
	const float G2 = (3.0-sqrt(3.0))/6.0;

	// Skew the (x,y) space to determine which cell of 2 simplices we're in
	float s = (P.x + P.y) * F2;	// Hairy factor for 2D skewing
	vec2 Pi = floor(P + vec2(s));
	float t = (Pi.x + Pi.y)*G2;	// Hairy factor for unskewing

	vec2 Pf = P - Pi + vec2(t);	// The x,y distances from the cell origin
	vec2 o1 = step( Pf.yx, Pf.xy );	// middle corner offset
	Pi = (Pi+vec2(0.5)) * ONE;	// Integer part, scaled and offset for texture lookup
	
	// simplex origin
	float n0 = get_val2( Pf, Pi );
	// middle corner
	float n1 = get_val2( Pf-o1+vec2(G2), Pi+o1*ONE );
	//last corner
	float n2 = get_val2( Pf-vec2(1.0-2.0*G2), Pi+vec2(ONE) );
	
	// Sum up and scale the result to cover the range [-1,1]
	return 70.0 * (n0 + n1 + n2);
}


//	3D Simplex Noise	//

float get_val3(vec3 pf, vec3 pi)	{
	float ga = texture(unit_perm, pi.z).x;
	float gb = texture(unit_perm, pi.y+ga).x;
	float gi = texture(unit_perm, pi.x+gb).x;

	vec3 grad = texture(unit_grad, (gi*BYTE+0.5)*GRA ).xyz * 2.0 - vec3(1.0);
	float t = 0.6 - dot(pf,pf), t2 = t*t;
	return step(0.0,t) * t2*t2 * dot(grad,pf);
}

float snoise(vec3 P)	{
	// (un)skewing factors
	const float F3 = 1.0/3.0;
	const float G3 = 1.0/6.0;
	const vec3 v1 = vec3(1.0);

	// Skew the (x,y,z) space to determine which cell of 6 simplices we're in
 	float s = dot(v1,P) * F3;	// Factor for 3D skewing
	vec3 Pi = floor(P + s);
	float t = dot(v1,Pi)* G3;

	vec3 Pf = P - Pi + t;		// The x,y distances from the cell origin
	Pi = (Pi+vec3(0.5)) * ONE;	// Integer part, scaled and offset for texture lookup

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	vec3 rc = step( Pf.xyz, Pf.yzx );	//gives all 3 comparison results
	vec3 o1 = rc.zxy * (v1-rc).xyz;		//component is the first
	vec3 o2 = v1 - rc.xyz * (v1-rc).zxy;	//component is not the least

	// simplex origin
	float n0 = get_val3( Pf, Pi );
	// second corner
	float n1 = get_val3( Pf-o1+vec3(G3), Pi+o1*ONE );
	// third corner
	float n2 = get_val3( Pf-o2+vec3(2.0*G3), Pi+o2*ONE );
	// last corner
	float n3 = get_val3( Pf-vec3(1.0-3.0*G3), Pi+vec3(ONE) );

	// Sum up and scale the result to cover the range [-1,1]
	return 32.0 * (n0 + n1 + n2 + n3);
}
