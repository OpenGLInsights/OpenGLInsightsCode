#ifndef STAGE_CONSUMER_H
#define STAGE_CONSUMER_H

#include "stage.h"
#include "glContext.h"
#include "limits.h"
/// The rendering stage
class stageConsumer : public Stage
{
private:
	GLuint _iboID;
	GLuint _vboID;
	LARGE_INTEGER _frequency;
	LARGE_INTEGER _startCPUTime;

	GLuint _destTexID[limits::MAX_STREAMS];

public:
    stageConsumer();
    virtual ~stageConsumer();

    bool init(const commandLineOptions *options);
    bool cleanup();

    bool preExecution();
	bool startFrame();
    bool execute(unsigned int stream);
	bool endFrame();
    bool postExecution();
};

#endif // STAGE_PRODUCER_H