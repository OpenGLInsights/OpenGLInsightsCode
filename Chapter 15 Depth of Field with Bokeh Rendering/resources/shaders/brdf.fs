//------------------------------------------------------------------------------
// Constants
//  1/(2pi) = 0.159154943f
//  1/pi    = 0.3183098861f
//------------------------------------------------------------------------------
// Cook-Torrance BRDF
float CookBRDF(	in vec3 _viewDir,
				in vec3 _lightDir,
				in vec3 _normal,
				in float _roughness,
				in float _specularity)
{
	vec3  h			= normalize(_viewDir+_lightDir);
	float VdotH 	= max(0.0f,dot(_viewDir,h));
	float NdotH 	= max(0.0f,dot(_normal,h));
	float NdotL 	= max(0.0f,dot(_normal,_lightDir));
	float NdotV 	= max(0.0f,dot(_normal,_viewDir));
	float sNdotH	= sqrt(1.f-NdotH*NdotH);

	// Use Schlick approximation for Fresnel
	// Use Kelemen and Szirmau-Kalos apprixmation for the geometric term
	float F0		= 0.1f;
	float F			= F0 + (1.f-F0) * pow(1.f - VdotH,5.f);
	#if 1
	float G			= min(1.f,min( 2.f*NdotH*NdotV/VdotH , 2.f*NdotH*NdotL/VdotH ));
	float M0		= max(0.f,F *G / (NdotL * NdotV));
	#else
	float M0		= max(0.f,F / (VdotH * VdotH));
	#endif

	// Use Beckmann NDF
	float kappa		= sNdotH/(NdotH*_roughness);
	float D			= max(0.f, 1.f / (3.141592654f * _roughness*_roughness * pow(NdotH,4.f)) * exp(-kappa*kappa));

	float sRadiance	= M0 * D;
	float dRadiance = NdotL * 0.3183098861f;
	return dRadiance + _specularity*sRadiance;
}
//------------------------------------------------------------------------------
// Wang Wrapping [SigAsia09]
void WangWrap(	in  vec3  _vDirection,
				in  vec3  _normal,
				in  float _roughness,
				out vec3  _rDirection,
				out float _rExponent,
				out float _rScale)
{
	float NdotV 	= max(0.0f,dot(_normal,_vDirection));

	// Because we use a perfect reflection as BRDF center
	// H = N
	// Use Schlick approximation for Fresnel
	// Use Kelemen and Szirmau-Kalos apprixmation for the geometric term
	float F0		= 0.1f;
	float F			= F0 + (1.f-F0) * pow(1.f - NdotV,5.f);
	float M0		= max(0.f,F / (NdotV * NdotV));

	// Wrap NDF to the explicit BRDF response (assume isotropic ouput)
	float lambda_d	= 2.f/(_roughness*_roughness) - 2;
	_rExponent 		= clamp(lambda_d / float(abs(4*NdotV)),0.f,20000.f);
	_rDirection		= normalize(2.f * NdotV * _normal - _vDirection);
	_rScale			= M0 * 0.159154943f;
}
//------------------------------------------------------------------------------
// Wang BRDF [SigAsia09]
float WangBRDF(	in vec3  _viewDir,
				in vec3  _lightDir,
				in vec3  _normal,
				in float _roughness,
				in float _specularity)
{
	// Use Wang09 approximation for CT BRDF
	vec3  h			= normalize(_viewDir+_lightDir);
	float VdotH 	= max(0.0f,dot(_viewDir,h));
	float NdotV 	= max(0.0f,dot(_normal,_viewDir));
	float NdotL 	= max(0.0f,dot(_normal,_lightDir));

	// Use Schlick approximation for Fresnel
	// Use Kelemen and Szirmau-Kalos apprixmation for the geometric term
	float F0		= 0.1f;
	float F			= F0 + (1.f-F0) * pow(1.f - VdotH,5.f);
	float M0		= max(0.f,F / (VdotH * VdotH));

	// Wrap NDF to the explicit BRDF response (assume isotropic ouput)
	float lambda_d	= 2.f/(_roughness*_roughness) - 2;
	float lambda_w	= clamp(lambda_d / float(abs(4*NdotV)),0.f,20000.f);
	vec3 p_w		= normalize(2.f * NdotV * _normal - _viewDir);
	float D			= (lambda_d+2) * 0.159154943f * exp(lambda_w * (dot(p_w,_lightDir)-1) );

	float sRadiance	= M0 * D;
	float dRadiance = NdotL * 0.3183098861f;
	return dRadiance + _specularity*sRadiance;
}
//------------------------------------------------------------------------------
