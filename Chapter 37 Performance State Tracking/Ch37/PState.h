#pragma once

struct PStateS
{
	unsigned int ID;
	unsigned int gpuClock;
	unsigned int memClock;
	unsigned int shaderClock;
	unsigned int coreVoltage;
	unsigned int gpuUtil;
};

typedef struct PStateS PStateStruct;

class PState
{
public:
	PState(void);
	virtual ~PState(void);

	//virtual void		 GetPStatesInfo()	 {}
	virtual void		 GetCurrentPState(PStateStruct& state){}
	virtual unsigned int GetGPUUtil()		 { return 0;}
	virtual unsigned int GetCurrentPStateID(){ return 0;}
};
