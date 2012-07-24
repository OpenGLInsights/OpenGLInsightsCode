#include "stdafx.h"

#include <math.h>

#include "stageProducer.h"
#include "window.h"
#include "queue.h"
#include "timer.h"

/// Constructor
stageProducer::stageProducer()
{
	_frame = 0;
}

/// Destructor
stageProducer::~stageProducer()
{
}

/// Init the render stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
stageProducer::init(const commandLineOptions *options)
{
	// call base class
	if(!Stage::init(options))
		return false;
	_rc = glContext::getInstance().createRenderingContext(_affinityDC);
	if(!_rc)
		return false;

	if(!glContext::getInstance().makeCurrent(_rc))
		return false;

	//TODO check support for functions 
	// Check if all the necessary functions are supported GL_ARB_vertex_buffer_object, "
	//            "GL_EXT_framebuffer_object, GL_ARB_shader_objects
	glGenFramebuffers(1, &_fboID);	
	glGenTextures(_options._numStreams, _srcTexID);
	for(unsigned int stream = 0; stream < _options._numStreams; stream++)
	{
		glBindTexture(GL_TEXTURE_2D, _srcTexID[stream]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _options._texWidth, _options._texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	//TODO init the teapot
	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("stageProducer::init - Detected GL error.");
		return false;
	}

	if(!glContext::getInstance().makeUnCurrent())
		return false;

	return true;
}

/// Cleanup
///
/// @return false if failed
bool
stageProducer::cleanup()
{
	if(_fboID)
	{
		glDeleteFramebuffers(1, &_fboID);
		_fboID = 0;
	}

	if(_rc != 0)
	{
		glContext::getInstance().deleteRenderingContext(_rc);
		_rc = 0;
	}

	return true;
}

/// Called before the execution starts
///
/// @return false if failed
bool
stageProducer::preExecution()
{
	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("wglMakeCurrent");
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboID);
	return true;
}

/// Called after execution finished
///
/// @return false if failed
bool
stageProducer::postExecution()
{
	glFinish();

	if(!glContext::getInstance().makeUnCurrent())
		return false;

	return true;
}

/// The stage work function
///
/// @param stream [in] current stream index
///
/// @return false if failed
bool
stageProducer::execute(unsigned int stream)
{
	queue::element *producedTex;
	// wait for the texture to be consumed so that we can copy INTO it
	producedTex = _inQueue[0]->dequeue();

	// draw something to the texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _srcTexID[stream], 0);
#ifdef _DEBUG
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		window::getInstance().messageBoxWithLastError("stageProducer::execute -Detected invalid framebuffer");
		return false;
	}
#endif

	glViewport(0,0, _options._texWidth,_options._texHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(fmod(_frame / 256.f, 1.f), fmod(_frame / 13.f, 1.f), fmod(_frame / 118.f, 1.f));
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(_options._texWidth,0);
	glVertex2f(_options._texWidth,_options._texHeight);
	glVertex2f(0,_options._texHeight);	
	glEnd();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	// copy to consumer
	GLboolean success = wglCopyImageSubDataNV(_rc->hGLRC(), _srcTexID[stream], GL_TEXTURE_2D, 0,0,0,0,
		_destrc->hGLRC(), producedTex->_id, GL_TEXTURE_2D, 0,0,0,0,
		_options._texWidth, _options._texHeight, 1);
	if(success == GL_FALSE)
	{
		window::getInstance().messageBoxError("wglCopyImageSubDataNV failed");
		return false;
	}

#ifdef _DEBUG
	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("Detected GL Error");
		return false;
	}
#endif

	// signal the upload and readback thread that we are done using that texture
	_outQueue[0]->enqueue(producedTex);
	_frame++;
	return true;
}
