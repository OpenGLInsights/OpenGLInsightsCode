#include "stdafx.h"

#include "stage.h"
#include "timer.h"
#include "window.h"

/// Constructor
Stage::Stage()
{
	_done = false;
	_lastStage = false;
}

/// Destructor
Stage::~Stage()
{
}

/// Init a stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
Stage::init(const commandLineOptions *options)
{
	_options = *options;

	return true;
}

/// Cleanup
///
/// @return false if failed
bool
Stage::cleanup()
{
	return true;
}

//TODO
 //Attach an input queue

 //@param index [in] queue index
 //@param queue [in] queue
void
Stage::attachInput(unsigned int index, queue *queue)
{
    _inQueue[index] = queue;
}

/// Attach an output queue
///
/// @param index [in] queue index
/// @param queue [in] queue
void
Stage::attachOutput(unsigned int index, queue *queue)
{
    _outQueue[index] = queue;
}

/// The thread function
///
/// @param dataPtr [in] stage class pointer
DWORD WINAPI stageThreadFunction(LPVOID dataPtr)
{
	Stage *thisStage = (Stage*)dataPtr;

	thisStage->executionLoop();

	return 0;
}

/// Start this stage
///
/// @return false if failed
bool
Stage::start()
{
	_thread = CreateThread(NULL, 0, stageThreadFunction, this, 0, NULL);

	return (_thread != 0);
}

/// The execution looop
void
Stage::executionLoop()
{
	unsigned int stream;

	if(!preExecution())
		return;

	if(_lastStage)
		Timer::getInstance().start();

	// initialize
	while(!_done)
	{
		//TODOVALIDATION
		//        if(_lastStage)
		//          main::getInstance().startFrame();

		for(stream = 0; stream < _options._numStreams; stream++)
		{
			execute(stream);
#ifdef _DEBUG
			if(glGetError() != GL_NO_ERROR)
			{
				window::getInstance().messageBoxError("executionLoop - Detected GL error.");
				break;
			}
#endif
		}

		if(_lastStage)
		{
			_done = !Timer::getInstance().update();
			//    main::getInstance().endFrame(); // no vlaidation needed TODO VALIDATION
		}
	}
	postExecution();
}
