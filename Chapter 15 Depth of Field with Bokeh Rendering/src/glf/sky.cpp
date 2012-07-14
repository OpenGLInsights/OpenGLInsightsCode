//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/sky.hpp>
#include <glf/rng.hpp>
#include <glf/geometry.hpp>
#include <glf/window.hpp>
#include <glm/gtx/transform.hpp>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

namespace glf
{
	//-------------------------------------------------------------------------
	CubeMap::CubeMap():
	program("CubeMap")
	{
		glf::Info("Create cubemap");
		CreateCubePos(vbuffer);
		vao.Add(vbuffer,semantic::Position,3,GL_FLOAT);

		program.Compile(ProgramOptions::CreateVSOptions().Append(LoadFile(directory::ShaderDirectory + "cubemap.vs")),
						LoadFile(directory::ShaderDirectory + "cubemap.fs"));
		envTexUnit			= program["EnvTex"].unit;
		transformVar		= program["Transformation"].location;

		glProgramUniform1i(program.id, program["EnvTex"].location, envTexUnit);
		assert(glf::CheckError("CubeMap::CubeMap"));
	}
	//-------------------------------------------------------------------------
	CubeMap::~CubeMap()
	{

	}
	//-------------------------------------------------------------------------
	void CubeMap::Draw(	const glm::mat4&		_proj, 
						const glm::mat4& 		_view, 
						const TextureCube&  	_envTex)
	{
		// Remove the translation from the view matrix
		glm::mat4 transform = _proj * glm::mat4(_view[0],
												_view[1],
												_view[2],
												glm::vec4(0,0,0,1));
		glUseProgram(program.id);
		_envTex.Bind(envTexUnit);
		glProgramUniformMatrix4fv(program.id, transformVar, 1, GL_FALSE, &transform[0][0]);
		vao.Draw(GL_TRIANGLES,vbuffer.count,0);
		glf::CheckError("CubeMap::Draw");
	}
	//-------------------------------------------------------------------------
	SkyBuilder::SkyBuilder(int _res):
	program("Sky"),
	resolution(_res),
	sunTheta(0),
	sunPhi(0),
	sunFactor(1),
	turbidity(2)
	{
		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);

		glm::mat4 transformations[6];
		transformations[0] = glm::rotate( 90.f,0.f,1.f,0.f);	// Positive X
		transformations[1] = glm::rotate(-90.f,0.f,1.f,0.f);	// Negative X
		transformations[2] = glm::rotate( 90.f,1.f,0.f,0.f);	// Positive Y
		transformations[3] = glm::rotate(-90.f,1.f,0.f,0.f);	// Negative Y
		transformations[4] = glm::mat4(1.f);					// Positive Z
		transformations[5] = glm::rotate(180.f,1.f,0.f,0.f);	// Negative Z

		ProgramOptions options = ProgramOptions::CreateVSOptions();
		program.Compile(options.Append(LoadFile(directory::ShaderDirectory + "skybuilder.vs")),
						options.Append(LoadFile(directory::ShaderDirectory + "skybuilder.gs")),
						options.Append(LoadFile(directory::ShaderDirectory + "skybuilder.fs")));

		drawSunVar	 		= program["DrawSun"].location;
		sunFactorVar 		= program["SunFactor"].location;
		turbidityVar 		= program["Turbidity"].location;
		sunSphCoordVar 		= program["SunSphCoord"].location;

		glProgramUniform2f(program.id, sunSphCoordVar,	sunTheta, sunPhi);
		glProgramUniform1f(program.id, turbidityVar,	turbidity);
		glProgramUniformMatrix4fv(program.id, program["Transformations[0]"].location, 6, GL_FALSE, &transformations[0][0][0]);

		// Init sky framebuffer
		glGenFramebuffers(1,&skyFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER,skyFramebuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		glf::CheckFramebuffer(skyFramebuffer);
		glf::CheckError("SkyBuilder::SkyBuilder2");
	}
	//-------------------------------------------------------------------------
	SkyBuilder::~SkyBuilder()
	{

	}
	//-------------------------------------------------------------------------
	void SkyBuilder::SetSunFactor(		float _sunFactor)
	{
		sunFactor 		= _sunFactor;
		glProgramUniform1f(program.id, sunFactorVar, sunFactor);
	}
	//-------------------------------------------------------------------------
	void SkyBuilder::SetPosition(		float _theta, 
										float _phi)
	{
		sunTheta 		= _theta;
		sunPhi	 		= _phi;
		sunIntensity 	= ComputeSunIntensity(sunTheta,sunPhi, turbidity);
		glProgramUniform2f(program.id, sunSphCoordVar, sunTheta, sunPhi);
	}
	//-------------------------------------------------------------------------
	void SkyBuilder::SetTurbidity(		float _turbidity)
	{
		turbidity 		= _turbidity;
		sunIntensity 	= ComputeSunIntensity(sunTheta,sunPhi, turbidity);
		glProgramUniform1f(program.id, turbidityVar, turbidity);
	}
	//-------------------------------------------------------------------------
	void SkyBuilder::Build(				TextureCube& _cubeTex,
										bool _drawSun)
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(false);

		assert(_cubeTex.size.x==resolution);
		assert(_cubeTex.size.y==resolution);
		glProgramUniform1i(program.id, drawSunVar, _drawSun);

		// Render to cube map
		glUseProgram(program.id);
		glViewport(0,0,resolution,resolution);
			glBindFramebuffer(GL_FRAMEBUFFER,skyFramebuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _cubeTex.id, 0);
				glClear(GL_COLOR_BUFFER_BIT);
				vao.Draw(GL_TRIANGLES,vbo.count,0);
			glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,ctx::window.Size.x,ctx::window.Size.y);

		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);

		// Generate mipmap (required by the cubemap renderer otherwise 
		// interpolation at corner are wrong, since corser mipmap level are 
		// not available)
		glBindTexture(_cubeTex.target,_cubeTex.id);
		glGenerateMipmap(_cubeTex.target);
		glBindTexture(_cubeTex.target,0);

		assert(glf::CheckFramebuffer(skyFramebuffer));
		glf::CheckError("Sky::Update");	
	}
	//--------------------------------------------------------------------------
	glm::vec3 SkyBuilder::ToCartesian(float _theta, float _phi)
	{
		float sinTheta = sin(_theta);
		return glm::vec3(sinTheta*cos(_phi),sinTheta*sin(_phi),cos(_theta));
	}
	//--------------------------------------------------------------------------
	float SkyBuilder::PerezFunction(float A,  
									float B,  
									float C,  
									float D,  
									float E,  
									float cosTheta, 
									float gamma,
									float thetaS, 
									float lvz )
	{  
		float den = (1.f + A * exp(B)) * (1.f + C * exp(D * thetaS) + E * cos(thetaS)*cos(thetaS));  
		float num = (1.f + A * exp(B / cosTheta)) * (1.f + C * exp(D * gamma) + E * cos(gamma)*cos(gamma));  
		return (lvz * num / den);
	}
	//-------------------------------------------------------------------------
	glm::vec3 SkyBuilder::ComputeSunIntensity(float _sunTheta, float _sunPhi, float _turbidity)
	{
		using namespace glm;
		const float     SunRadius   = 0.018f;
		const float     SunFalloff  = 0.022f;
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
		//
		float cosTheta  = cos(_sunTheta);
		float gamma  	= 0.f; //acos(dot(ToCartesian(_sunTheta,_sunPhi),dir));
		float thetaS 	= _sunTheta;   

		// Check if direction is in the sun and clamp according to its distance
		// Factor is in [1,5] : 5 in the sun, 1 outside with a smooth transition
		float w         = (gamma-SunRadius)/(SunFalloff-SunRadius);
		cosTheta        = gamma<SunFalloff?cos(thetaS):cosTheta;
		gamma           = gamma<SunFalloff?0:gamma;
		float factor    = 1.f + sunFactor*smoothstep(0.f,1.f,1.f-w);

		float thetaS2	= thetaS * thetaS;  
		float thetaS3	= thetaS * thetaS2; 
		vec3  T			= vec3(_turbidity*_turbidity,_turbidity,1);
		vec4  thetas    = vec4(thetaS3,thetaS2,thetaS,1);

		// Compute zenith luminance and convert it from kcd/m^2 to cd/m^2  
		float chi		= (4.f / 9.0f - _turbidity / 120.f) * (M_PI - 2.f * thetaS);  
		float zenith_Y  = (4.0453f*_turbidity - 4.9710f) * tan( chi ) - 0.2155f*_turbidity + 2.4192f;
		zenith_Y	   *= 1000.f;  

		// Compute chromacity
		float zenith_x  = dot( T, Mx * thetas);
		float zenith_y  = dot( T, My * thetas);

		// Compute variation according to the view direction
		vec2 t			= vec2(_turbidity,1);
		float x 		= PerezFunction(dot(t,A_x),dot(t,B_x),dot(t,C_x),dot(t,D_x),dot(t,E_x),cosTheta,gamma,thetaS,zenith_x);
		float y 		= PerezFunction(dot(t,A_y),dot(t,B_y),dot(t,C_y),dot(t,D_y),dot(t,E_y),cosTheta,gamma,thetaS,zenith_y);

		// Conversion from xyZ to XYZ
		vec3 XYZ;
		XYZ.y 			= abs(PerezFunction(dot(t,A_Y),dot(t,B_Y),dot(t,C_Y),dot(t,D_Y),dot(t,E_Y),cosTheta,gamma,thetaS,zenith_Y))*factor;
		XYZ.x 			= (x / y) * XYZ.y;  
		XYZ.z			= ((1.f - x - y) / y) * XYZ.y;

		// Conversion from XYZ to RGB
		//	FragColor = vec4( vec3(1.0) - exp(-(1.0/15000.0) * (XYZ2RGB * XYZ)), 1);
		return XYZ2RGB * XYZ;
	}
	//-------------------------------------------------------------------------
}
