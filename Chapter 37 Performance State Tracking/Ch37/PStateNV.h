#pragma once
#include "pstate.h"
// --- NVAPI ---
#include "NVAPI_Include/nvapi.h"
#include "NVAPI_Include/NvApiDriverSettings.h"
#pragma comment( lib, "NVAPI_Include/nvapi.lib")
// --- --- ---
#define UTIL_DOMAIN_GPU	0
#define UTIL_DOMAIN_FB	1
#define UTIL_DOMAIN_VID	2

struct NVVoltages
{
	int domainId;
	int mVolts;
};

struct NVPState
{
	int id;
	int clockGraphics;
	int clockMemory;
	int clockProcessor;
	int voltNo;
	NVVoltages* voltages;
};



class PStateNV :
	public PState
{
public:
	PStateNV(void);
	virtual ~PStateNV(void);

	virtual void		 GetCurrentPState(PStateStruct& state);
	virtual unsigned int GetCurrentPStateID();
	virtual unsigned int GetGPUUtil();
	void GetGPUUtils(unsigned int &gpu, unsigned int &fb, unsigned int &vid);
	void PrintToFile();

protected:
	void GetPStateInfo();

protected:
	NvPhysicalGpuHandle				m_hPhysicalGPU[NVAPI_MAX_PHYSICAL_GPUS];
	NvU32							m_gpuCount;
	NV_GPU_DYNAMIC_PSTATES_INFO_EX	m_DynamicPStateInfo;	
	struct NVPState*				m_pPStates;
	unsigned int					m_nNumPState;

};
