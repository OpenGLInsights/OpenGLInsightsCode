#ifndef STAGE_UPLOAD_H
#define STAGE_UPLOAD_H

#include "stage.h"
#include "glContext.h"
#include "limits.h"

/// The upload stage
class StageUpload : public Stage
{
private:
    GLuint _unpackPBOID[2];
    void *_imageBuffer[limits::MAX_STREAMS];

    unsigned int _pboIndex;

public:
    StageUpload();
    virtual ~StageUpload();

    bool init(const commandLineOptions *options);
    bool cleanup();

    bool preExecution();
    bool execute(unsigned int stream);
    bool postExecution();
};

#endif // STAGE_UPLOAD_H