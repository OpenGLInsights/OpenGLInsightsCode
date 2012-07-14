#version 150 core
//matrix->quaternion reference:
//www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm

vec4 qmake(mat3 basis)	{
	float W = 0.5 * sqrt(1.0 + basis[0][0] + basis[1][1] + basis[2][2]);
	float S = 0.25 / W;
	float X = basis[2][1] - basis[1][2];
	float Y = basis[0][2] - basis[2][0];
	float Z = basis[1][0] - basis[0][2];
	return vec4( S*vec3(X,Y,Z), W );
}

vec4 qmake_alt(mat3 basis)	{
	const vec2 vs = vec2(1.0,-1.0);
	vec4 q = sqrt( max(vec4(0.0), //rounding errors shield
		vs.xyyx * basis[0][0] + vs.yxyx * basis[1][1] +
		vs.yyxx * basis[2][2] + vec4(1.0) ));
	return 0.5 * q * sign( vec4(
		basis[2][1]-basis[1][2],
		basis[0][2]-basis[2][0],
		basis[1][0]-basis[0][1],
		1.0));
}