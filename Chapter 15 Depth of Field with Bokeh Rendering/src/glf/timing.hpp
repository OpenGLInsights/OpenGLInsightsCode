#ifndef GLF_TIMING_HPP
#define GLF_TIMING_HPP

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <glf/utils.hpp>
#include <glf/memory.hpp>
#include <glf/font.hpp>
#include <vector>

namespace glf
{
	//--------------------------------------------------------------------------
	// Compute the average elapsed time into a given repetitive code section
	// TODO : add a query queue for having only input lag instead of sparse sampling
	class GPUSectionTimer
	{
	public:
					GPUSectionTimer();
					~GPUSectionTimer();
		void		StartSection();	// Indicates the start of the section
		void		EndSection();	// Indicates the end of the section
		float		Timing() const;	// Return the average elapsed time into this section
	private:
		GLuint		id;
		bool		waiting;
		double		current;
	};
	//--------------------------------------------------------------------------
	class CPUSectionTimer
	{
	public:
					CPUSectionTimer();
					~CPUSectionTimer();
		void		StartSection();	// Indicates the start of the section
		void		EndSection();	// Indicates the end of the section
		float		Timing() const;	// Return the average elapsed time into this section
	private:
		int			startTime;
		int			current;
	};
	//--------------------------------------------------------------------------
	namespace section
	{
		inline bool IsGPUSection(		int _section);
		inline bool IsCPUSection(		int _section);
		inline int  ToIndex(			int _section);

		extern int	InvalidSection;
		extern int	MaskSection;
		extern int	GPUSection;
		extern int	CPUSection;

		// CSM inner timings
		extern int	CsmBuilderRegular;
		extern int	CsmBuilderTerrain;
		extern int	CsmBuilderFilter;

		// Dof inner timings
		extern int	DofReset;
		extern int	DofBlurDepth;
		extern int	DofDetection;
		extern int	DofBlur;
		extern int	DofSynchronization;
		extern int	DofRendering;

		// Pass timings
		extern int	Gbuffer;
		extern int	CsmBuilder;
		extern int	CsmRender;
		extern int	SkyRender;
		extern int	SsaoRender;
		extern int	SsaoBlur;
		extern int	DofProcess;
		extern int	PostProcess;

		// Frame timings
		extern int	Frame;
	}
	//--------------------------------------------------------------------------
	class TimingManager
	{
	public:
		typedef SmartPointer<TimingManager> Ptr;
		static Ptr	Create(				);
					TimingManager(		);
					~TimingManager(		);
		void 		StartSection(		int _section);
		void 		EndSection(			int _section);
		float 		GPUTiming(			int _section) const;
		float 		CPUTiming(			int _section) const;
		const std::string& Name(		int _section) const;

	private:
		void 		AddSection(			int& _sectionID, 
										const std::string& _sectionName,
										bool _addGPUSection, 
										bool _addCPUSection);
		std::vector<GPUSectionTimer*>	gpuTimers;
		std::vector<CPUSectionTimer*>	cpuTimers;
		std::vector<std::string>		strTimers;
		int								counter;
	};
	//--------------------------------------------------------------------------
	class TimingRenderer
	{
	public:
					TimingRenderer(		int _w,
										int _h);
		void		Draw(				const TimingManager& _timings);
	private:
		void 		DrawGPULine(		const TimingManager& _timings,
										int _sectionID,
										int _x,
										int _y,
										const glm::vec4& _color,
										char* _buffer);
		void 		DrawCPULine(		const TimingManager& _timings,
										int _sectionID,
										int _x,
										int _y,
										const glm::vec4& _color,
										char* _buffer);
	private:
		Font							font;
		FontRenderer 					fontRenderer;
	};
}

//------------------------------------------------------------------------------
// Include
//------------------------------------------------------------------------------
#include <glf/timing.inl>

#endif
