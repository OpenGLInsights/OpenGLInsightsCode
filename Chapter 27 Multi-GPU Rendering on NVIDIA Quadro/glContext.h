#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

#include "commandLine.h"

/// OpenGL context abstraction

class glContext
{
public:
    /// A OpenGL rendering context
    class renderingContext
    {
        friend class glContext;
    private:
        HDC _hDC;
        HGLRC _hGLRC;
		bool _isDummy;
    public:
        renderingContext();
        ~renderingContext();

        /// Get the DC handle
        ///
        /// @return DC handle
        HDC hDC() const { return _hDC; };
        /// Get the RC handle
        ///
        /// @return RC handle
        HGLRC hGLRC() const { return _hGLRC; };

    };

private:
    commandLineOptions _options;
    GLuint _pixelFormat;
    HDC _hDC;
    renderingContext *_mainRC;

    // this is a singleton, hide constructors etc.
    glContext();
    glContext(const glContext&);
    glContext& operator=(const glContext&);
    ~glContext();

public:
    /// Get an instance of this class
    ///
    /// @return instance
    static glContext &getInstance()
    {
        static glContext instance; 
        return instance; 
    }

    bool init(const commandLineOptions *options);

    /// Set the main rendering context
    ///
    /// @param rc [in] main rc
    void setMainRC(renderingContext *rc) { _mainRC = rc; }

    renderingContext *createRenderingContext(HDC theDC=NULL);
    void deleteRenderingContext(renderingContext *rc);

    /// Get the current rendering context
    ///
    /// @return current rc
    renderingContext getCurrent() const
    {
        renderingContext rc;

        rc._hDC = wglGetCurrentDC();
        rc._hGLRC = wglGetCurrentContext();
		rc._isDummy = true ;
        return rc;
    }

    bool makeCurrent(renderingContext *rc);
    bool makeUnCurrent();
};

#endif // GL_CONTEXT_H