#include "stdafx.h"

#include <math.h>

#include "StageRender.h"
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

static const GLcharARB mandelbrotShader[] =
{
	"uniform float maxIterations;\n"
	"uniform sampler2D tex;\n"
	"\n"
	"void main ()\n"
	"{\n"
	"    vec2 c = gl_TexCoord[0].st;\n"
	"    vec2 z = c;\n"
	"    vec4 color = texture2D(tex, gl_TexCoord[0].st);\n"
	"\n"
	"    for (float i = 0; i < maxIterations; i += 1.0)\n"
	"    {\n"
	"        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;\n"
	"        if (dot(z, z) > 4.0)\n"
	"        {\n"
	"            color = i / maxIterations;\n"
	"            break;\n"
	"        }\n"
	"    }\n"
	"    gl_FragColor = color;\n"
	"}\n"
};

/// Constructor
StageRender::StageRender()
{
}

/// Destructor
StageRender::~StageRender()
{
}

/// Init the render stage
///
/// @param options [in] command line options
///
/// @return false if failed
bool
StageRender::init(const commandLineOptions *options)
{
	GLsizei shaderLength;
	const GLcharARB *shaderString;
	GLint status;

	// call base class
	if(!Stage::init(options))
		return false;

	_rc = glContext::getInstance().createRenderingContext();
	if(!_rc)
		return false;

	if(!glContext::getInstance().makeCurrent(_rc))
		return false;

	//TODO check support for functions 
	// Check if all the necessary functions are supported GL_ARB_vertex_buffer_object, "
	//            "GL_EXT_framebuffer_object, GL_ARB_shader_objects
	glGenFramebuffers(1, &_fboID);
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

	_fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);

	shaderLength = sizeof(mandelbrotShader);
	shaderString = mandelbrotShader;
	glShaderSourceARB(_fragmentShader, 1, &shaderString, &shaderLength);
	glCompileShaderARB(_fragmentShader);
	glGetShaderiv(_fragmentShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		GLcharARB infoLog[512];
		GLsizei returnedLength;

		glGetInfoLogARB(_fragmentShader, sizeof(infoLog), &returnedLength, infoLog);
		window::getInstance().messageBoxError("Vertex shader compilation failed.");
		return false;
	}
	_program = glCreateProgramObjectARB();
	glAttachObjectARB(_program, _fragmentShader);
	glLinkProgramARB(_program);
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (!status)
	{
		GLcharARB infoLog[512];
		GLsizei returnedLength;

		glGetInfoLogARB(_program, sizeof(infoLog), &returnedLength, infoLog);
		window::getInstance().messageBoxError("Program linking failed.");
		return false;
	}

	glUseProgramObjectARB(_program);
	_maxIterationsLocation = glGetUniformLocationARB(_program, "maxIterations");
	_maxIterations = _options._maxIterations;
	glUniform1fARB(_maxIterationsLocation, (float)_maxIterations);
	glUseProgramObjectARB(0);

	if(glGetError() != GL_NO_ERROR)
	{
		window::getInstance().messageBoxError("StageRender::init - Detected GL error.");
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
StageRender::cleanup()
{
	if(_rc != 0)
	{
		glContext::getInstance().deleteRenderingContext(_rc);
		_rc = 0;
	}

	if(_fboID)
	{
		glDeleteFramebuffers(1, &_fboID);
		_fboID = 0;
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

	return true;
}

/// Called before the execution starts
///
/// @return false if failed
bool
StageRender::preExecution()
{
	if(!glContext::getInstance().makeCurrent(_rc))
	{
		window::getInstance().messageBoxWithLastError("wglMakeCurrent");
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fboID);
	glEnable(GL_TEXTURE_2D);
	glUseProgramObjectARB(_program);

	return true;
}

/// Called after execution finished
///
/// @return false if failed
bool
StageRender::postExecution()
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
StageRender::execute(unsigned int stream)
{
	queue::element *uploadTex, *readbackTex;
	// wait for the texture to be uploaded
	uploadTex = _inQueue[0]->dequeue();
	//wait for a texture to be readback
	readbackTex = _inQueue[1]->dequeue();

	//TODOCALIBRATION begin here
	// draw something to the output texture
	glBindTexture(GL_TEXTURE_2D, uploadTex->_id);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, readbackTex->_id, 0);

#ifdef _DEBUG
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		window::getInstance().messageBoxError("Detected invalid framebuffer.");
		return false;
	}
#endif

	glViewport(0,0, _options._outTexWidth, _options._outTexHeight);
	glDrawElements(GL_TRIANGLES, sizeof(indexData)/sizeof(indexData[0]), GL_UNSIGNED_SHORT, 0);

	// Unbind the render buffer so that it's safe to us it from the next stage. Concurrently using the render buffer on two stages will cause bad things.
	//TODO gives error for older driver
//    if(_bug906242Fixed)
	//glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
#ifdef _DEBUG
			if(glGetError() != GL_NO_ERROR)
			{
				window::getInstance().messageBoxError("Detected GL error.");
			}
			else {
				printf("No error\n");
			}

#endif	
	glBindTexture(GL_TEXTURE_2D, 0);

	//TODOCALIBRATION end here
	// signal the upload and readback thread that we are done using that texture
	_outQueue[0]->enqueue(uploadTex);
	_outQueue[1]->enqueue(readbackTex);

	return true;
}
