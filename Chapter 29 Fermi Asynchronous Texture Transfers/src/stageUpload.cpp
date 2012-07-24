#include "stdafx.h"

#include "StageUpload.h"
#include "window.h"
#include "queue.h"
static const unsigned int colorTable[] =
{
	0xFF0000FF,
	0xFF00FF00,
	0xFFFF0000,
	0xFF00FFFF
};

/// Constructor
StageUpload::StageUpload()
{
	unsigned int i;

	_unpackPBOID[0] = 0;
	_unpackPBOID[1] = 0;

	for(i = 0; i < limits::MAX_STREAMS; i++)
		_imageBuffer[i] = NULL;
}

/// Destructor
StageUpload::~StageUpload()
{
}

/// Init the upload stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
StageUpload::init(const commandLineOptions *options)
{
	unsigned int i, stream;
	// call base class
	if(!Stage::init(options))
		return false;

	_rc = glContext::getInstance().createRenderingContext();
	if(!_rc)
		return false;

	if(!glContext::getInstance().makeCurrent(_rc))
		return false;

	// TODO Check if all the necessary functions are supported - GL_ARB_map_buffer_range

	unsigned int size = _options._inTexWidth * _options._inTexHeight * 4 * sizeof(GLubyte);

	glGenBuffers(2, _unpackPBOID);
	for(stream = 0; stream < 2; stream++)
	{
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _unpackPBOID[stream]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, size, 0, GL_STREAM_DRAW_ARB);
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("StageUpload::init - Detected GL error.");
		return false;
	}

	if(!glContext::getInstance().makeUnCurrent())
		return false;

	for(stream = 0; stream < _options._numStreams; stream++)
	{
		_imageBuffer[stream] = malloc(size);
		if(!_imageBuffer[stream])
		{
			window::getInstance().messageBoxError("StageUpload::init - Out of memory.");
			return false;
		}
		const unsigned int color = colorTable[stream % (sizeof(colorTable) / sizeof(colorTable[0]))];
		for(i = 0; i < _options._inTexWidth * _options._inTexHeight; i++)
			((unsigned int*)_imageBuffer[stream])[i] = color;
	}

	return true;
}

/// Cleanup
///
/// @return false if failed
bool
StageUpload::cleanup()
{
	unsigned int i;

	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("StageUpload::cleanup - wglMakeCurrent");
		return false;
	}

	if(_unpackPBOID[0] != 0)
	{
		glDeleteBuffers(2, _unpackPBOID);
		_unpackPBOID[0] = 0;
		_unpackPBOID[1] = 0;
	}

	for(i = 0; i < limits::MAX_STREAMS; i++)
	{
		if(_imageBuffer[i])
		{
			free(_imageBuffer[i]);
			_imageBuffer[i] = NULL;
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
StageUpload::preExecution()
{
	_pboIndex = 0;

	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("StageUpload::preExecution - wglMakeCurrent");
		return false;
	}

	return true;
}

/// Called after execution finished
///
/// @return false if failed
bool
StageUpload::postExecution()
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
StageUpload::execute(unsigned int stream)
{
	queue::element *element;
	unsigned int size = _options._inTexWidth * _options._inTexHeight * 4 * sizeof(GLubyte);

	// 1. APP-> PBO transfer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, _unpackPBOID[_pboIndex]);

	unsigned int *ptr;
	ptr = (unsigned int*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER_ARB, 0, size, GL_MAP_WRITE_BIT);
	if(!ptr)
	{
		window::getInstance().messageBoxError("StageUpload::execute - glMapBufferRange failed.");
		return false;
	}
	//if we are doing a memcpy, ideally this should be in a separate thread!
	//memcpy(ptr, _imageBuffer[stream], size);
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

	// 2. PBO -> Texture transfer, here is where we need access to shared texture
	// wait for the render or readback thread to finish using on this texture
	element = _inQueue[0]->dequeue();

	//TODOCALIBRATION begin here
	// got the texture, upload to the texture using the PBO
	glBindTexture(GL_TEXTURE_2D, element->_id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _options._inTexWidth,_options._inTexHeight, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Unbind the texture so that it's safe to us it from the next stage. Concurrently using the texture on two stages will cause bad things.
	glBindTexture(GL_TEXTURE_2D, 0);

	// signal the render thread that we are done using that texture
	_outQueue[0]->enqueue(element);

	// use the next PBO
	_pboIndex ^= 1;

	return true;
}
