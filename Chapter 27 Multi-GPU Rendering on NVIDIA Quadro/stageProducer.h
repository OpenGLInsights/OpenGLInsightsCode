#ifndef STAGE_PRODUCER_H
#define STAGE_PRODUCER_H

#include "stage.h"
#include "glContext.h"
#include "limits.h"
/// The rendering stage
class stageProducer : public Stage
{
private:
	GLuint _iboID;
	GLuint _vboID;
    GLuint _fboID;
	unsigned int _frame;
	GLuint _srcTexID[limits::MAX_STREAMS];
    glContext::renderingContext *_destrc;

public:
    stageProducer();
    virtual ~stageProducer();

    bool init(const commandLineOptions *options);
    bool cleanup();

    bool preExecution();
    bool execute(unsigned int stream);
    bool postExecution();

	void setDestRC(const glContext::renderingContext *destRC) {
		_destrc = (glContext::renderingContext *) destRC;
	}
};

#endif // STAGE_PRODUCER_H