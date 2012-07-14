#version 420 core

//-----------------------------------------------------------------------------
// Implementation based on :
//  - http://www.cs.utah.edu/~shirley/papers/sunsky/sunsky.pdf
//-----------------------------------------------------------------------------

uniform float	Turbidity; // [2,10]  Turbidity : 1 = pure aire, 64=thin fog
uniform vec2	SunSphCoord;
uniform float	SunFactor;
uniform bool	DrawSun;

in  vec3		gPosition;
out vec4		FragColor;
//-----------------------------------------------------------------------------
const float     SunRadius   = 0.018f;
const float     SunFalloff  = 0.022f;
const float		M_PI		= 3.14159265f;
const vec2 		A_Y			= vec2( 0.1787, -1.4630);
const vec2 		B_Y			= vec2(-0.3554,  0.4275);
const vec2 		C_Y			= vec2(-0.0227,  5.3251);
const vec2 		D_Y			= vec2( 0.1206, -2.5771);
const vec2 		E_Y 		= vec2(-0.0670,  0.3703);

const vec2 		A_x 		= vec2(-0.0193, -0.2592);
const vec2 		B_x 		= vec2(-0.0665,  0.0008);
const vec2 		C_x 		= vec2(-0.0004,  0.2125);
const vec2 		D_x 		= vec2(-0.0641, -0.8989);
const vec2 		E_x			= vec2(-0.0033,  0.0452);

const vec2 		A_y 		= vec2(-0.0167, -0.2608);
const vec2 		B_y  		= vec2(-0.0950,  0.0092);
const vec2 		C_y 		= vec2(-0.0079,  0.2102);
const vec2 		D_y  		= vec2(-0.0441, -1.6537);
const vec2 		E_y 		= vec2(-0.0109,  0.0529);
//-----------------------------------------------------------------------------
const mat4x3 	Mx 			= mat4x3(	0.00166,    -0.02903,     0.11693,
					                   -0.00375,     0.06377,    -0.21196,
			                    		0.00209,    -0.03203,     0.06052,
					                    0,           0.00394,     0.25886);

const mat4x3 	My			= mat4x3(	 0.00275,    -0.04214,     0.15346,
				                   		-0.00610,     0.08970,    -0.26756, 
				                    	 0.00317,    -0.04153,     0.06670,
				                    	 0,           0.00516,     0.26688);

const mat3 		XYZ2RGB		= mat3(		 3.240479,   -0.969256,    0.055648, 
				                   		-1.53715,     1.875991,   -0.204043,
				                   		-0.49853,     0.041556,    1.057311);

//-----------------------------------------------------------------------------
vec3 ToCartesian(float _theta, float _phi)
{
	float sinTheta = sin(_theta);
	return vec3(sinTheta*cos(_phi),sinTheta*sin(_phi),cos(_theta));
}
//-----------------------------------------------------------------------------
float PerezFunction(float A,
                    float B,
                    float C,
                    float D,
                    float E,
                    float cosTheta,
                    float gamma,
                    float thetaS,
                    float lvz )
{
	float den = (1.0 + A * exp(B)) * (1.0 + C * exp(D * thetaS) + E * cos(thetaS)*cos(thetaS));
	float num = (1.0 + A * exp(B / cosTheta)) * (1.0 + C * exp(D * gamma) + E * cos(gamma)*cos(gamma));
	return (lvz * num / den);
}
//-----------------------------------------------------------------------------
void main()
{
	// Branching is better ?
	if(gPosition.z<0.f)
	{
		// Set a brun color for the ground
		//FragColor = vec4(vec3(0.431372549,0.345098039,0.117647059)*1000,1);
		FragColor = vec4(0,0,0,1);
		return;
	}

	// Correct azimythal angle of sun because of cubemap transformation
	vec3 dir		= normalize(gPosition);
	float cosTheta	= dir.z;
	float gamma		= acos(dot(ToCartesian(SunSphCoord.x,2*M_PI-SunSphCoord.y),dir));
	float thetaS	= SunSphCoord.x;
	float phiS		= 2*M_PI-SunSphCoord.y;

	// Check if direction is in the sun and clamp according to its distance
	// Factor is in [1,5] : 5 in the sun, 1 outside with a smooth transition
	float factor = 1;
	if(DrawSun)
	{
		float w		= (gamma-SunRadius)/(SunFalloff-SunRadius);
		cosTheta	= gamma<SunFalloff?cos(thetaS):cosTheta;
		gamma		= gamma<SunFalloff?0:gamma;
		factor	    = 1.f + SunFactor*smoothstep(0,1,1-w);
	}

	float thetaS2	= thetaS * thetaS;
	float thetaS3	= thetaS * thetaS2;
	vec3  T			= vec3(Turbidity*Turbidity,Turbidity,1);
	vec4  thetas    = vec4(thetaS3,thetaS2,thetaS,1);

	// Compute zenith luminance and convert it from kcd/m^2 to cd/m^2  
	float chi		= (4.0 / 9.0 - Turbidity / 120.0) * (M_PI - 2.0 * thetaS);  
	float zenith_Y  = (4.0453*Turbidity - 4.9710) * tan( chi ) - 0.2155*Turbidity + 2.4192;
	zenith_Y	   *= 1000.f;  

	// Compute chromacity
	float zenith_x  = dot( T, Mx * thetas);
	float zenith_y  = dot( T, My * thetas);

	// Compute variation according to the view direction
	vec2 t			= vec2(Turbidity,1);
	float x 		= PerezFunction(dot(t,A_x),dot(t,B_x),dot(t,C_x),dot(t,D_x),dot(t,E_x),cosTheta,gamma,thetaS,zenith_x);
	float y 		= PerezFunction(dot(t,A_y),dot(t,B_y),dot(t,C_y),dot(t,D_y),dot(t,E_y),cosTheta,gamma,thetaS,zenith_y);

	// Conversion from xyZ to XYZ
	vec3 XYZ;
	XYZ.y 			= abs(PerezFunction(dot(t,A_Y),dot(t,B_Y),dot(t,C_Y),dot(t,D_Y),dot(t,E_Y),cosTheta,gamma,thetaS,zenith_Y))*factor;
	XYZ.x 			= (x / y) * XYZ.y;
	XYZ.z			= ((1.0 - x - y) / y) * XYZ.y;

	// Conversion from XYZ to RGB
//	FragColor = vec4( vec3(1.0) - exp(-(1.0/15000.0) * (XYZ2RGB * XYZ)), 1);
//	FragColor = vec4( vec3(1.0) - exp(-(1.0/8000.0) * (XYZ2RGB * XYZ)), 1);
	FragColor = vec4( (XYZ2RGB * XYZ), 1);
}

