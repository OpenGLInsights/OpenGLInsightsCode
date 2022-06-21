#include "stdafx.h"

#include "StageReadback.h"
#include "window.h"
#include "queue.h"

/// Constructor
StageReadback::StageReadback()
{
	_fboID = 0;
	_packPBOID[0] = 0;
	_packPBOID[1] = 0;

	for(unsigned int i = 0; i < limits::MAX_STREAMS; i++)
		_resultBuffer[i] = NULL;
}

/// Destructor
StageReadback::~StageReadback()
{
}

/// Init the readback stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
StageReadback::init(const commandLineOptions *options)
{
	// call base class
	if(!Stage::init(options))
		return false;

	_rc = glContext::getInstance().createRenderingContext();
	if(!_rc)
		return false;

	if(!glContext::getInstance().makeCurrent(_rc))
		return false;

	// TODO Check if all the necessary functions are supported - GL_ARB_map_buffer_range & GL_EXT_framebuffer_object

	unsigned int size = _options._outTexWidth * _options._outTexHeight * 4 * sizeof(GLubyte);

	glGenBuffers(2, _packPBOID);
	for(unsigned int stream = 0; stream < 2; stream++)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, _packPBOID[stream]);
		glBufferData(GL_PIXEL_PACK_BUFFER_ARB, size, 0, GL_STREAM_DRAW_ARB);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
	//generate fbo
	glGenFramebuffers(1, &_fboID);

	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("StageReadback::init - Detected GL error.");
		return false;
	}

	if(!glContext::getInstance().makeUnCurrent())
		return false;

	for(unsigned int stream = 0; stream < _options._numStreams; stream++)
	{
		_resultBuffer[stream] = malloc(size);
		if(!_resultBuffer[stream])
		{
			window::getInstance().messageBoxError("StageReadback::init - Out of memory.");
			return false;
		}
	}

	return true;
}

/// Cleanup
///
/// @return false if failed
bool
StageReadback::cleanup()
{
	unsigned int i;

	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("StageReadback::cleanup - wglMakeCurrent");
		return false;
	}

	if (_fboID) {
		glDeleteFramebuffers(1, &_fboID);
		_fboID = 0;
	}
	if(_packPBOID[0] != 0)
	{
		glDeleteBuffers(2, _packPBOID);
		_packPBOID[0] = 0;
		_packPBOID[1] = 0;
	}

	for(i = 0; i < limits::MAX_STREAMS; i++)
	{
		if(_resultBuffer[i])
		{
			free(_resultBuffer[i]);
			_resultBuffer[i] = NULL;
		}
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
StageReadback::preExecution()
{
    // at the start there is no valid previous result, so set this to 'invalid'
    _prevResult = INVALID_RESULT;
	_pboIndex = 0;

	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("StageUpload::preExecution - wglMakeCurrent");
		return false;
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fboID);
	return true;
}

/// Called after execution finished
///
/// @return false if failed
bool
StageReadback::postExecution()
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
StageReadback::execute(unsigned int stream)
{
	queue::element *element;
	unsigned int size = _options._outTexWidth * _options._outTexHeight * 4 * sizeof(GLubyte);

	// 1. Texture -> PBO transfer, here is where we need access to shared texture
	// wait for the render thread to finish rendering to this texture
	element = _inQueue[0]->dequeue();
	{
		// got the texture, readback from texture to pbo
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, _packPBOID[_pboIndex]);
		//TODOCALIBRATION begin here
		glBindTexture(GL_TEXTURE_2D, element->_id);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		// Unbind the texture so that it's safe to us it from the next stage. Concurrently using the texture on two stages will cause bad things.
		glBindTexture(GL_TEXTURE_2D, 0);
		//TODOCALIBRATION end here
		// signal to render thread that we are done reading back from that texture and its free to render to it
	}
	_outQueue[0]->enqueue(element);

	// use the next PBO
	if(_prevResult != INVALID_RESULT)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, _packPBOID[_prevResult]);

		// 1. PBO-> APP transfer
		const void * ptr = (const void*)glMapBufferRange(GL_PIXEL_PACK_BUFFER_ARB, 0, size, GL_MAP_READ_BIT);
		if(!ptr)
		{
			window::getInstance().messageBoxError("StageReadback::execute - glMapBufferRange failed.");
			return false;
		}
		//if we are doing a memcpy, ideally this should be in a separate thread!
		//memcpy(_resultBuffer[stream], ptr, size);
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER_ARB);
	}

	// we now have a valid previous result
	_prevResult = _pboIndex;
	// use the next PBO
	_pboIndex ^= 1;

	return true;
}
