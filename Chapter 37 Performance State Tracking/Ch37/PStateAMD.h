#pragma once
#include "pstate.h"
#include <winbase.h>

#include "ADL_Include/adl_defines.h"
#include "ADL_Include/adl_sdk.h"
#include "ADL_Include/adl_structures.h"

typedef int ( *ADL_MAIN_CONTROL_CREATE )(ADL_MAIN_MALLOC_CALLBACK, int );
typedef int ( *ADL_MAIN_CONTROL_DESTROY )();
typedef int ( *ADL_OVERDRIVE5_CURRENTACTIVITY_GET ) (int, ADLPMActivity *);

class PStateAMD :
	public PState
{
public:
	PStateAMD(void);
	virtual ~PStateAMD(void);

	virtual void		 GetCurrentPState(PStateStruct& state);
	virtual unsigned int GetCurrentPStateID();
	virtual unsigned int GetGPUUtil();

protected:
	HINSTANCE m_hDLL;		// Handle to DLL
	ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create;
    ADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy;
	ADL_OVERDRIVE5_CURRENTACTIVITY_GET ADL_Overdrive5_CurrentActivity_Get;
	ADLPMActivity m_activity;
};
