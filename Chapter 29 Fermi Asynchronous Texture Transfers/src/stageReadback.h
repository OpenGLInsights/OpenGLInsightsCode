#ifndef STAGE_READBACK_H
#define STAGE_READBACK_H

#include "stage.h"
#include "glContext.h"
#include "limits.h"

/// The readback stage
class StageReadback : public Stage
{
private:
    static const unsigned int INVALID_RESULT = 0xFFFFFFFF;
	GLuint _fboID;
    GLuint _packPBOID[2];
    void *_resultBuffer[limits::MAX_STREAMS];

    unsigned int _pboIndex;
	unsigned int _prevResult;

public:
    StageReadback();
    virtual ~StageReadback();

    bool init(const commandLineOptions *options);
    bool cleanup();

    bool preExecution();
    bool execute(unsigned int stream);
    bool postExecution();
};

#endif // STAGE_REEADBACK_H