#ifndef STAGE_H
#define STAGE_H
#include "glContext.h"
#include "commandLine.h"
#include "queue.h"
#include <WinDef.h>

//A stage is a part of the pipeline. A stage encapsulate tasks in a separate thread that need a dedicated OGL context 
//To interact with other stages use  fence & cpu event TODO elaborate
//Usage : 
//1. App constructs a stage
//2. Calls init which creates an OGL context and allocates all the buffers needed for that context
//3. Calls start() - creates the thread for this stage
class Stage
{
	bool _done; //are we done with this stage?
	bool _lastStage; //is this the last stage so that we can do any Timer calculations if necessary
	HANDLE _thread; //thread that executes this stage
	friend DWORD WINAPI stageThreadFunction(LPVOID dataPtr) ;
	//called by the stageThreadFunction created in start()
	void executionLoop();

protected:
	commandLineOptions _options;
	//the # queues depends on how many dependencies we have, 
	// eg render has to wait for upload and readback to finish, so it needs 2 queues
	//upload and readback only need 1 queue, but we just instantiate to 2 for simplicity
	queue *_inQueue[2];                 ///< input queues, 
    queue *_outQueue[2];                ///< output queuese
	//GL context for this stage
    glContext::renderingContext *_rc;

	//CHild classes must define these, called by exectionLoop()
	/// Called before the execution starts, done once
    virtual bool preExecution() = 0;
    /// The stage work function that is done in a loop repeatedly
    virtual bool execute(unsigned int stream) = 0;
    /// Called after loop execution finished, done once
    virtual bool postExecution() = 0;

public:

	//TODO set laststage
	Stage();
	virtual ~Stage();
	
	//init, creates context and does initialization
	virtual bool init(const commandLineOptions *options);
	//Creates a thread for this stage calling staeThreadFunction
	bool start() ;
	virtual bool cleanup();;

    void attachInput(unsigned int index, queue *queue);
    void attachOutput(unsigned int index, queue *queue);
    /// Indicate that this is the last stage
    void setLastStage() { _lastStage = true; }

};




#endif // STAGE_H