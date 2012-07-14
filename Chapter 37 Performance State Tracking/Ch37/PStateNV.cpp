#include "StdAfx.h"
#include "PStateNV.h"

PStateNV::PStateNV(void)
{
	NvAPI_Status status = NvAPI_Initialize();
	if(status != NVAPI_OK)
		throw "NVAPI cannot be initialized!";

	NvAPI_EnumPhysicalGPUs(m_hPhysicalGPU, &m_gpuCount);
	m_DynamicPStateInfo.version = NV_GPU_DYNAMIC_PSTATES_INFO_EX_VER;
	m_nNumPState = 0;
	GetPStateInfo();
}

PStateNV::~PStateNV(void)
{
	for(unsigned int i=0; i< m_nNumPState; i++)
	{
		delete [] m_pPStates[i].voltages;
	}
	delete [] m_pPStates;

	NvAPI_Status status = NvAPI_Unload();
	if(status != NVAPI_OK )
		throw "NVAPI resources are locked. NVAPI cannot be unloaded!";
}

unsigned int PStateNV::GetGPUUtil()
{
	NvAPI_Status status = NvAPI_GPU_GetDynamicPstatesInfoEx(m_hPhysicalGPU[0], &m_DynamicPStateInfo);
	if(status == NVAPI_OK )
		return m_DynamicPStateInfo.utilization[UTIL_DOMAIN_GPU].percentage;
	else
		return 0U;

}

void PStateNV::GetGPUUtils(unsigned int &gpu, unsigned int &fb, unsigned int &vid)
{
	NvAPI_Status status = NvAPI_GPU_GetDynamicPstatesInfoEx(m_hPhysicalGPU[0], &m_DynamicPStateInfo);
	if(status == NVAPI_OK )
	{
		gpu = m_DynamicPStateInfo.utilization[UTIL_DOMAIN_GPU].percentage;
		fb  = m_DynamicPStateInfo.utilization[UTIL_DOMAIN_FB].percentage;
		vid = m_DynamicPStateInfo.utilization[UTIL_DOMAIN_VID].percentage;

	}
	else
		gpu = fb = vid = 0U;

}

unsigned int PStateNV::GetCurrentPStateID()
{
	NV_GPU_PERF_PSTATE_ID currentPState;
	NvAPI_Status status = NvAPI_GPU_GetCurrentPstate(m_hPhysicalGPU[0], &currentPState);
	if(status == NVAPI_OK )
	{
		return (unsigned int) currentPState;
	}
	else
		throw "NvAPI_GPU_GetCurrentPstate error!";
}

void PStateNV::GetPStateInfo()
{
	NV_GPU_PERF_PSTATES_INFO PerfPstatesInfo;
	PerfPstatesInfo.version = NV_GPU_PERF_PSTATES_INFO_VER;
	NvU32  inputFlags = 1U;
	NvAPI_Status status = NvAPI_GPU_GetPstatesInfoEx( m_hPhysicalGPU[0], &PerfPstatesInfo, inputFlags);
	if(status == NVAPI_OK )
	{
		int numPState = PerfPstatesInfo.numPstates;
		int numClocks = PerfPstatesInfo.numClocks;
		int numVoltages = PerfPstatesInfo.numVoltages;
		m_nNumPState = numPState;
		m_pPStates = new struct NVPState[numPState];
		for(int i = 0; i < numPState; i++)
		{
			m_pPStates[i].id = PerfPstatesInfo.pstates[i].pstateId;
			m_pPStates[i].voltNo = numVoltages;
		
			for(int j = 0; j < numClocks; j++)
			{
				int type = PerfPstatesInfo.pstates[i].clocks[j].domainId;
				if(type == NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS)
					m_pPStates[i].clockGraphics = PerfPstatesInfo.pstates[i].clocks[j].freq;
				else if(type == NVAPI_GPU_PUBLIC_CLOCK_MEMORY)
					m_pPStates[i].clockMemory = PerfPstatesInfo.pstates[i].clocks[j].freq;
				else if(type == NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR)
					m_pPStates[i].clockProcessor = PerfPstatesInfo.pstates[i].clocks[j].freq;
			}
			m_pPStates[i].voltages = new struct NVVoltages[numVoltages];
			for(int j = 0; j < numVoltages; j++)
			{
				m_pPStates[i].voltages[j].domainId = PerfPstatesInfo.pstates[i].voltages[j].domainId;
				m_pPStates[i].voltages[j].mVolts = PerfPstatesInfo.pstates[i].voltages[j].mvolt;
			}
		}
		
	}

}
void PStateNV::PrintToFile()
{
	FILE* f;
	errno_t err = fopen_s(&f,"PStates.txt", "w");
	if(err) return;

	for(unsigned int i=0; i< m_nNumPState; i++)
	{
		fprintf(f, "ID:%d\n", m_pPStates[i].id);
		fprintf(f, "clockGraphics:  %7d [kHz]\n", m_pPStates[i].clockGraphics);
		fprintf(f, "clockMemory:    %7d [kHz]\n", m_pPStates[i].clockMemory);
		fprintf(f, "clockProcessor: %7d [kHz]\n", m_pPStates[i].clockProcessor);
		fprintf(f, "Voltages:\n");
		for(int j = 0; j < m_pPStates[i].voltNo; j++)
		{
			fprintf(f, "    Domain ID:%d", m_pPStates[i].voltages[j].domainId);
			fprintf(f, "  %d [mV]\n", m_pPStates[i].voltages[j].mVolts);
		}
		fprintf(f, "------------------------------\n");
	}
	fclose(f);
}

void PStateNV::GetCurrentPState(PStateStruct& state)
{
	state.gpuUtil = GetGPUUtil();
	unsigned int ID = GetCurrentPStateID();
	for(unsigned int i=0; i< m_nNumPState; i++)
	{
		if(ID == m_pPStates[i].id)
		{
			state.ID			= ID;
			state.gpuClock		= m_pPStates[i].clockGraphics;
			state.memClock		= m_pPStates[i].clockMemory;
			state.shaderClock	= m_pPStates[i].clockProcessor;
			state.coreVoltage	= m_pPStates[i].voltages[0].mVolts;
			return;
		}
	}
	return;
}