#ifndef STAGE_RENDER_H
#define STAGE_RENDER_H

#include "stage.h"
#include "glContext.h"

/// The rendering stage
class StageRender : public Stage
{
private:
    GLuint _iboID;
    GLuint _vboID;
    GLuint _fboID;
    GLhandleARB _fragmentShader;
    GLhandleARB _program;
    GLint _maxIterationsLocation;
    unsigned int _maxIterations;

public:
    StageRender();
    virtual ~StageRender();

    bool init(const commandLineOptions *options);
    bool cleanup();

    bool preExecution();
    bool execute(unsigned int stream);
    bool postExecution();
};

#endif // STAGE_RENDER_H