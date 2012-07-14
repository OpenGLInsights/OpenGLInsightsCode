#include "StdAfx.h"
#include "PStateAMD.h"

// Memory allocation function
void* __stdcall ADL_Main_Memory_Alloc ( int iSize )
{
    //void* lpBuffer = malloc ( iSize );
	void* lpBuffer = (void*)(new BYTE[iSize]);
    return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free ( void** lpBuffer )
{
    if ( NULL != *lpBuffer )
    {
		BYTE* pBuff = (BYTE*)(*lpBuffer);
		delete [] pBuff;
        //free ( *lpBuffer );
        *lpBuffer = NULL;
    }
}

PStateAMD::PStateAMD(void)
{
	m_hDLL = LoadLibrary(_T("atiadlxx.dll"));
    if (m_hDLL == NULL)
	{
        // A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
        // Try to load the 32 bit library (atiadlxy.dll) instead
        m_hDLL = LoadLibrary(_T("atiadlxy.dll"));
	}
	if (m_hDLL == NULL)
    {
        throw "ADL library not found!";
        return;
    }
	ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) GetProcAddress(m_hDLL,"ADL_Main_Control_Create");
    ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY) GetProcAddress(m_hDLL,"ADL_Main_Control_Destroy");
    //ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(hDLL,"ADL_Adapter_NumberOfAdapters_Get");
    //ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(hDLL,"ADL_Adapter_AdapterInfo_Get");
    //ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET) GetProcAddress(hDLL,"ADL_Display_DisplayInfo_Get");
	ADL_Overdrive5_CurrentActivity_Get = (ADL_OVERDRIVE5_CURRENTACTIVITY_GET) GetProcAddress(m_hDLL,"ADL_Overdrive5_CurrentActivity_Get");

	if ( ADL_Main_Control_Create == NULL || //OK
         ADL_Main_Control_Destroy == NULL || //OK
      //   ADL_Adapter_NumberOfAdapters_Get == NULL ||
       //  ADL_Adapter_AdapterInfo_Get == NULL ||
       //  ADL_Display_DisplayInfo_Get == NULL ||
		 ADL_Overdrive5_CurrentActivity_Get == NULL) //OK
		{
	       throw "ADL's API is missing!";
		   return;
		}
	
    if ( ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
       throw "ADL Initialization Error!";

	m_activity.iSize = sizeof(ADLPMActivity);
}

PStateAMD::~PStateAMD(void)
{
	//ADL_Main_Memory_Free ( (void**)&lpAdapterInfo );
    //ADL_Main_Memory_Free ( (void**)&lpAdlDisplayInfo );
	ADL_Main_Control_Destroy();
	FreeLibrary(m_hDLL);
}

void PStateAMD::GetCurrentPState(PStateStruct& state)
{
	ADL_Overdrive5_CurrentActivity_Get(0, &m_activity);

	state.ID			= m_activity.iCurrentPerformanceLevel;
	state.gpuClock		= m_activity.iEngineClock * 10;
	state.memClock		= m_activity.iMemoryClock * 10;
	state.shaderClock	= state.gpuClock;
	state.coreVoltage	= m_activity.iVddc;
	state.gpuUtil		= m_activity.iActivityPercent;
}
unsigned int PStateAMD::GetCurrentPStateID()
{
	ADL_Overdrive5_CurrentActivity_Get(0, &m_activity);
	return m_activity.iCurrentPerformanceLevel;
}
unsigned int PStateAMD::GetGPUUtil()
{
	ADL_Overdrive5_CurrentActivity_Get(0, &m_activity);
	return m_activity.iActivityPercent;
}