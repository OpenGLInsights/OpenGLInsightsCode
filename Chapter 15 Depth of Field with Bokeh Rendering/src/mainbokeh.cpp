//------------------------------------------------------------------------------
// Depth of Field with Bokeh Rendering
//
// Charles de Rousiers <charles.derousiers@gmail.com>
//------------------------------------------------------------------------------
#include <glf/window.hpp>
#include <glf/scene.hpp>
#include <glf/buffer.hpp>
#include <glf/pass.hpp>
#include <glf/csm.hpp>
#include <glf/debug.hpp>
#include <glf/sky.hpp>
#include <glf/probe.hpp>
#include <glf/ssao.hpp>
#include <glf/camera.hpp>
#include <glf/wrapper.hpp>
#include <glf/dofprocessor.hpp>
#include <glf/postprocessor.hpp>
#include <glf/terrain.hpp>
#include <glf/utils.hpp>
#include <glf/io/scene.hpp>
#include <glf/io/image.hpp>
#include <glf/io/config.hpp>
#include <fstream>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
//------------------------------------------------------------------------------
#ifdef WIN32
	#pragma warning( disable : 4996 )
	#pragma warning( disable : 4201 )
#endif
//------------------------------------------------------------------------------
#ifdef WIN32
	#define MAJOR_VERSION	4
	#define MINOR_VERSION	2
#else
	#define MAJOR_VERSION	4
	#define MINOR_VERSION	1	// Create a 4.2 context. Driver bug 285.05.09 on ubuntu 10.11
#endif

//-----------------------------------------------------------------------------
namespace ctx
{
	glf::Camera::Ptr						camera;
	glf::Window 							window(glm::ivec2(1280, 720));
	glui::GlutContext* 						ui;
	bool									drawHelpers = false;
	bool									drawTimings = false;
	bool									drawUI      = true;
	bool									drawWire    = false;
}
//-----------------------------------------------------------------------------
namespace
{
	struct SkyParams
	{
		float 								sunTheta;
		float 								sunPhi;
		float 								sunFactor;
		int 								turbidity;
	};

	struct ToneParams
	{
		float 								expToneExposure;
		float								toneExposure;
	};

	struct CSMParams
	{
		int 								nSamples;
		int 								nCascades;
		int 								resolution;
		float								bias;
		float								aperture;
		float								blendFactor;
		float								cascadeAlpha;
	};

	struct SSAOParams
	{
		float								beta;
		float								epsilon;
		float								sigma;
		float								kappa;
		float								radius;
		int									nSamples;
		float								sigmaScreen;
		float								sigmaDepth;
		int 								nTaps;
	};

	struct DOFParams
	{
		int 								nSamples;
		float								nearStart;
		float								nearEnd;
		float								farStart;
		float								farEnd;
		float								maxCoCRadius;
		float								maxBokehRadius;
		float								lumThreshold;
		float								cocThreshold;
		float								bokehDepthCutoff;
		bool								poissonFiltering;
		bool								enable;
	};

	struct TerrainParams
	{
		int									tileResolution;
		std::vector<float>					depthFactors;
		float								tessFactor;
		float								projFactor;
	};

	struct Application
	{
		Application(						int _w, 
											int _h,
											const SkyParams& _skyParams,
											const ToneParams& _toneParams,
											const CSMParams& _csmParams,
											const SSAOParams& _ssaoParams,
											const DOFParams& _dofParams,
											const TerrainParams& _terrainParams);
		glf::ResourceManager				resources;
		glf::SceneManager					scene;

		glf::TimingRenderer					timingRenderer;
		glf::HelperRenderer					helperRenderer;

		glf::GBuffer						gbuffer;
		glf::RenderSurface					renderSurface;
		glf::RenderTarget					renderTarget1;
		glf::RenderTarget					renderTarget2;
		glf::RenderTarget					renderTarget3;

		glf::CSMLight						csmLight;
		glf::CSMBuilder						csmBuilder;
		glf::CSMRenderer					csmRenderer;

		glf::CubeMap						cubeMap;
		glf::SkyBuilder						skyBuilder;
		glf::TerrainBuilder					terrainBuilder;

		glf::ProbeLight						probeLight;
		glf::ProbeBuilder					probeBuilder;
		glf::ProbeRenderer					probeRenderer;

		glf::SSAO							ssao;

		glf::DOFProcessor					dofProcessor;
		glf::PostProcessor					postProcessor;

		CSMParams 							csmParams;
		SSAOParams 							ssaoParams;
		ToneParams 							toneParams;
		SkyParams							skyParams;
		DOFParams							dofParams;
		TerrainParams						terrainParams;

		bool								updateTerrain;
		bool								updateLighting;
		int									activeBokeh;
		int									activeBuffer;
		int									activeMenu;

		#if ENABLE_BOKEH_STATISTICS
		bool								bokehQuery;
		bool								bokehRecord;
		std::ofstream						bokehFile;
		#endif
	};
	Application*							app;

	const char*								bokehNames[]	= {"Pentagonal","Hexagonal","Circle","Star"};
	struct									bokehType		{ enum Type {BK_PENTAGONAL, BK_HEXAGONAL, BK_CIRCLE,BK_STAR,MAX }; };
	const char*								bufferNames[]	= {"Composition","Position","Normal","Diffuse"};
	struct									bufferType		{ enum Type {GB_COMPOSITION,GB_POSITION,GB_NORMAL,GB_DIFFUSE,MAX }; };
	const char*								menuNames[]		= {"Tone","Sky","CSM","SSAO", "DoF", "Terrain" };
	struct									menuType		{ enum Type {MN_TONE,MN_SKY,MN_CSM,MN_SSAO,MN_DOF,MN_TERRAIN,MAX }; };

	Application::Application(				int _w, 
											int _h,
											const SkyParams& _skyParams,
											const ToneParams& _toneParams,
											const CSMParams& _csmParams,
											const SSAOParams& _ssaoParams,
											const DOFParams& _dofParams,
											const TerrainParams& _terrainParams):
	timingRenderer(_w,_h),
	gbuffer(_w,_h),
	renderSurface(_w,_h),
	renderTarget1(_w,_h),
	renderTarget2(_w,_h),
	renderTarget3(_w,_h),
	csmLight(_csmParams.resolution,_csmParams.resolution,_csmParams.nCascades),
	csmBuilder(),
	csmRenderer(_w,_h),
	cubeMap(),
	skyBuilder(1024),
	terrainBuilder(),
	probeLight(1024),
	probeBuilder(1024),
	probeRenderer(_w,_h),
	ssao(_w,_h),
	dofProcessor(_w,_h),
	postProcessor(_w,_h)
	{
		skyParams					= _skyParams;
		toneParams					= _toneParams;
		csmParams					= _csmParams;
		ssaoParams					= _ssaoParams;
		dofParams					= _dofParams;
		terrainParams				= _terrainParams;

		updateTerrain				= true;
		updateLighting				= true;
		activeBokeh					= 1;
		activeBuffer				= 0;
		activeMenu					= 5;
		csmLight.direction			= glm::vec3(0,0,-1);

		#if ENABLE_BOKEH_STATISTICS
		bokehQuery					= false;
		bokehRecord					= false;
		bokehFile.open("BokehPerformances.dat");
		#endif
	}
}
//------------------------------------------------------------------------------
bool resize(int _w, int _h)
{
	return true;
}
//------------------------------------------------------------------------------
bool begin()
{
	assert(glf::CheckGLVersion(MAJOR_VERSION,MINOR_VERSION));

	glClearColor(0.f,0.f,0.f,0.f);
	glClearDepthf(1.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glf::CheckError("begin");

	// Load configuration
	glf::io::ConfigLoader loader;
	glf::io::ConfigNode* root	= loader.Load("../resources/configs/config.json");

	DOFParams dofParams;
	glf::io::ConfigNode *dofNode= loader.GetNode(root,"dof");
	dofParams.nSamples 			= loader.GetInt(dofNode,"nSamples",24);
	dofParams.poissonFiltering 	= loader.GetBool(dofNode,"poissonFiltering",false);
	dofParams.nearStart 		= loader.GetFloat(dofNode,"nearStart",0.01f);
	dofParams.nearEnd 			= loader.GetFloat(dofNode,"nearEnd",3.f);
	dofParams.farStart 			= loader.GetFloat(dofNode,"farStart",10.f);
	dofParams.farEnd 			= loader.GetFloat(dofNode,"farEnd",20.f);
	dofParams.maxCoCRadius 		= loader.GetFloat(dofNode,"maxCoCRadius",10.f);
	dofParams.maxBokehRadius 	= loader.GetFloat(dofNode,"maxBokehRadius",15.f);
	dofParams.lumThreshold 		= loader.GetFloat(dofNode,"lumThreshold",5000.f);
	dofParams.cocThreshold 		= loader.GetFloat(dofNode,"cocThreshold",3.5f);
	dofParams.bokehDepthCutoff 	= loader.GetFloat(dofNode,"bokehDepthCutoff",1.f);
	dofParams.enable			= true;

	SkyParams skyParams;
	glf::io::ConfigNode *skyNode= loader.GetNode(root,"sky");
	skyParams.turbidity 		= loader.GetInt(skyNode,"turbidity",2);
	skyParams.sunTheta 			= loader.GetFloat(skyNode,"sunTheta",0.63f);
	skyParams.sunPhi 			= loader.GetFloat(skyNode,"sunPhi",5.31f);
	skyParams.sunFactor 		= loader.GetFloat(skyNode,"sunFactor",3.5f);

	ToneParams toneParams;
	glf::io::ConfigNode*toneNode= loader.GetNode(root,"tone");
	toneParams.expToneExposure	= loader.GetFloat(toneNode,"expToneExposure",-4.08f);
	toneParams.toneExposure		= pow(10.f,toneParams.expToneExposure);

	CSMParams csmParams;
	glf::io::ConfigNode*csmNode	= loader.GetNode(root,"csm");
	csmParams.nSamples 			= loader.GetInt(csmNode,"nSamples",1);
	csmParams.nCascades			= loader.GetInt(csmNode,"nCascades",4);
	csmParams.resolution		= loader.GetInt(csmNode,"resolution",1024);
	csmParams.bias 				= loader.GetFloat(csmNode,"bias",0.0016f);
	csmParams.aperture 			= loader.GetFloat(csmNode,"aperture",0.f);
	csmParams.cascadeAlpha 		= loader.GetFloat(csmNode,"cascadeAlpha",0.5f);
	csmParams.blendFactor		= loader.GetFloat(csmNode,"blendFactor",1.0f);

	SSAOParams ssaoParams;
	glf::io::ConfigNode*ssaoNode= loader.GetNode(root,"ssao");
	ssaoParams.nSamples 		= loader.GetInt(ssaoNode,"nSamples",16);
	ssaoParams.nTaps 			= loader.GetInt(ssaoNode,"nTaps",1);
	ssaoParams.beta 			= loader.GetFloat(ssaoNode,"beta",10e-04f);
	ssaoParams.epsilon 			= loader.GetFloat(ssaoNode,"epsilon",0.0722f);
	ssaoParams.sigma 			= loader.GetFloat(ssaoNode,"sigma",1.f);
	ssaoParams.kappa 			= loader.GetFloat(ssaoNode,"kappa",1.f);
	ssaoParams.radius 			= loader.GetFloat(ssaoNode,"radius",1.f);
	ssaoParams.sigmaScreen 		= loader.GetFloat(ssaoNode,"sigmaScreen",1.f);
	ssaoParams.sigmaDepth 		= loader.GetFloat(ssaoNode,"sigmaDepth",1.f);

	TerrainParams terrainParams;
	glf::io::ConfigNode*terrainNode= loader.GetNode(root,"terrain");
	terrainParams.tileResolution= loader.GetInt(terrainNode,"tileResolution",32);
	terrainParams.tessFactor 	= loader.GetFloat(ssaoNode,"tessFactor",16.f);
	terrainParams.projFactor 	= loader.GetFloat(ssaoNode,"projFactor",10.f);

	ctx::camera 				= glf::Camera::Ptr(new glf::HybridCamera());
	glf::manager::timings		= glf::TimingManager::Create();
	glf::manager::helpers		= glf::HelperManager::Create();
	app 						= new Application(	ctx::window.Size.x,
													ctx::window.Size.y,
													skyParams,
													toneParams,
													csmParams,
													ssaoParams,
													dofParams,
													terrainParams);

	glf::io::LoadScene(	glf::directory::SceneDirectory + "tank.json",
						app->resources,
						app->scene,
						true);

	// Retrive terrain heights
	app->terrainParams.depthFactors.resize(app->scene.terrainMeshes.size());
	for(unsigned int i=0;i<app->terrainParams.depthFactors.size();++i)
		app->terrainParams.depthFactors[i] = app->scene.terrainMeshes[i].heightFactor;

	float farPlane = 2.f * glm::length(app->scene.wBound.pMax - app->scene.wBound.pMin);
	ctx::camera->Perspective(45.f, ctx::window.Size.x, ctx::window.Size.y, 0.1f, farPlane);

	app->renderTarget1.AttachDepthStencil(app->gbuffer.depthTex);
	app->renderTarget2.AttachDepthStencil(app->gbuffer.depthTex);
	app->renderTarget3.AttachDepthStencil(app->gbuffer.depthTex);

	glf::manager::helpers->CreateReferential(1.f);

	#if ENABLE_OBJECT_BBOX_HELPERS
	for(unsigned int i=0;i<app->scene.oBounds.size();++i)
	{
		glf::manager::helpers->CreateBound(	app->scene.oBounds[i],
											app->scene.transformations[i]);
	}
	for(unsigned int i=0;i<app->scene.tBounds.size();++i)
	{
		glf::manager::helpers->CreateBound(	app->scene.tBounds[i]);
	}
	#endif

	glf::CheckError("initScene::end");

	return glf::CheckError("begin");
}
//------------------------------------------------------------------------------
bool end()
{
	return glf::CheckError("end");
}
//------------------------------------------------------------------------------
void gui()
{
	static char labelBuffer[512];
	static glui::Rect none(0,0,200,20);
	static glui::Rect frameRect(0,0,200,10);
	static glui::Rect sliderRect(0, 0, 200, 12);

	ctx::ui->Begin();

		ctx::ui->BeginGroup(glui::Flags::Grow::DOWN_FROM_LEFT);
			ctx::ui->BeginFrame();
			for(int i=0;i<bufferType::MAX;++i)
			{
				bool active = i==app->activeBuffer;
				ctx::ui->CheckButton(none,bufferNames[i],&active);
				app->activeBuffer = active?i:app->activeBuffer;
			}
			ctx::ui->EndFrame();

			ctx::ui->BeginFrame();
			for(int i=0;i<menuType::MAX;++i)
			{
				bool active = i==app->activeMenu;
				ctx::ui->CheckButton(none,menuNames[i],&active);
				app->activeMenu = active?i:app->activeMenu;
			}
			ctx::ui->EndFrame();
			ctx::ui->CheckButton(none,"Helpers",&ctx::drawHelpers);
			ctx::ui->CheckButton(none,"Wire frame",&ctx::drawWire);
		ctx::ui->EndGroup();

		bool update = false;
		ctx::ui->BeginGroup(glui::Flags::Grow::DOWN_FROM_RIGHT);
			ctx::ui->BeginFrame();

			if(app->activeMenu == menuType::MN_SKY)
			{
				sprintf(labelBuffer,"Sun (%.2f,%.2f)",app->skyParams.sunTheta,app->skyParams.sunPhi);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,0.5f*M_PI,&app->skyParams.sunTheta);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,2.f*M_PI,&app->skyParams.sunPhi);

				float fturbidity		= float(app->skyParams.turbidity);
				sprintf(labelBuffer,"Turbidity : %d",app->skyParams.turbidity);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,2.f,10.f,&fturbidity);
				app->skyParams.turbidity = int(fturbidity);

				sprintf(labelBuffer,"Factor : %f",app->skyParams.sunFactor);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,1.f,100.f,&app->skyParams.sunFactor);

				if(update)
				{
					app->updateLighting = true;
				}
			}

			if(app->activeMenu == menuType::MN_CSM)
			{
				sprintf(labelBuffer,"BlendFactor: %f",app->csmParams.blendFactor);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->csmParams.blendFactor);

				sprintf(labelBuffer,"Alpha : %f",app->csmParams.cascadeAlpha);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->csmParams.cascadeAlpha);

				sprintf(labelBuffer,"Bias: %f",app->csmParams.bias);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,0.01f,&app->csmParams.bias);

				sprintf(labelBuffer,"Aperture: %f",app->csmParams.aperture);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,6.f,&app->csmParams.aperture);

				float fnSamples = float(app->csmParams.nSamples);
				sprintf(labelBuffer,"nSamples: %d",app->csmParams.nSamples);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,1.f,32.f,&fnSamples);
				app->csmParams.nSamples = int(fnSamples);
			}


			if(app->activeMenu == menuType::MN_SSAO)
			{
				sprintf(labelBuffer,"Beta : %.4f",app->ssaoParams.beta);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->ssaoParams.beta);

				sprintf(labelBuffer,"Kappa : %.4f",app->ssaoParams.kappa);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->ssaoParams.kappa);

				sprintf(labelBuffer,"Epsilon : %.4f",app->ssaoParams.epsilon);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->ssaoParams.epsilon);

				sprintf(labelBuffer,"Sigma : %.4f",app->ssaoParams.sigma);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->ssaoParams.sigma);

				sprintf(labelBuffer,"Radius : %.4f",app->ssaoParams.radius);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,3.f,&app->ssaoParams.radius);

				float fnSamples = float(app->ssaoParams.nSamples);
				sprintf(labelBuffer,"nSamples : %d",app->ssaoParams.nSamples);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,1.f,32.f,&fnSamples);
				app->ssaoParams.nSamples = int(fnSamples);

				sprintf(labelBuffer,"Sigma Screen : %.4f",app->ssaoParams.sigmaScreen);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,3.f,&app->ssaoParams.sigmaScreen);

				sprintf(labelBuffer,"Sigma Depth : %.4f",app->ssaoParams.sigmaDepth);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.f,1.f,&app->ssaoParams.sigmaDepth);

				float fnTaps = float(app->ssaoParams.nTaps);
				sprintf(labelBuffer,"nTaps : %d",app->ssaoParams.nTaps);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,8.f,&fnTaps);
				app->ssaoParams.nTaps = int(fnTaps);
			}

			if(app->activeMenu == menuType::MN_TONE)
			{
				sprintf(labelBuffer,"Tone Exposure : 10^%.2f",app->toneParams.expToneExposure);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,-6.f,6.f,&app->toneParams.expToneExposure);
				app->toneParams.toneExposure = pow(10.f,app->toneParams.expToneExposure);
			}

			if(app->activeMenu == menuType::MN_DOF)
			{
				ctx::ui->CheckButton(none,"Activate",&app->dofParams.enable);

				sprintf(labelBuffer,"Near Start : %.2f",app->dofParams.nearStart);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.1f,5.f,&app->dofParams.nearStart);

				sprintf(labelBuffer,"Near End : 10^%.2f",app->dofParams.nearEnd);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.1f,5.f,&app->dofParams.nearEnd);

				sprintf(labelBuffer,"Far Start : %.2f",app->dofParams.farStart);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,1.f,100.f,&app->dofParams.farStart);

				sprintf(labelBuffer,"Far End : %.2f",app->dofParams.farEnd);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,1.f,100.f,&app->dofParams.farEnd);

				sprintf(labelBuffer,"Max CoC Radius : %.2f",app->dofParams.maxCoCRadius);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,1.f,30.f,&app->dofParams.maxCoCRadius);

				sprintf(labelBuffer,"Max Bokeh Radius : %.2f",app->dofParams.maxBokehRadius);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,1.f,30.f,&app->dofParams.maxBokehRadius);

				float fnSamples = float(app->dofParams.nSamples);
				sprintf(labelBuffer,"nSamples : %d",app->dofParams.nSamples);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,1.f,32.f,&fnSamples);
				app->dofParams.nSamples = int(fnSamples);

				sprintf(labelBuffer,"Lum. Threshold : %.0f",app->dofParams.lumThreshold);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,100.0f,15000.1f,&app->dofParams.lumThreshold);

				sprintf(labelBuffer,"CoC. Threshold : %.2f",app->dofParams.cocThreshold);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,1.0f,30.f,&app->dofParams.cocThreshold);

				sprintf(labelBuffer,"Bokeh depth cutoff : %.2f",app->dofParams.bokehDepthCutoff);
				ctx::ui->Label(none,labelBuffer);
				ctx::ui->HorizontalSlider(sliderRect,0.001f,1.f,&app->dofParams.bokehDepthCutoff);

				ctx::ui->CheckButton(none,"Poisson filtering",&app->dofParams.poissonFiltering);

				// Change bokeh shape
				int previousActiveBokeh = app->activeBokeh;
				for(int i=0;i<bokehType::MAX;++i)
				{
					bool active = i==app->activeBokeh;
					ctx::ui->CheckButton(none,bokehNames[i],&active);
					app->activeBokeh = active?i:app->activeBokeh;
				}
				if(previousActiveBokeh != app->activeBokeh)
				{
					app->dofProcessor.BokehTexture(glf::directory::TextureDirectory + bokehNames[app->activeBokeh] + std::string("Bokeh.png"));
				}

				#if ENABLE_BOKEH_STATISTICS
				if(ctx::ui->Button(none,"Bokeh query"))  app->bokehQuery = true;
				if(ctx::ui->Button(none,"Bokeh record")) app->bokehRecord = true;
				#endif
			}

			if(app->activeMenu == menuType::MN_TERRAIN)
			{
				update = false;
				float tileExp = floor(log2(app->terrainParams.tileResolution));
				sprintf(labelBuffer,"Tile resolution : %d",app->terrainParams.tileResolution);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,1.f,10.f,&tileExp);
				app->terrainParams.tileResolution = int(pow(2.f,floor(tileExp)));

				for(unsigned int i=0;i<app->terrainParams.depthFactors.size();++i)
				{
					sprintf(labelBuffer,"Depth factor [%d]: %f",i,app->terrainParams.depthFactors[i]);
					ctx::ui->Label(none,labelBuffer);
					update |= ctx::ui->HorizontalSlider(sliderRect,-10.f,10.f,&app->terrainParams.depthFactors[i]);
				}

				sprintf(labelBuffer,"Tesselation factor : %f",app->terrainParams.tessFactor);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,32.f,&app->terrainParams.tessFactor);

				sprintf(labelBuffer,"Projection factor : %f",app->terrainParams.projFactor);
				ctx::ui->Label(none,labelBuffer);
				update |= ctx::ui->HorizontalSlider(sliderRect,0.f,32.f,&app->terrainParams.projFactor);

				if(update)
				{
					app->updateTerrain = true;
				}
			}

			ctx::ui->EndFrame();
		ctx::ui->EndGroup();
	ctx::ui->End();

	glf::CheckError("Interface");
}
//------------------------------------------------------------------------------
void display()
{
	glf::manager::timings->StartSection(glf::section::Frame);

	// Optimize far plane
	glm::mat4 projection		= ctx::camera->Projection();
	glm::mat4 view				= ctx::camera->View();
	float nearValue				= ctx::camera->Near();
	glm::vec3 viewPos			= ctx::camera->Eye();

	// Update lighting if needed
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	if(app->updateLighting)
	{
		app->skyBuilder.SetSunFactor(app->skyParams.sunFactor);
		app->skyBuilder.SetPosition(app->skyParams.sunTheta,app->skyParams.sunPhi);
		app->skyBuilder.SetTurbidity(float(app->skyParams.turbidity));
		app->skyBuilder.Build(app->probeLight.cubeTex);
		app->probeBuilder.Filter(app->probeLight);
		float sunLuminosity = glm::max(glm::dot(app->skyBuilder.sunIntensity, glm::vec3(0.299f, 0.587f, 0.114f)), 0.0001f);

		glm::vec3 dir;
		dir.x = -sin(app->skyParams.sunTheta)*cos(app->skyParams.sunPhi);
		dir.y = -sin(app->skyParams.sunTheta)*sin(app->skyParams.sunPhi);
		dir.z = -cos(app->skyParams.sunTheta);
		app->csmLight.SetDirection(dir);
		app->csmLight.SetIntensity(glm::vec3(sunLuminosity));

		app->updateLighting = false;
	}

	// Update terrain if needed
	if(app->updateTerrain)
	{
		for(unsigned int i=0;i<app->scene.terrainMeshes.size();++i)
		{
			app->scene.terrainMeshes[i].Tesselation(app->terrainParams.tileResolution,
													app->terrainParams.depthFactors[i],
													app->terrainParams.tessFactor,
													app->terrainParams.projFactor);
			app->terrainBuilder.BuildNormals(		app->scene.terrainMeshes[i].heightTex,
													app->scene.terrainMeshes[i].normalTex,
													app->scene.terrainMeshes[i].terrainSize,
													app->terrainParams.depthFactors[i]);
		}

		app->updateTerrain = false;
	}

	// Enable writting into the depth buffer
	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glf::manager::timings->StartSection(glf::section::CsmBuilder);
	app->csmBuilder.Draw(	app->csmLight,
							*ctx::camera,
							app->csmParams.cascadeAlpha,
							app->csmParams.blendFactor,
							app->scene);
	glf::manager::timings->EndSection(glf::section::CsmBuilder);

	// Enable writting into the stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	if(ctx::drawWire) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glf::manager::timings->StartSection(glf::section::Gbuffer);
	app->gbuffer.Draw(		projection,
							view,
							app->scene);
	glf::manager::timings->EndSection(glf::section::Gbuffer);
	if(ctx::drawWire) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);

	// Disable writting into the stencil buffer
	// And activate stencil comparison
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	switch(app->activeBuffer)
	{
		case bufferType::GB_COMPOSITION : 
				glBindFramebuffer(GL_FRAMEBUFFER,app->renderTarget1.framebuffer);
				glClear(GL_COLOR_BUFFER_BIT);

				// Render cube map
				glDisable(GL_STENCIL_TEST);
				glCullFace(GL_FRONT);
				app->cubeMap.Draw(	projection,
									view,
									app->probeLight.cubeTex);
				glCullFace(GL_BACK);
				glEnable(GL_STENCIL_TEST);

				// Render sky lighting
				glf::manager::timings->StartSection(glf::section::SkyRender);
				app->probeRenderer.Draw(app->probeLight,
										app->gbuffer,
										viewPos,
										app->renderTarget1);
				glf::manager::timings->EndSection(glf::section::SkyRender);

				glBindFramebuffer(GL_FRAMEBUFFER,app->renderTarget2.framebuffer);
				glClear(GL_COLOR_BUFFER_BIT);

				// Render ssao::ssao pass
				glf::manager::timings->StartSection(glf::section::SsaoRender);
				app->ssao.Draw(			app->gbuffer,
										view,
										nearValue,
										app->ssaoParams.beta,
										app->ssaoParams.epsilon,
										app->ssaoParams.kappa,
										app->ssaoParams.sigma,
										app->ssaoParams.radius,
										app->ssaoParams.nSamples,
										app->renderTarget2);
				glf::manager::timings->EndSection(glf::section::SsaoRender);

				glBindFramebuffer(GL_FRAMEBUFFER,app->renderTarget3.framebuffer);

				// Render ssao::bilateral pass1
				glf::manager::timings->StartSection(glf::section::SsaoBlur);
				app->ssao.Draw(			app->renderTarget2.texture,
										app->gbuffer.positionTex,
										view,
										app->ssaoParams.sigmaScreen,
										app->ssaoParams.sigmaDepth,
										app->ssaoParams.nTaps,
										glm::vec2(1,0),
										app->renderTarget3);

				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc( GL_ZERO, GL_SRC_ALPHA); // Do a multiplication between SSAO and sky lighting

				glBindFramebuffer(GL_FRAMEBUFFER,app->renderTarget1.framebuffer);

				// Render ssao::bilateral pass2
				app->ssao.Draw(			app->renderTarget3.texture,
										app->gbuffer.positionTex,
										view,
										app->ssaoParams.sigmaScreen,
										app->ssaoParams.sigmaDepth,
										app->ssaoParams.nTaps,
										glm::vec2(0,1),
										app->renderTarget1);
				glf::manager::timings->EndSection(glf::section::SsaoBlur);

				glBlendFunc( GL_ONE, GL_ONE);

				// Render csm/sun light pass
				glf::manager::timings->StartSection(glf::section::CsmRender);
				app->csmRenderer.Draw(	app->csmLight,
										app->gbuffer,
										viewPos,
										app->csmParams.blendFactor,
										app->csmParams.bias,
										app->renderTarget1);
				glf::manager::timings->EndSection(glf::section::CsmRender);

				glBindFramebuffer(GL_FRAMEBUFFER,0);

				glDisable(GL_STENCIL_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);

				// Render dof processing pass
				glf::manager::timings->StartSection(glf::section::DofProcess);
				if(app->dofParams.enable)
				app->dofProcessor.Draw(	app->renderTarget1.texture,
										app->gbuffer.positionTex,
										view,
										app->dofParams.nearStart,
										app->dofParams.nearEnd,
										app->dofParams.farStart,
										app->dofParams.farEnd,
										app->dofParams.maxCoCRadius,
										app->dofParams.maxBokehRadius,
										app->dofParams.nSamples,
										app->dofParams.lumThreshold,
										app->dofParams.cocThreshold,
										app->dofParams.bokehDepthCutoff,
										app->dofParams.poissonFiltering,
										app->renderTarget2);
				glf::manager::timings->EndSection(glf::section::DofProcess);
				
				// Record performances
				#if ENABLE_BOKEH_STATISTICS
				if(app->bokehQuery)
				{
					glf::Info("nBokehs : %d",app->dofProcessor.GetDetectedBokehs());
					app->bokehQuery = false;
				}
				if(app->bokehRecord)
				{
					app->bokehFile <<
					app->dofProcessor.GetDetectedBokehs() << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofReset) << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofBlurDepth) << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofDetection) << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofBlur) << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofSynchronization) << " " <<
					glf::manager::timings->GPUTiming(glf::section::DofRendering) << std::endl;
					app->bokehRecord = false;
				}
				#endif

				glBindFramebuffer(GL_FRAMEBUFFER,0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);

				// Render post processing pass
				glf::manager::timings->StartSection(glf::section::PostProcess);
				if(app->dofParams.enable)
					app->postProcessor.Draw(app->renderTarget2.texture,
											app->toneParams.toneExposure,
											app->renderTarget1);
				else
					app->postProcessor.Draw(app->renderTarget1.texture,
											app->toneParams.toneExposure,
											app->renderTarget2);
				glf::manager::timings->EndSection(glf::section::PostProcess);
				break;
		case bufferType::GB_POSITION :
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glBindFramebuffer(GL_FRAMEBUFFER,0);
				glDepthMask(true);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  | GL_STENCIL_BUFFER_BIT);
				app->renderSurface.Draw(app->gbuffer.positionTex);
				break;
		case bufferType::GB_NORMAL : 
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				app->renderSurface.Draw(app->gbuffer.normalTex);
				break;
		case bufferType::GB_DIFFUSE : 
				glDisable(GL_STENCIL_TEST);
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_BLEND);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				app->renderSurface.Draw(app->gbuffer.diffuseTex);
				break;
		default: assert(false);
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(ctx::drawHelpers)
		app->helperRenderer.Draw(projection,view,glf::manager::helpers->helpers);
	if(ctx::drawUI) 
		gui();
	if(ctx::drawTimings) 
		app->timingRenderer.Draw(*glf::manager::timings);
	glDisable(GL_BLEND);

	glf::CheckError("display");
	glf::SwapBuffers();

	glf::manager::timings->EndSection(glf::section::Frame);
}
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	glf::Info("Start");
	if(glf::Run(argc, 
				argv,
				glm::ivec2(ctx::window.Size.x,ctx::window.Size.y), 
				MAJOR_VERSION, 
				MINOR_VERSION))
				return 0;
	return 1;
}
