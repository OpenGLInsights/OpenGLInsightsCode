//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <glf/csm.hpp>
#include <glf/window.hpp>
#include <glf/geometry.hpp>
#include <glf/debug.hpp>
#include <glm/gtx/transform.hpp>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
#define CONSTANT_K_EVSM			50.f
#define ALIGN_CSM_WITH_CAMERA	1
#define ENABLE_SHADOW_SSM		0
#define ENABLE_SHADOW_VSM		0
#define ENABLE_SHADOW_EVSM		1
#if (ENABLE_SHADOW_SSM + ENABLE_SHADOW_VSM + ENABLE_SHADOW_EVSM != 1) 
#	error("Invalid selection of shadow techniques") 
#endif

namespace glf
{
	//-------------------------------------------------------------------------
	// Corner0 : -1 -1 
	// Corner1 :  1 -1 
	// Corner2 :  1  1 
	// Corner3 : -1  1 
	//-------------------------------------------------------------------------
	void FrustumPlaneExtraction
	(
		const glm::vec3& _camPos,
		const glm::vec3& _camDir,
		const glm::vec3& _camUp,
		float _camRatio,
		float _camFov,
		float _camZ,
		glm::vec3& _corner0,
		glm::vec3& _corner1,
		glm::vec3& _corner2,
		glm::vec3& _corner3
	)
	{
		// Compute half size of near and far planes
		float hHeight  	 = tan(_camFov*0.5f) * _camZ;
		float hWidth   	 = hHeight * _camRatio;
		glm::vec3 center = _camPos + _camDir * _camZ;
		glm::vec3 right	 = glm::normalize(glm::cross(_camDir,_camUp));

		// Deduce bounding points
		_corner0 = center - (_camUp * hHeight) - (right * hWidth);
		_corner1 = center - (_camUp * hHeight) + (right * hWidth);
		_corner2 = center + (_camUp * hHeight) + (right * hWidth);
		_corner3 = center + (_camUp * hHeight) - (right * hWidth);
	}
	//-------------------------------------------------------------------------
	CSMLight::CSMLight(int _w, int _h,int _nCascades):
	direction(0,0,-1),
	intensity(1.f,1.f,1.f),
	nCascades(_nCascades)
	{
		glf::Info("CSMLight::CSMLight");
		assert(nCascades<=4);

		projs 		= new glm::mat4[nCascades];
		viewprojs	= new glm::mat4[nCascades];
		nearPlanes	= new float[nCascades];
		farPlanes	= new float[nCascades];

		depthTexs.Allocate(GL_DEPTH_COMPONENT32F,_w,_h,nCascades);
		depthTexs.SetFiltering(GL_LINEAR,GL_LINEAR);
		depthTexs.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		depthTexs.SetCompare(GL_COMPARE_REF_TO_TEXTURE,GL_LEQUAL);

		#if (ENABLE_SHADOW_VSM || ENABLE_SHADOW_EVSM)
		momentTexs.Allocate(GL_RG32F,_w,_h,nCascades);
		momentTexs.SetFiltering(GL_LINEAR,GL_LINEAR);
		momentTexs.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

		tmpTexs.Allocate(GL_RG32F,_w,_h,nCascades);
		tmpTexs.SetFiltering(GL_LINEAR,GL_LINEAR);
		tmpTexs.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);

		filterTexs.Allocate(GL_RG32F,_w,_h,nCascades);
		filterTexs.SetFiltering(GL_LINEAR,GL_LINEAR);
		filterTexs.SetWrapping(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
		#endif

		glGenFramebuffers(1, &depthFBO);
		glBindFramebuffer(GL_FRAMEBUFFER,depthFBO);
		#if ENABLE_SHADOW_SSM
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexs.id, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		#else
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,momentTexs.id, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexs.id, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		#endif
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(depthFBO);

		#if (ENABLE_SHADOW_VSM || ENABLE_SHADOW_EVSM)
		glGenFramebuffers(1, &tmpFBO);
		glBindFramebuffer(GL_FRAMEBUFFER,tmpFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,tmpTexs.id, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(tmpFBO);

		glGenFramebuffers(1, &filterFBO);
		glBindFramebuffer(GL_FRAMEBUFFER,filterFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,filterTexs.id, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glf::CheckFramebuffer(filterFBO);
		#endif

		// Default init
		for(int i=0;i<nCascades;++i)
		{
			nearPlanes[i] = 0.1f;
			farPlanes[i]  = 100.f;
		}
		glf::CheckError("CSMLight::CSMLight");
	}
	//-------------------------------------------------------------------------
	CSMLight::~CSMLight()
	{
		delete[] farPlanes;
		delete[] nearPlanes;
		delete[] viewprojs;
		delete[] projs;
		glDeleteFramebuffers(1,&depthFBO);
		#if (ENABLE_SHADOW_VSM || ENABLE_SHADOW_EVSM)
		glDeleteFramebuffers(1,&filterFBO);
		#endif
	}
	//-------------------------------------------------------------------------
	void CSMLight::SetDirection(	const glm::vec3& _direction)
	{
		direction		 = glm::normalize(_direction);
	}
	//-------------------------------------------------------------------------
	void CSMLight::SetIntensity(	const glm::vec3& _intensity)
	{
		intensity = _intensity;
	}
	//-------------------------------------------------------------------------
	CSMBuilder::CSMBuilder():
	maxCascades(4)
	{
		CreateScreenTriangle(vbo);
		vao.Add(vbo,semantic::Position,2,GL_FLOAT);

		// Program regular mesh
		ProgramOptions regularOptions = ProgramOptions::CreateVSOptions();
		regularOptions.AddDefine<int>("CSM_BUILDER", 1);
		#if   ENABLE_SHADOW_SSM
		regularOptions.AddDefine<int>("SSM", 1);
		#elif ENABLE_SHADOW_VSM
		regularOptions.AddDefine<int>("VSM", 1);
		#elif ENABLE_SHADOW_EVSM
		regularOptions.AddDefine<int>("EVSM", 1);
		regularOptions.AddDefine<float>("K_EVSM_VALUE", CONSTANT_K_EVSM);
		#endif
		regularOptions.AddDefine<int>("MAX_CASCADES",maxCascades);
		regularRenderer.program.Compile(regularOptions.Append(LoadFile(directory::ShaderDirectory + "meshregular.vs")),
										regularOptions.Append(LoadFile(directory::ShaderDirectory + "meshregular.gs")),
										regularOptions.Append(LoadFile(directory::ShaderDirectory + "meshregular.fs")));

		regularRenderer.projVar 		= regularRenderer.program["Projections[0]"].location;
		regularRenderer.viewVar 		= regularRenderer.program["View"].location;
		regularRenderer.modelVar 		= regularRenderer.program["Model"].location;
		regularRenderer.nCascadesVar	= regularRenderer.program["nCascades"].location;

		// Program terrain mesh
		ProgramOptions terrainOptions = ProgramOptions::CreateVSOptions();
		terrainOptions.AddDefine<int>("CSM_BUILDER", 1);
		#if   ENABLE_SHADOW_SSM
		terrainOptions.AddDefine<int>("SSM", 1);
		#elif ENABLE_SHADOW_VSM
		terrainOptions.AddDefine<int>("VSM", 1);
		#elif ENABLE_SHADOW_EVSM
		terrainOptions.AddDefine<int>("EVSM", 1);
		terrainOptions.AddDefine<float>("K_EVSM_VALUE", CONSTANT_K_EVSM);
		#endif
		terrainOptions.AddDefine<int>("MAX_CASCADES",maxCascades);
		terrainRenderer.program.Compile(terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.vs")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.cs")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.es")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.gs")),
										terrainOptions.Append(LoadFile(directory::ShaderDirectory + "meshterrain.fs")));

		terrainRenderer.projVar 		= terrainRenderer.program["Projections[0]"].location;
		terrainRenderer.viewVar 		= terrainRenderer.program["View"].location;
		terrainRenderer.nCascadesVar	= terrainRenderer.program["nCascades"].location;

		terrainRenderer.heightTexUnit	= terrainRenderer.program["HeightTex"].unit;
		terrainRenderer.tileSizeVar		= terrainRenderer.program["TileSize"].location;
		terrainRenderer.tileCountVar	= terrainRenderer.program["TileCount"].location;
		terrainRenderer.tileOffsetVar	= terrainRenderer.program["TileOffset"].location;
		terrainRenderer.projFactorVar	= terrainRenderer.program["ProjFactor"].location;
		terrainRenderer.tessFactorVar	= terrainRenderer.program["TessFactor"].location;
		terrainRenderer.heightFactorVar	= terrainRenderer.program["HeightFactor"].location;

		glProgramUniform1i(terrainRenderer.program.id, terrainRenderer.program["HeightTex"].location,  terrainRenderer.heightTexUnit);


		// Program filter moments
		ProgramOptions filterOptions = ProgramOptions::CreateVSOptions();
		#if   ENABLE_SHADOW_SSM
		filterOptions.AddDefine<int>("SSM", 1);
		#elif ENABLE_SHADOW_VSM
		filterOptions.AddDefine<int>("VSM", 1);
		#elif ENABLE_SHADOW_EVSM
		filterOptions.AddDefine<int>("EVSM", 1);
		#endif
		filterOptions.AddDefine<int>("CSM_FILTER",1);
		filterOptions.AddDefine<int>("MAX_CASCADES",maxCascades);
		momentFilter.program.Compile(	filterOptions.Append(LoadFile(directory::ShaderDirectory + "csm.vs")),
										filterOptions.Append(LoadFile(directory::ShaderDirectory + "csm.gs")),
										filterOptions.Append(LoadFile(directory::ShaderDirectory + "csm.fs")));

		momentFilter.directionVar  = momentFilter.program["Direction"].location;
		momentFilter.momentTexUnit = momentFilter.program["MomentTex"].unit;
		glProgramUniform1i(momentFilter.program.id, momentFilter.program["MomentTex"].location, momentFilter.momentTexUnit);
	}
	//-------------------------------------------------------------------------
	void CSMBuilder::Draw(	CSMLight&			_light,
							const Camera&		_camera,
							float 				_cascadeAlpha,
							float 				_blendFactor,
							const SceneManager& _scene)
	{

		// Extract camera near/far
		float n = _camera.Near();
		float f = _camera.Far();

		// Extract near plane corners
		glm::mat4 camView	= _camera.View();
		glm::vec3 camPos	= _camera.Eye();
		glm::vec3 camDir	=  glm::normalize(_camera.Center()-camPos);
		glm::vec3 camUp		= _camera.Up();
		glm::vec3 camRight	=  glm::normalize(glm::cross(camDir,camUp));
		float camFov		= _camera.Fov();
		float camRatio		= _camera.Ratio();

		#if ENABLE_CSM_HELPERS
		glf::manager::helpers->Clear();
		glf::manager::helpers->CreateReferential(camRight,camUp,-camDir,0.3f,glm::translate(camPos.x,camPos.y,camPos.z));
		#endif

		// Compute scene AABB in view space
		// Update according to the scene bound
		// Invert min.z/max.z bounds because of the view matrix : objects are into the negative z part
		BBox sceneView   	= Transform(_scene.wBound,camView);
		n 					= std::max(n,-sceneView.pMax.z);
		f 					= std::min(f,-sceneView.pMin.z);

		// Compute lightView matrix (CSM is aligned with camera)
		glm::vec3 lightTar	= camPos;
		glm::vec3 lightDir	= _light.direction;
		glm::vec3 lightUp	= -camRight;
		if(fabs(glm::dot(lightUp,lightDir))>0.9f) lightUp = camUp;
		glm::vec3 lightRight= glm::normalize(glm::cross(lightDir,lightUp));
		lightUp				= glm::normalize(glm::cross(lightRight,lightDir));
		_light.view			= glm::lookAt(lightTar,lightTar+lightDir,lightUp);
		_light.camView		= camView;

		#if ENABLE_CSM_HELPERS
		glf::manager::helpers->CreateReferential(lightRight,lightUp,-lightDir,1.f,glm::translate(camPos.x,camPos.y,camPos.z));
		#endif

		// Compute scene AABB in light space
		BBox sceneLight   	= Transform(_scene.wBound,_light.view);

		// Project corners into light space
		// TODO : Change to a direct transform from cam project space to light space instead of world to light space ?
		glm::vec3 c0,c1,c2,c3;
		FrustumPlaneExtraction(camPos,camDir,camUp,camRatio,camFov,n,c0,c1,c2,c3);
		glm::vec4 c00_v 	= _light.view * glm::vec4(c0,1.f);
		glm::vec4 c01_v 	= _light.view * glm::vec4(c1,1.f);
		glm::vec4 c02_v 	= _light.view * glm::vec4(c2,1.f);
		glm::vec4 c03_v 	= _light.view * glm::vec4(c3,1.f);

		#if ENABLE_CSM_HELPERS
		glf::manager::helpers->CreateBound(c0,c1,c2,c3);
		#endif

		// For each cascade
		float previousFar	= n;
		for(int i=0;i<_light.nCascades;++i)
		{
			// Divide frustum with a magic formula 
			// (see : http://software.intel.com/en-us/articles/shadowexplorer/)
			float w			= ( i + 1.f ) / float(_light.nCascades);
			float camSpaceZ = _cascadeAlpha * n * pow( f / n, w ) + ( 1 - _cascadeAlpha ) * ( n + w * ( f - n ) );

			// Compute the far split plane
			FrustumPlaneExtraction(camPos,camDir,camUp,camRatio,camFov,camSpaceZ,c0,c1,c2,c3);
			glm::vec4 c10_v = _light.view * glm::vec4(c0,1.f);
			glm::vec4 c11_v = _light.view * glm::vec4(c1,1.f);
			glm::vec4 c12_v = _light.view * glm::vec4(c2,1.f);
			glm::vec4 c13_v = _light.view * glm::vec4(c3,1.f);

			#if ENABLE_CSM_HELPERS
			glm::mat4 viewInverse = glm::inverse(_light.view);
			glf::manager::helpers->CreateBound(	glm::vec3(viewInverse * c00_v),
												glm::vec3(viewInverse * c01_v),
												glm::vec3(viewInverse * c02_v),
												glm::vec3(viewInverse * c03_v),

												glm::vec3(viewInverse * c10_v),
												glm::vec3(viewInverse * c11_v),
												glm::vec3(viewInverse * c12_v),
												glm::vec3(viewInverse * c13_v),

												glm::mat4(1.f),
												glm::vec3(1,0,1));
			#endif

			// Compute AABB in light space
			BBox boundSplit;
			boundSplit.Add(glm::vec3(c00_v));
			boundSplit.Add(glm::vec3(c01_v));
			boundSplit.Add(glm::vec3(c02_v));
			boundSplit.Add(glm::vec3(c03_v));
			boundSplit.Add(glm::vec3(c10_v));
			boundSplit.Add(glm::vec3(c11_v));
			boundSplit.Add(glm::vec3(c12_v));
			boundSplit.Add(glm::vec3(c13_v));

			// Extract min-max Z-range in light space (take in accound scene bounds)
			boundSplit.pMin.x = glm::max(sceneLight.pMin.x, boundSplit.pMin.x);	
			boundSplit.pMax.x = glm::min(sceneLight.pMax.x, boundSplit.pMax.x);
			boundSplit.pMin.y = glm::max(sceneLight.pMin.y, boundSplit.pMin.y);	
			boundSplit.pMax.y = glm::min(sceneLight.pMax.y, boundSplit.pMax.y);
			boundSplit.pMin.z = sceneLight.pMin.z;
			boundSplit.pMax.z = sceneLight.pMax.z;

			// Save the far split plane for the next split (it becomes the near split plane)
			c00_v = c10_v;
			c01_v = c11_v;
			c02_v = c12_v;
			c03_v = c13_v;

			// Copy the camera z of the current split
			_light.nearPlanes[i]= previousFar;
			_light.farPlanes[i]	= camSpaceZ;
			previousFar			= camSpaceZ;
			//glf::Info("CamZ : %f",camSpaceZ);

			// Compute the light projection based on split AABB
			// Inverse z because of the light view matrix which has negative z
			_light.projs[i]		= glm::ortho(boundSplit.pMin.x,  boundSplit.pMax.x,
											 boundSplit.pMin.y,  boundSplit.pMax.y,
											-boundSplit.pMax.z, -boundSplit.pMin.z);
			_light.viewprojs[i]	= _light.projs[i] * _light.view;

			#if ENABLE_CSM_HELPERS
			glm::mat4 invViewProj = glm::inverse(_light.viewprojs[i]);
			glf::manager::helpers->CreateBound(	glm::vec3(invViewProj * glm::vec4(-1,-1,-1, 1)),
												glm::vec3(invViewProj * glm::vec4( 1,-1,-1, 1)),
												glm::vec3(invViewProj * glm::vec4( 1, 1,-1, 1)),
												glm::vec3(invViewProj * glm::vec4(-1, 1,-1, 1)),

												glm::vec3(invViewProj * glm::vec4(-1,-1, 1, 1)),
												glm::vec3(invViewProj * glm::vec4( 1,-1, 1, 1)),
												glm::vec3(invViewProj * glm::vec4( 1, 1, 1, 1)),
												glm::vec3(invViewProj * glm::vec4(-1, 1, 1, 1)));
			#endif
		}

		// Render cascaded shadow maps 
		assert(_light.nCascades<=maxCascades);
		glViewport(0,0,_light.depthTexs.size.x,_light.depthTexs.size.y);
		glBindFramebuffer(GL_FRAMEBUFFER,_light.depthFBO);

		#if ENABLE_SHADOW_SSM
		glClear(GL_DEPTH_BUFFER_BIT);
		#else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		#endif
		// Regular renderer
		glf::manager::timings->StartSection(glf::section::CsmBuilderRegular);
		{
			glUseProgram(regularRenderer.program.id);
			glProgramUniform1i(regularRenderer.program.id, 			regularRenderer.nCascadesVar,	_light.nCascades);
			glProgramUniformMatrix4fv(regularRenderer.program.id, 	regularRenderer.projVar,  		_light.nCascades, 	GL_FALSE, &_light.projs[0][0][0]);
			glProgramUniformMatrix4fv(regularRenderer.program.id, 	regularRenderer.viewVar,  		1, 					GL_FALSE, &_light.view[0][0]);

			for(unsigned int o=0;o<_scene.shadowMeshes.size();++o)
			{
				glProgramUniformMatrix4fv(regularRenderer.program.id, regularRenderer.modelVar, 1, GL_FALSE, &_scene.transformations[o][0][0]);
				_scene.shadowMeshes[o].Draw();
			}
			glf::CheckError("CSMBuilder::Draw::Regulars");
		}
		glf::manager::timings->EndSection(glf::section::CsmBuilderRegular);

		// Terrain renderer
		glf::manager::timings->StartSection(glf::section::CsmBuilderTerrain);
		{
			glUseProgram(terrainRenderer.program.id);
			glProgramUniform1i(terrainRenderer.program.id, 			terrainRenderer.nCascadesVar,	_light.nCascades);
			glProgramUniformMatrix4fv(terrainRenderer.program.id, 	terrainRenderer.projVar,  		_light.nCascades, 	GL_FALSE, &_light.projs[0][0][0]);
			glProgramUniformMatrix4fv(terrainRenderer.program.id, 	terrainRenderer.viewVar,  		1, 					GL_FALSE, &_light.view[0][0]);

			for(unsigned int o=0;o<_scene.terrainMeshes.size();++o)
			{
				const TerrainMesh& mesh = _scene.terrainMeshes[o];
				glProgramUniform3f(terrainRenderer.program.id, 		terrainRenderer.tileOffsetVar,	mesh.tileOffset.x, mesh.tileOffset.y, mesh.tileOffset.z);
				glProgramUniform2i(terrainRenderer.program.id, 		terrainRenderer.tileCountVar,	mesh.tileCount.x,  mesh.tileCount.y);
				glProgramUniform2f(terrainRenderer.program.id, 		terrainRenderer.tileSizeVar,	mesh.tileSize.x,   mesh.tileSize.y);
				glProgramUniform1f(terrainRenderer.program.id, 		terrainRenderer.tessFactorVar,	mesh.tessFactor);
				glProgramUniform1f(terrainRenderer.program.id, 		terrainRenderer.heightFactorVar,mesh.heightFactor);
				glProgramUniform1f(terrainRenderer.program.id, 		terrainRenderer.projFactorVar,	mesh.projFactor);

				mesh.heightTex->Bind(terrainRenderer.heightTexUnit);
				_scene.terrainMeshes[o].Draw();
			}
			glf::CheckError("CSMBuilder::Draw::Terrains");
		}
		glf::manager::timings->EndSection(glf::section::CsmBuilderTerrain);

		// Filter shadow map with VSM or EVSM
		glf::manager::timings->StartSection(glf::section::CsmBuilderFilter);
		#if (ENABLE_SHADOW_VSM || ENABLE_SHADOW_EVSM)
		glUseProgram(momentFilter.program.id);

		glBindFramebuffer(GL_FRAMEBUFFER,_light.tmpFBO);
		_light.momentTexs.Bind(momentFilter.momentTexUnit);
		glProgramUniform2f(momentFilter.program.id, momentFilter.directionVar, 1, 0);
		vao.Draw(GL_TRIANGLES,3,0,_light.nCascades); // Instanced screen triangles for each cascade

		glBindFramebuffer(GL_FRAMEBUFFER,_light.filterFBO);
		_light.tmpTexs.Bind(momentFilter.momentTexUnit);
		glProgramUniform2f(momentFilter.program.id, momentFilter.directionVar, 0, 1);
		vao.Draw(GL_TRIANGLES,3,0,_light.nCascades); // Instanced screen triangles for each cascade
		#endif
		glf::manager::timings->EndSection(glf::section::CsmBuilderFilter);

		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glViewport(0,0,ctx::window.Size.x,ctx::window.Size.y);

		glf::CheckError("CSMBuilder::Draw");
	}
	//-------------------------------------------------------------------------
	CSMRenderer::CSMRenderer(int _w, int _h):
	program("CSMRenderer")
	{
		ProgramOptions options = ProgramOptions::CreateVSOptions();
		#if   ENABLE_SHADOW_SSM
		options.AddDefine<int>("SSM", 1);
		#elif ENABLE_SHADOW_VSM
		options.AddDefine<int>("VSM", 1);
		#elif ENABLE_SHADOW_EVSM
		options.AddDefine<int>("EVSM", 1);
		options.AddDefine<float>("K_EVSM_VALUE", CONSTANT_K_EVSM);
		#endif
		options.AddDefine<int>("CSM_RENDERER",1);
		options.AddDefine<int>("LIGHTING_ONLY",ENABLE_LIGHTING_ONLY);
		options.Include(LoadFile(directory::ShaderDirectory + "brdf.fs"));
		program.Compile(options.Append(LoadFile(directory::ShaderDirectory + "csm.vs")),
						options.Append(LoadFile(directory::ShaderDirectory + "csm.fs")));

		viewPosVar 			= program["ViewPos"].location;
		lightDirVar 		= program["LightDir"].location;
		lightViewProjsVar	= program["LightViewProjs[0]"].location;
		lightIntensityVar	= program["LightIntensity"].location;
		biasVar				= program["Bias"].location;
		nCascadesVar		= program["nCascades"].location;

		blendFactorVar		= program["BlendFactor"].location;

		positionTexUnit		= program["PositionTex"].unit;
		diffuseTexUnit		= program["DiffuseTex"].unit;
		normalTexUnit		= program["NormalTex"].unit;
		shadowTexUnit		= program["ShadowTex"].unit;

		glProgramUniform1i(program.id, program["PositionTex"].location,	positionTexUnit);
		glProgramUniform1i(program.id, program["ShadowTex"].location,	shadowTexUnit);
		glProgramUniform1i(program.id, program["DiffuseTex"].location,	diffuseTexUnit);
		glProgramUniform1i(program.id, program["NormalTex"].location,	normalTexUnit);

		glf::CheckError("CSMRenderer::Create");
	}
	//-------------------------------------------------------------------------
	void CSMRenderer::Draw(	const CSMLight&	_light,
							const GBuffer&	_gbuffer,
							const glm::vec3&_viewPos,
							float 			_blendFactor,
							float 			_bias,
							RenderTarget&	_target)
	{
		glUseProgram(program.id);

		glProgramUniform1f(program.id,			blendFactorVar,			_blendFactor);

		glProgramUniform1f(program.id,			biasVar,			_bias);
		glProgramUniform1i(program.id,			nCascadesVar,		_light.nCascades);
		glProgramUniform3f(program.id,			viewPosVar,			_viewPos.x, _viewPos.y, _viewPos.z);
		glProgramUniform3f(program.id,			lightDirVar,		_light.direction.x, _light.direction.y, _light.direction.z);
		glProgramUniformMatrix4fv(program.id,	lightViewProjsVar,	_light.nCascades, 	GL_FALSE, &_light.viewprojs[0][0][0]);
		glProgramUniform3f(program.id,			lightIntensityVar,	_light.intensity.x,	_light.intensity.y,	_light.intensity.z);

		#if ENABLE_SHADOW_SSM
		_light.depthTexs.Bind(shadowTexUnit);
		#else
		//_light.momentTexs.Bind(shadowTexUnit);
		_light.filterTexs.Bind(shadowTexUnit);
		#endif
		_gbuffer.positionTex.Bind(positionTexUnit);
		_gbuffer.diffuseTex.Bind(diffuseTexUnit);
		_gbuffer.normalTex.Bind(normalTexUnit);
		_target.Draw();

		glf::CheckError("CSMRenderer::Draw");
	}
}

