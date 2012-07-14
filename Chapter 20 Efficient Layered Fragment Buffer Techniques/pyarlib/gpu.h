/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


//a huge opengl wrapper. nothign special. just convenience classes to make code shorter

#ifndef GPU_MEMORY_H
#define GPU_MEMORY_H

#include "vec.h"
#include "includegl.h"
#include "shader.h"

struct GPUObject
{
	GLuint object;
	GLenum type;
	GPUObject(GLuint ntype) : type(ntype) {object = 0;}
	virtual ~GPUObject() {}
	operator GLuint() const {return object;}
	virtual bool release() =0;
};

struct TextureAttribs
{
	bool mipmap;
	int anisotropy;
	bool nearest;
	bool repeat;
	TextureAttribs();
	void applyAttribs(GLuint target);
};

struct RenderTarget : GPUObject
{
	bool multisample;
	int samples;
	GLuint format;
	vec2i size;
	RenderTarget(GLuint type = (GLuint)-1);
	virtual bool resize(vec2i size) =0;
	virtual bool release() =0;
	virtual void bind() =0;
	virtual void unbind() =0;
};

struct Texture : RenderTarget, TextureAttribs
{
protected:
	Texture(GLuint type) : RenderTarget(type) {}
public:
	virtual bool resize(vec2i size);
	virtual bool release();
	virtual void bind();
	virtual void unbind();
	virtual void genMipmap();
};

struct Texture2D : Texture
{
	Texture2D(vec2i size = vec2i(0), GLuint format = GL_RGBA, int samples = 0);
	void randomize();
};

struct Texture3D : Texture
{
	int layers;
	Texture3D(vec3i size = vec3i(0), GLuint format = GL_RGBA, int samples = 0);
	virtual bool resize(vec3i size);
	virtual bool resize(vec2i size);
};


struct TextureCubeMap : Texture
{
	TextureCubeMap(vec2i size = vec2i(0), GLuint format = GL_RGBA, int samples = 0);
	virtual bool resize(vec2i size);
};

struct RenderBuffer : RenderTarget
{
	RenderBuffer(vec2i size = vec2i(0), GLuint format = GL_RGBA, int samples = 0);
	virtual bool resize(vec2i size);
	virtual bool release();
	virtual void bind();
	virtual void unbind();
};

struct GPUBuffer : GPUObject, ShaderUniform
{
	GLuint64 address;
	GLenum access;
	GLuint image; //GL_TEXTURE_BUFFER for imageBuffer
	GLenum imageFormat;
	int dataSize;
	bool writeable;
	bool bindless;

	GPUBuffer(GLenum type, GLenum access = GL_STATIC_DRAW, bool writeable = true);
	void bind();
	void unbind();
	bool resize(int bytes, bool force = true); //force can be disabled to ignore size reduction
	//NOTE: .buffer(...) will NOT reduce the memory. it will only increase if needed
	void buffer(const void* data, int bytes, int byteOffset = 0);
	void createImage(GLenum format);
	void* map(bool read = true, bool write = true);
	void* map(unsigned int offset, unsigned int size, bool read = true, bool write = true);
	bool unmap();
	int size();
	virtual bool release();
protected:
	virtual bool setUniform(Shader* program, const std::string& name) const;
};

struct VertexBuffer : GPUBuffer
{
	VertexBuffer(GLenum access = GL_STATIC_DRAW, bool writeable = true) : GPUBuffer(GL_ARRAY_BUFFER, access, writeable) {}
};

struct IndexBuffer : GPUBuffer
{
	IndexBuffer(GLenum access = GL_STATIC_DRAW, bool writeable = true) : GPUBuffer(GL_ELEMENT_ARRAY_BUFFER, access, writeable) {}
};

struct TextureBuffer : GPUBuffer
{
	TextureBuffer(GLenum access = GL_STATIC_DRAW, bool writeable = true) : GPUBuffer(GL_TEXTURE_BUFFER, access, writeable) {}
};

struct UniformBuffer : GPUBuffer
{
	UniformBuffer(GLenum access = GL_STATIC_DRAW, bool writeable = true) : GPUBuffer(GL_UNIFORM_BUFFER, access, writeable) {}
};

struct FrameBuffer : GPUObject
{
	bool hasResize;
	bool hasAttach;
	int maxAttach;
	int backupViewport[4];
	vec2i size; //for error checking and convenience only
	RenderTarget *depth;
	RenderTarget *stencil;
	RenderTarget *colour[16];

	GLuint getDepth() {return depth ? (GLuint)*depth : 0;}
	GLuint getStencil() {return stencil ? (GLuint)*stencil : 0;}
	GLuint getColour(int i) {return colour[i] ? (GLuint)*colour[i] : 0;}

	FrameBuffer(vec2i size = vec2i(0));
	bool attach(GLenum attachment, RenderTarget* target);
	bool resize(vec2i size);
	bool resize(int w, int h) {return resize(vec2i(w, h));}
	void attach();
	void bind();
	void blit(GLuint target = 0, bool blitDepth = false, vec2i offset = vec2i(0), vec2i scaleTo = vec2i(-1));
	void unbind();
	virtual bool release();
};

#endif
