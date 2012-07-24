#include "stdafx.h"
#include "glContext.h"
#include "window.h"

//TODO update to OGL3.2 and add ARB_DEBUG_OUTPUT support
//ARB_debug_output functions are not available and ignored in opengl when no debug context.

static const PIXELFORMATDESCRIPTOR pfd =    // pfd Tells Windows How We Want Things To Be
{
	sizeof (PIXELFORMATDESCRIPTOR),         // Size Of This Pixel Format Descriptor
	1,                                      // Version Number
	PFD_DRAW_TO_WINDOW |                    // Format Must Support Window
	PFD_SUPPORT_OPENGL |                    // Format Must Support OpenGL
	PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
	PFD_TYPE_RGBA,                          // Request An RGBA Format
	24,                                     // Select Our Color Depth
	0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
	1,                                      // Alpha Buffer
	0,                                      // Shift Bit Ignored
	0,                                      // No Accumulation Buffer
	0, 0, 0, 0,                             // Accumulation Bits Ignored
	24,                                     // 24 Bit Z-Buffer (Depth Buffer)  
	8,                                      // 8 Bit Stencil Buffer
	0,                                      // No Auxiliary Buffer
	PFD_MAIN_PLANE,                         // Main Drawing Layer
	0,                                      // Reserved
	0, 0, 0                                 // Layer Masks Ignored
};


/// Constructor
glContext::glContext()
{
	_hDC = 0;
	_mainRC = 0;
}

/// Destructor
glContext::~glContext()
{
	if(_hDC)
		ReleaseDC(window::getInstance().hWnd(), _hDC);
}

/// Initialize
///
/// @param options [in] command line options
///
/// @return false if failed
bool
glContext::init(const commandLineOptions *options)
{
  //  HGLRC dummyRC;

	_options = *options;
	_hDC = GetDC(window::getInstance().hWnd());

	_pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	if(_pixelFormat == 0)
	{
		window::getInstance().messageBoxWithLastError("ChoosePixelFormat");
		return false;
	}

	if(SetPixelFormat(_hDC, _pixelFormat, &pfd) == FALSE)
	{
		window::getInstance().messageBoxWithLastError("SetPixelFormat");
		return false;
	}

    // create a dummy context needed for getting extension
    //dummyRC = wglCreateContext(_hDC);
    //if(dummyRC == 0)
    //{
    //    window::getInstance().messageBoxWithLastError("wglCreateContext");
    //    return false;
    //}

    //// init context
    //if(!wglMakeCurrent(_hDC, dummyRC))
    //{
    //    window::getInstance().messageBoxWithLastError("wglMakeCurrent");
    //    return false;
    //}

    //// TODO Check if all the necessary functions are supported
    //
    //    window::getInstance().messageBoxError("A required extension is not supported (WGL_ARB_create_context, GL_ARB_sync, GL_ARB_occlusion_query)"); 
    //    return false;
    //}

    //// delete the dummy context
    //if(!wglMakeCurrent(_hDC, NULL))
    //{
    //    window::getInstance().messageBoxWithLastError("wglMakeCurrent");
    //    return false;
    //}

    //if(!wglDeleteContext(dummyRC))
    //{
    //    window::getInstance().messageBoxWithLastError("wglDeleteContext");
    //    return false;
    //}

	return true;
}

/// Constructor
glContext::renderingContext::renderingContext()
{
	_hDC = 0;
	_hGLRC = 0;
	_isDummy = false;
}

/// Destructor
glContext::renderingContext::~renderingContext()
{
	if (_isDummy) {
	if(_hGLRC)
		wglDeleteContext(_hGLRC);
	if(_hDC)
		ReleaseDC(window::getInstance().hWnd(), _hDC);
	}
}

/// Create a GL rendering context
///
/// @return NULL if failed
glContext::renderingContext *
glContext::createRenderingContext()
{
	glContext::renderingContext *rc;

	rc = new renderingContext();

	rc->_hDC = GetDC(window::getInstance().hWnd());
	rc->_hGLRC = wglCreateContext(rc->_hDC);
//   HGLRC mainhGLRC = _mainRC ? _mainRC->_hGLRC : 0; //todo for ARB_CREATE_CONTEXT
    //rc->_hGLRC = wglCreateContextAttribsARB(rc->_hDC, mainhGLRC, contextAttribs); //TODO for ARB_CREAE_CONTEXT, contextAttribs = NULL if no debug required

	if (_mainRC) //main context already created, share withi these contexts
		wglShareLists(_mainRC->_hGLRC,rc->_hGLRC);
	if(rc->_hGLRC == 0)
	{
		window::getInstance().messageBoxWithLastError("wglCreateContext");
		return false;
	}

	//TODO debug - makecurrent(rc) to set the debug callbak here
   // makeCurrent(rc);

    //GLExtensionFunctions::getInstance().resolve();
   // if(_options._verbose && GLExtensionFunctions::getInstance()._has_GL_ARB_debug_output) {
     //   glDebugMessageCallbackARB(debugMessageCallback, NULL);
    //}
//    makeUnCurrent();
	return rc;
}

/// Delete a GL rendering context
void
glContext::deleteRenderingContext(glContext::renderingContext *rc)
{
	delete rc;
}

/// Make a rendering context current
///
/// @param rc [in] rendering context
///
/// @return false if failed
bool glContext::makeCurrent(renderingContext *rc)
{
	if(!wglMakeCurrent(rc->_hDC, rc->_hGLRC))
	{
		window::getInstance().messageBoxWithLastError("wglMakeCurrent");
		return false;
	}

	return true;
}

/// Make the current rendering context un-current
///
/// @return false if failed
bool glContext::makeUnCurrent()
{
	if(!wglMakeCurrent(NULL, NULL))
	{
		window::getInstance().messageBoxWithLastError("wglMakeCurrent");
		return false;
	}

	return true;
}
