#include "stdafx.h"

#include <math.h>

#include "stageConsumer.h"
#include "window.h"
#include "queue.h"
#include "timer.h"

static const GLfloat vertexData[] =
{
	-1.f, -1.f,
	0.f, 0.f,

	-1.f,  1.f,
	0.f,  1.f,

	1.f, -1.f,
	1.f,  0.f,

	1.f,  1.f,
	1.f,  1.f,
};

static const GLushort indexData[] =
{
	0,1,2, 2,1,3
};


/// Constructor
stageConsumer::stageConsumer()
{
}

/// Destructor
stageConsumer::~stageConsumer()
{
}

/// Init the render stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
stageConsumer::init(const commandLineOptions *options)
{
	// call base class
	if(!Stage::init(options))
		return false;

	//use affinity DC here
	_rc = glContext::getInstance().createRenderingContext(_affinityDC);
	if(!_rc)
		return false;

	if(!glContext::getInstance().makeCurrent(_rc))
		return false;

	//TODO check support for functions 
	// Check if all the necessary functions are supported GL_ARB_vertex_buffer_object, "
	//            "GL_EXT_framebuffer_object, GL_ARB_shader_objects

	glGenTextures(_options._numStreams, _destTexID);
	for(unsigned int stream = 0; stream < _options._numStreams; stream++)
	{
		glBindTexture(GL_TEXTURE_2D, _destTexID[stream]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _options._texWidth,_options._texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(1, &_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(GLfloat), (const GLvoid*)(2 * sizeof(GLfloat)));

	glGenBuffers(1, &_iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), &indexData, GL_STATIC_DRAW);

	//TODO what is this for?
	//    glGenQueries(1, &_drawTimeQuery);
	//  QueryPerformanceFrequency(&_frequency);

	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("stageConsumer::init - Detected GL error.");
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
stageConsumer::cleanup()
{
	if(_destTexID)
	{
		glDeleteTextures(_options._numStreams, _destTexID);
	}

	if(_vboID)
	{
		glDeleteBuffers(1, &_vboID);
		_vboID = 0;
	}
	if(_iboID)
	{
		glDeleteBuffers(1, &_iboID);
		_iboID = 0;
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
stageConsumer::preExecution()
{
	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("wglMakeCurrent");
		return false;
	}

	// start the producer stage
	queue::element *element;

	//queue my textures so that the producer can start copying into this
	for(unsigned int stream = 0; stream < _options._numStreams; stream++)
	{
		element = new queue::element();
		element->_id = _destTexID[stream];
		_outQueue[0]->enqueue(element);
	}

	return true;
}

/// Called after execution finished
///
/// @return false if failed
bool
stageConsumer::postExecution()
{
	glFinish();

	if(!glContext::getInstance().makeUnCurrent())
		return false;

	return true;
}

/// Called per frame before iterating over all streams
///
/// @return false if failed
bool
stageConsumer::startFrame() {
	glClearColor(0.5,0.5,0.5,1.0);
	glViewport(0,0, window::getInstance().width(), window::getInstance().height());
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	return true;
}

/// Called per frame after iterating over all streams
///
/// @return false if failed
bool 
stageConsumer::endFrame() {
	glViewport(0,0, window::getInstance().width(), window::getInstance().height());
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);	
    if(!wglSwapLayerBuffers(_rc->hDC(), WGL_SWAP_MAIN_PLANE))
    {
        window::getInstance().messageBoxWithLastError("wglSwapLayerBuffers");
        return false;
    }
	return true;
}

/// The stage work function, happens in a loop till the app signals exit
///
/// @param stream [in] current stream index
///
/// @return false if failed
bool
stageConsumer::execute(unsigned int stream)
{
	queue::element *producedTex;
	// wait for the texture to be produced ie nvcopyimage is done
	producedTex = _inQueue[0]->dequeue();

	// draw from texture
	const unsigned int column = stream % ((window::getInstance().width() + 255) / 256);
	const unsigned int row = stream / ((window::getInstance().width() + 255) / 256);

	glViewport(column * 256, row * 256, 256, 256);

	glBindTexture(GL_TEXTURE_2D, producedTex->_id);
	glDrawElements(GL_TRIANGLES, sizeof(indexData)/sizeof(indexData[0]), GL_UNSIGNED_SHORT, 0);

#ifdef _DEBUG
	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("Detected GL error.");
	}
	else {
		printf("No error\n");
	}

#endif	
	// signal the producer thread that we are done using that texture so it can start pproducing
	_outQueue[0]->enqueue(producedTex);
	return true;
}
