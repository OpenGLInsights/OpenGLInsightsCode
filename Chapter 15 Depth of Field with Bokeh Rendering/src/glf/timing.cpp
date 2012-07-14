//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <glf/timing.hpp>
#include <glf/window.hpp>
#include <glf/debug.hpp>
#include <glui/arial12.hpp>
#include <cassert>

namespace glf
{
	//-------------------------------------------------------------------------
	namespace
	{
		double timerToFloat(GLuint64 timer) 
		{
			GLuint64 tmp = timer;
			//GLuint64 nsec = tmp % 1000;
			tmp /= 1000;
			GLuint64 mcsec = tmp % 1000;
			tmp /= 1000;
			GLuint64 misec = tmp % 1000;
			tmp /= 1000;
			GLuint64 sec = tmp;
	
			return double(int(sec) * 1000.0 + int(misec) + 0.001 * int(mcsec));
		}
	}
	//-------------------------------------------------------------------------
	GPUSectionTimer::GPUSectionTimer()
	{
		glGenQueries(1, &id);
		waiting = false;
		current = 0;
	}
	//-------------------------------------------------------------------------
	GPUSectionTimer::~GPUSectionTimer()
	{
		glDeleteQueries(1,&id);
	}
	//-------------------------------------------------------------------------
	void GPUSectionTimer::StartSection()
	{
		if(!waiting) 
		{
			glBeginQuery(GL_TIME_ELAPSED, id);
		}
	}
	//-------------------------------------------------------------------------
	void GPUSectionTimer::EndSection()
	{
		// Return result in milliseconds
		if(!waiting) 
		{
			glEndQuery(GL_TIME_ELAPSED);
			waiting = true;
		}

		if(waiting)
		{
			GLint available = 0;
			glGetQueryObjectiv(id, GL_QUERY_RESULT_AVAILABLE, &available);

			if(available)
			{
				GLuint timeElapsed; // /!\ nanoseconds
				glGetQueryObjectuiv(id, GL_QUERY_RESULT, &timeElapsed);
				current = timerToFloat(timeElapsed);
				waiting = false;
			}
		}
	}
	//-------------------------------------------------------------------------
	float GPUSectionTimer::Timing() const
	{
		return float(current);
	}
	//-------------------------------------------------------------------------
	CPUSectionTimer::CPUSectionTimer()
	{
		startTime = 0;
		current = 0;
	}
	//-------------------------------------------------------------------------
	CPUSectionTimer::~CPUSectionTimer()
	{

	}
	//-------------------------------------------------------------------------
	void CPUSectionTimer::StartSection()
	{
		startTime = glutGet(GLUT_ELAPSED_TIME);
	}
	//-------------------------------------------------------------------------
	void CPUSectionTimer::EndSection()
	{
		current = glutGet(GLUT_ELAPSED_TIME) - startTime;
	}
	//-------------------------------------------------------------------------
	float CPUSectionTimer::Timing() const
	{
		return float(current);
	}
	//--------------------------------------------------------------------------
	namespace section
	{
		// General
		int	InvalidSection		= 0;
		int	GPUSection			= 0x80000000;
		int	CPUSection			= 0x40000000;
		int MaskSection			= 0x1FFFFFFF;

		// CSM inner timings
		int	CsmBuilderRegular	= 0;
		int	CsmBuilderTerrain	= 0;
		int	CsmBuilderFilter	= 0;

		// Dof inner timings
		int	DofReset			= 0;
		int	DofBlurDepth		= 0;
		int	DofDetection		= 0;
		int	DofBlur				= 0;
		int	DofSynchronization	= 0;
		int	DofRendering		= 0;

		// Pass timings
		int	Gbuffer				= 0;
		int	CsmBuilder			= 0;
		int	CsmRender			= 0;
		int	SkyRender			= 0;
		int	SsaoRender			= 0;
		int	SsaoBlur			= 0;
		int	DofProcess			= 0;
		int	PostProcess			= 0;

		// Frame timings
		int	Frame				= 0;
	}
	//--------------------------------------------------------------------------
	TimingManager::Ptr TimingManager::Create()
	{
		return Ptr(new TimingManager());
	}
	//--------------------------------------------------------------------------
	TimingManager::TimingManager():
	counter(0)
	{
		gpuTimers.resize(64,NULL);
		cpuTimers.resize(64,NULL);
		strTimers.resize(64,"");

		#if ENABLE_GPU_PASSES_TIMING
			AddSection(section::Gbuffer,			"GBuffer",				true,false);

			#if ENABLE_CSM_PASS_TIMING
			AddSection(section::CsmBuilderRegular,	"CSM Builder Regular",	true,false);
			AddSection(section::CsmBuilderTerrain,	"CSM Builder Terrain",	true,false);
			AddSection(section::CsmBuilderFilter,	"CSM Builder Filter",	true,false);
			#else
			AddSection(section::CsmBuilder,			"CSM Builder",			true,false);
			#endif 

			AddSection(section::CsmRender,			"CSM Render",			true,false);
			AddSection(section::SkyRender,			"Sky Render",			true,false);
			AddSection(section::SsaoRender,			"SSAO Render",			true,false);
			AddSection(section::SsaoBlur,			"SSAO Blur",			true,false);
			#if ENABLE_DOF_PASS_TIMING
			AddSection(section::DofReset,			"DOF Reset",			true,false);
			AddSection(section::DofBlurDepth,		"DOF BlurDepth",		true,false);
			AddSection(section::DofDetection,		"DOF Detection",		true,false);
			AddSection(section::DofBlur,			"DOF Blur",				true,false);
			AddSection(section::DofSynchronization,	"DOF Synchronization",	true,false);
			AddSection(section::DofRendering,		"DOF Rendering",		true,false);
			#else
			AddSection(section::DofProcess,			"DOF Process",			true,false);
			#endif 
			AddSection(section::PostProcess,		"Post Process",			true,false);
		#endif

		#if ENABLE_GPU_FRAME_TIMING
		AddSection(section::Frame,				"Frame",				true,true);
		#else
		AddSection(section::Frame,				"Frame",				false,true);
		#endif
	}
	//--------------------------------------------------------------------------
	void TimingManager::AddSection(		int& _section,
										const std::string& _name,
										bool _gpu,
										bool _cpu)
	{
		if(_gpu || _cpu) 
		{
			_section = counter++;
			assert(counter<=int(gpuTimers.size()));
			assert(counter<=int(cpuTimers.size()));
			assert(counter<=int(strTimers.size()));

			strTimers[_section] = _name;

			if(_gpu)
			{
				_section |= section::GPUSection;
				gpuTimers[section::ToIndex(_section)] = new GPUSectionTimer();
			}

			if(_cpu)
			{
				_section |= section::CPUSection;
				cpuTimers[section::ToIndex(_section)] = new CPUSectionTimer();
			}
		}
		else
			_section = section::InvalidSection;
	}
	//--------------------------------------------------------------------------
	TimingManager::~TimingManager()
	{
		for(unsigned int i=0;i<gpuTimers.size();++i)
			delete gpuTimers[i];
		for(unsigned int i=0;i<cpuTimers.size();++i)
			delete cpuTimers[i];
	}
	//--------------------------------------------------------------------------
	void TimingManager::StartSection(int _section)
	{
		if(section::IsGPUSection(_section)) gpuTimers[section::ToIndex(_section)]->StartSection();
		if(section::IsCPUSection(_section)) cpuTimers[section::ToIndex(_section)]->StartSection();
	}
	//--------------------------------------------------------------------------
	void TimingManager::EndSection(int _section)
	{
		if(section::IsGPUSection(_section)) gpuTimers[section::ToIndex(_section)]->EndSection();
		if(section::IsCPUSection(_section)) cpuTimers[section::ToIndex(_section)]->EndSection();
	}
	//--------------------------------------------------------------------------
	float TimingManager::GPUTiming(int _section) const
	{
		if(section::IsGPUSection(_section))
			return gpuTimers[section::ToIndex(_section)]->Timing();
		else
			return 0;
	}
	//--------------------------------------------------------------------------
	float TimingManager::CPUTiming(int _section) const
	{
		if(section::IsCPUSection(_section))
			return cpuTimers[section::ToIndex(_section)]->Timing();
		else
			return 0;
	}
	//--------------------------------------------------------------------------
	const std::string& TimingManager::Name(int _section) const
	{
		assert(section::ToIndex(_section) < int(gpuTimers.size()));
		return strTimers[section::ToIndex(_section)];
	}
	//--------------------------------------------------------------------------
	TimingRenderer::TimingRenderer(int _w, int _h):
	font(),
	fontRenderer(_w,_h)
	{
		font.Load<glui::Arial12>();
	}
	//--------------------------------------------------------------------------
	void TimingRenderer::DrawGPULine(	const TimingManager& _timings,
										int _sectionID,
										int _x,
										int _y,
										const glm::vec4& _color,
										char* _buffer)
	{
		float timing = _timings.GPUTiming(_sectionID);
		sprintf(_buffer,"[GPU] %s : %.2fms",_timings.Name(_sectionID).c_str(),timing);
		fontRenderer.Draw(_x,_y,font,_buffer,_color);
	}
	//--------------------------------------------------------------------------
	void TimingRenderer::DrawCPULine(	const TimingManager& _timings,
										int _sectionID,
										int _x,
										int _y,
										const glm::vec4& _color,
										char* _buffer)
	{
		float timing = _timings.CPUTiming(_sectionID);
		sprintf(_buffer,"[CPU] %s : %.2fms",_timings.Name(_sectionID).c_str(),timing);
		fontRenderer.Draw(_x,_y,font,_buffer,_color);
	}
	//--------------------------------------------------------------------------
	void TimingRenderer::Draw(			const TimingManager& _timings)
	{
		static char buffer[128];

		// Bottom left corner
		glm::vec4 color(1,0,0,1);
		int x,y,verticalOffset;
		x				= 20;
		y				= 20;
		verticalOffset	= font.CharHeight('A') + 2;

		#if ENABLE_GPU_PASSES_TIMING
			DrawGPULine(_timings,section::PostProcess,			x,y,color,buffer); y+=verticalOffset;

			#if ENABLE_DOF_PASS_TIMING
			DrawGPULine(_timings,section::DofRendering,			x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::DofSynchronization,	x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::DofBlur,				x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::DofDetection,			x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::DofBlurDepth,			x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::DofReset,				x,y,color,buffer); y+=verticalOffset;
			#else
			DrawGPULine(_timings,section::DofProcess,			x,y,color,buffer); y+=verticalOffset;
			#endif

			DrawGPULine(_timings,section::SsaoBlur,				x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::SsaoRender,			x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::SkyRender,			x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::CsmRender,			x,y,color,buffer); y+=verticalOffset;

			#if ENABLE_CSM_PASS_TIMING
			DrawGPULine(_timings,section::CsmBuilderFilter,		x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::CsmBuilderTerrain,	x,y,color,buffer); y+=verticalOffset;
			DrawGPULine(_timings,section::CsmBuilderRegular,	x,y,color,buffer); y+=verticalOffset;
			#else
			DrawGPULine(_timings,section::CsmBuilder,			x,y,color,buffer); y+=verticalOffset;
			#endif

			DrawGPULine(_timings,section::Gbuffer,				x,y,color,buffer); y+=verticalOffset;
		#endif

		#if ENABLE_GPU_FRAME_TIMING
		DrawGPULine(_timings,section::Frame,				x,y,color,buffer); y+=verticalOffset;
		DrawCPULine(_timings,section::Frame,				x,y,color,buffer); y+=verticalOffset;
		#else
		DrawCPULine(_timings,section::Frame,				x,y,color,buffer); y+=verticalOffset;
		#endif

		// Draw FPS
		sprintf(buffer,"%.2f FPS",1000.f/_timings.CPUTiming(section::Frame));
		fontRenderer.Draw(x,y,font,buffer,color);
	}
}

