
/**********************************************************************\
* AUTHOR : HILLAIRE Sébastien
*
* MAIL   : hillaire_sebastien@yahoo.fr
* SITE   : sebastien.hillaire.free.fr
*
*	You are free to totally or partially use this file/code.
* If you do, please credit me in your software or demo and leave this
* note.
*	Share your work and your ideas as much as possible!
\*********************************************************************/

#include "FrameBufferObject.h"

bool CheckFramebufferStatus(const GLuint fbo)
{
    GLenum status;
	bool ret = false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
			printf("Frame Buffer Complete");
			ret = true;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
			printf("Unsupported framebuffer format");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, missing attachment");
            break;
        //case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
        //    printf("Framebuffer incomplete, duplicate attachment");
        //   break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            printf("Framebuffer incomplete attachment");
           break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            printf("Framebuffer incomplete, attached images must have same dimensions");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            printf("Framebuffer incomplete, attached images must have same format");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            printf("Framebuffer incomplete, missing draw buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            printf("Framebuffer incomplete, missing read buffer");
            break;
        default:
            printf("Framebuffer incomplete, UNKNOW ERROR");
           // assert(0);
    }

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return ret;
}

//////////////////////////////////////////////////////////////////////////////////

FrameBufferObject::FrameBufferObject(unsigned int width, unsigned int height,
		unsigned int nbColorBuffer, 
		const unsigned int* colorBufferInternalFormat, 
		const unsigned int* colorBufferSWRAP,
		const unsigned int* colorBufferTWRAP,
		const unsigned int* colorBufferMinFiltering,
		const unsigned int* colorBufferMagFiltering,
		FBO_DepthBufferType depthBufferType,
		const unsigned int depthBufferMinFiltering,
		const unsigned int depthBufferMagFiltering,
		const unsigned int depthBufferSWRAP,
		const unsigned int depthBufferTWRAP,
		bool depthTextureCompareToR,
		FrameBufferObject* fboContainingDepthBuffer)
{
	this->width = width;
	this->height= height;
	this->nbColorAttachement = nbColorBuffer;
	if(this->nbColorAttachement>this->getMaxColorAttachments())
		this->nbColorAttachement = this->getMaxColorAttachments();

	GLint proxyTexTestWidth;
	
	/////////////////INITIALIZATION/////////////////

	//color render buffer
	if(this->nbColorAttachement>0)
	{
		this->colorTextures = new GLuint[this->nbColorAttachement];
		this->colorMinificationFiltering = new GLuint[this->nbColorAttachement];
		this->validColorMipMapPyramid = new bool[this->nbColorAttachement];
		for(int i=0; i<this->nbColorAttachement; i++)
		{
			this->colorTextures[i]=0;
			glGenTextures(1, &this->colorTextures[i]);
			glBindTexture(GL_TEXTURE_2D, this->colorTextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, colorBufferMinFiltering[i]);
			this->colorMinificationFiltering[i] = colorBufferMinFiltering[i];
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, colorBufferMagFiltering[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, colorBufferSWRAP[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, colorBufferTWRAP[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, colorBufferInternalFormat[i], width, height, 0, GL_RGB, GL_FLOAT, NULL); 

			this->validColorMipMapPyramid[i]=false;
			if(this->colorMinificationFiltering[i]==GL_NEAREST_MIPMAP_NEAREST
			|| this->colorMinificationFiltering[i]==GL_LINEAR_MIPMAP_NEAREST
			||this->colorMinificationFiltering[i]==GL_NEAREST_MIPMAP_LINEAR
			|| this->colorMinificationFiltering[i]==GL_LINEAR_MIPMAP_LINEAR)
			{
				glGenerateMipmapEXT(GL_TEXTURE_2D);
			}
		}
	}

	//depth render buffer
	this->depthType = depthBufferType;
	this->validDepthMipMapPyramid=false;
	if(this->depthType!=FBO_DepthBufferType_NONE)
	{
		this->depthID=0;
		switch(this->depthType)
		{
		case FBO_DepthBufferType_TEXTURE:
			glGenTextures(1, &this->depthID);
			glBindTexture(GL_TEXTURE_2D, this->depthID);
			this->depthMinificationFiltering = depthBufferMinFiltering;
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, depthBufferMinFiltering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if(depthTextureCompareToR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_ARB, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

			if(this->depthMinificationFiltering==GL_NEAREST_MIPMAP_NEAREST
			|| this->depthMinificationFiltering==GL_LINEAR_MIPMAP_NEAREST
			||this->depthMinificationFiltering==GL_NEAREST_MIPMAP_LINEAR
			|| this->depthMinificationFiltering==GL_LINEAR_MIPMAP_LINEAR)
			{
				glGenerateMipmapEXT(GL_TEXTURE_2D);
			}
			break;
			
		case FBO_DepthBufferType_TEXTURE_DEPTH32F:
			glGenTextures(1, &this->depthID);
			glBindTexture(GL_TEXTURE_2D, this->depthID);
			if(this->depthMinificationFiltering==GL_NEAREST)	//FORCE MIPMAP DISABLED (is it supported??)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering = GL_LINEAR;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if(depthTextureCompareToR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			break;
			
		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
			glGenTextures(1, &this->depthID);
			glBindTexture(GL_TEXTURE_2D, this->depthID);
			if(this->depthMinificationFiltering==GL_NEAREST)	//FORCE MIPMAP DISABLED
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering = GL_LINEAR;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if(depthTextureCompareToR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8_EXT, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8_EXT, NULL);
			break;
			
		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
			glGenTextures(1, &this->depthID);
			glBindTexture(GL_TEXTURE_2D, this->depthID);
			if(this->depthMinificationFiltering==GL_NEAREST)	//FORCE MIPMAP DISABLED
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			else
			{
				this->depthMinificationFiltering = GL_LINEAR;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, depthBufferMagFiltering);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, depthBufferSWRAP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, depthBufferTWRAP);
			if(depthTextureCompareToR)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
			break;

		case FBO_DepthBufferType_RENDERTARGET:
		default:
			glGenRenderbuffersEXT(1, &this->depthID);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->depthID);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, width, height);
			break;

		case FBO_DepthBufferType_TEXTURE_FROM_ANOTHER_FBO:
			//nothing to create
			break;
		}
	}


	/////////////////ATTACHEMENT/////////////////
    glGenFramebuffersEXT(1, &this->fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,this->fbo);

	//color render buffer attachement
	if(nbColorBuffer>0)
	{
		for(int i=0; i<this->nbColorAttachement; i++)
		{
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+i, GL_TEXTURE_2D, this->colorTextures[i], 0 );
		}
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	}
	else
	{
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);
	}
	
	//depth render buffer attachement
	if(this->depthType!=FBO_DepthBufferType_NONE)
	{
		switch(this->depthType)
		{
		case FBO_DepthBufferType_RENDERTARGET:
		default:
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->depthID);
			break;

		case FBO_DepthBufferType_TEXTURE:
		case FBO_DepthBufferType_TEXTURE_DEPTH32F:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depthID, 0);
			break;
			
		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
		case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depthID, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depthID, 0);
			break;
			

		case FBO_DepthBufferType_TEXTURE_FROM_ANOTHER_FBO:
			switch(fboContainingDepthBuffer->depthType)
			{
			case FBO_DepthBufferType_RENDERTARGET:
				glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, fboContainingDepthBuffer->depthID);
				this->depthID=0;
				break;
			case FBO_DepthBufferType_TEXTURE:
			case FBO_DepthBufferType_TEXTURE_DEPTH32F:
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, fboContainingDepthBuffer->depthID, 0);
				this->depthID=0;
				break;
			case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH24_STENCIL8:
			case FBO_DepthBufferType_TEXTURE_PACKED_DEPTH32F_STENCIL8:
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, fboContainingDepthBuffer->depthID, 0);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, fboContainingDepthBuffer->depthID, 0);
				this->depthID=0;
				break;
			case FBO_DepthBufferType_NONE:
				break;
			}
			break;
		}
	}

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CheckFramebufferStatus(this->fbo);
}

FrameBufferObject::~FrameBufferObject()
{
	if(this->nbColorAttachement>0)
	{
		glDeleteTextures(this->nbColorAttachement,this->colorTextures);

		switch(this->depthType)
		{
		case FBO_DepthBufferType_RENDERTARGET:
			glDeleteRenderbuffersEXT(1, &this->depthID);
			break;
		case FBO_DepthBufferType_NONE:
			break;
		default:
			glDeleteTextures(1, &this->depthID);
			break;
		}

		delete [] this->colorTextures;
		delete [] this->colorMinificationFiltering;
		delete [] this->validColorMipMapPyramid;
	}
}



//////////////////////////////////////////////////////////////////////////////////



const GLint FrameBufferObject::getMaxColorAttachments()
{
  GLint maxAttach = 0;
  glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttach );
  return maxAttach;
}
const GLint FrameBufferObject::getMaxBufferSize()
{
  GLint maxSize = 0;
  glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE_EXT, &maxSize );
  return maxSize;
}



void FrameBufferObject::enableRenderToColorAndDepth(const unsigned int colorBufferNum) const
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo);

	if(this->nbColorAttachement>0)
	{
		this->validColorMipMapPyramid[colorBufferNum]=false;
		this->validDepthMipMapPyramid=false;
		unsigned int colorBuffer = GL_COLOR_ATTACHMENT0_EXT + colorBufferNum;
		if((int)colorBufferNum>this->nbColorAttachement)
			colorBuffer = GL_COLOR_ATTACHMENT0_EXT;
		glDrawBuffer(colorBuffer);
	}
	else
	{
		glDrawBuffer(GL_NONE);	//for example, in the case of rendering in a depth buffer
	}
}

void FrameBufferObject::enableRenderToColorAndDepth_MRT(const GLuint numBuffers, const GLenum* drawbuffers) const
{ 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo);
	
	//we mark invalid concerned mipmap pyramids
	for(GLuint i=0; i<numBuffers; i++)
	{
		this->validColorMipMapPyramid[(unsigned int)(drawbuffers[i]-GL_COLOR_ATTACHMENT0_EXT)]=false;
	}
	this->validDepthMipMapPyramid=false;

	//HERE, THERE SHOULD HAVE A TEST OF THE DRAW BUFFERS ID AND NUMBER
	/*for(GLuint i=0; i<numBuffers; i++)
	{
	}*/ 
    glDrawBuffers(numBuffers, drawbuffers);

}

void FrameBufferObject::disableRenderToColorDepth()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}



void FrameBufferObject::saveAndSetViewPort() const
{
	glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, this->width, this->height);
}

void FrameBufferObject::restoreViewPort() const
{
	glPopAttrib();
}



void FrameBufferObject::bindColorTexture(const unsigned int colorBufferNum) const
{
	if(this->nbColorAttachement>0 && (int)colorBufferNum<this->nbColorAttachement)
	{
		glBindTexture(GL_TEXTURE_2D, this->colorTextures[colorBufferNum]);
	}
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::bindDepthTexture() const
{
	if(this->depthType>=FBO_DepthBufferType_TEXTURE)
	{
		glBindTexture(GL_TEXTURE_2D, this->depthID);
	}
	else
		glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBufferObject::generateColorBufferMipMap(const unsigned int colorBufferNum) const
{
	if(this->nbColorAttachement>0 && (int)colorBufferNum<this->nbColorAttachement)
	{
		if(this->colorMinificationFiltering[colorBufferNum]==GL_NEAREST
		|| this->colorMinificationFiltering[colorBufferNum]==GL_LINEAR)
			return;	//don't allow to generate mipmap chain for texture that don't support it at the creation

		glBindTexture(GL_TEXTURE_2D, this->colorTextures[colorBufferNum]);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		this->validColorMipMapPyramid[colorBufferNum]=true;
	}
}

void FrameBufferObject::generateDepthBufferMipMap() const
{
	if(this->depthType>=FBO_DepthBufferType_TEXTURE)
	{
		if(this->depthMinificationFiltering==GL_NEAREST
		|| this->depthMinificationFiltering==GL_LINEAR)
			return;	//don't allow to generate mipmap chain for texture that don't support it at the creation

		glBindTexture(GL_TEXTURE_2D, this->depthID);
		glGenerateMipmapEXT(GL_TEXTURE_2D);
		this->validDepthMipMapPyramid=true;
	}
}



unsigned int FrameBufferObject::getNumberOfColorAttachement() const
{
	return this->nbColorAttachement;
}

FBO_DepthBufferType FrameBufferObject::getDepthBufferType() const
{
	return this->depthType;
}

unsigned int FrameBufferObject::getWidth() const
{
	return this->width;
}

unsigned int FrameBufferObject::getHeight() const
{
	return this->height;
}
/*
void FrameBufferObject::saveToDisk(const unsigned int colorBufferNum, const char* filepath) const
{
	if(this->nbColorAttachement>0 && (int)colorBufferNum<this->nbColorAttachement)
	{
		ImageBMP::TextureToDisk(filepath,GL_TEXTURE_2D,this->colorTextures[colorBufferNum]);
	}
}*/


bool FrameBufferObject::isColorMipMapPyramidValid(const unsigned int colorBufferNum) const
{
	if(this->nbColorAttachement>0 && (int)colorBufferNum<this->nbColorAttachement)
	{
		return this->validColorMipMapPyramid[colorBufferNum];
	}
	else
		return false;
	
}
bool FrameBufferObject::isDepthMipMapPyramidValid() const
{
	return this->validDepthMipMapPyramid;
}


bool FrameBufferObject::isFrameBufferComplete()
{
    GLenum status;
	bool ret = false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if(status == GL_FRAMEBUFFER_COMPLETE_EXT)
		ret = true;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return ret;
}

