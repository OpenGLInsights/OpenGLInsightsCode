/*
 * dragon.cpp
 *
 *  Created on: Sep 24, 2011
 *      Author: aqnuep
 */

#include <iostream>
#include <fstream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtx/inverse_transpose.hpp>
#include "wavefront.h"
#include "tga.h"
#include "dragon.h"

#define CHECK_GL_ERROR()												\
{																		\
	GLenum glError;														\
	if ((glError = glGetError()) != GL_NO_ERROR) {						\
		cerr << "OpenGL error code in '" << __FILE__ << "' at line " << __LINE__ << ": " << gluErrorString(glError) << endl;	\
	}																	\
}

#define CHECK_FRAMEBUFFER_STATUS()                            \
{                                                             \
    GLenum status;                                            \
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);		  \
    switch(status) {                                          \
    case GL_FRAMEBUFFER_COMPLETE:                         	  \
        break;                                                \
    case GL_FRAMEBUFFER_UNSUPPORTED:                    	  \
        /* choose different formats */                        \
        cerr << "Unsupported framebuffer format!" << endl;	  \
        break;                                                \
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:				  \
    	cerr << "Incomplete framebuffer attachment!" << endl; \
    	break;												  \
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:		  \
    	cerr << "Missing framebuffer attachment!" << endl; 	  \
    	break;												  \
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:			  \
    	cerr << "Incomplete layer target!" << endl;			  \
    	break;												  \
    default:                                                  \
        /* programming error; will fail on all hardware */    \
        cerr << "Invalid framebuffer format!" << endl;		  \
        exit(0);                                          	  \
    }														  \
}

using namespace glm;
using namespace demo;

namespace dragon {

DragonDemo::DragonDemo() {

	cerr << "> Initializing scene data..." << endl;

	// initialize camera data
	float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
	float fieldOfView = 45.f;
	camera.position = vec3(-7.5f, 5.5f, 7.5f);
	camera.rotation = vec3(40.0f, 45.0f, 0.0f);
	transform.zNear = 0.1f;
	transform.zFar = 40.f;
	transform.nearTop = transform.zNear * tan(0.5f * fieldOfView);
	transform.nearRight = transform.nearTop * aspectRatio;
	transform.Viewport = vec4( 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT );
	transform.ProjectionMatrix = perspective(fieldOfView, aspectRatio, transform.zNear, transform.zFar);

	// create uniform buffer and store camera data
	glGenBuffers(1, &transformUB);
	glBindBuffer(GL_UNIFORM_BUFFER, transformUB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), &transform, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformUB);

	loadShaders();
	loadModels();
	setupGBuffer();
	createLights();

	glEnable(GL_FRAMEBUFFER_SRGB);

	cerr << "> Configuring rendering environment..." << endl;

	glEnable(GL_CULL_FACE);

	cerr << "> Done!" << endl;

	CHECK_GL_ERROR();
}

DragonDemo::~DragonDemo() {

	cerr << "> Freeing graphics resources... ";

	// delete shader programs
	glDeleteProgram(GBufferVSP);
	glDeleteProgram(GBufferFSP);
	glDeleteProgram(shadowMultiVSP);
	glDeleteProgram(shadowMultiGSP);
	glDeleteProgram(shadowSingleVSP);
	glDeleteProgram(lightVSP);
	glDeleteProgram(lightGSP);
	glDeleteProgram(lightFSP);

	// delete program pipelines
	glDeleteProgramPipelines(1, &GBufferPPL);
	glDeleteProgramPipelines(1, &shadowMultiPPL);
	glDeleteProgramPipelines(1, &shadowSinglePPL);
	glDeleteProgramPipelines(1, &lightPPL);

	// delete uniform buffers
	glDeleteBuffers(1, &transformUB);
	glDeleteBuffers(1, &lightTransformUB);
	glDeleteBuffers(1, &lightUB);

	// delete vertex buffer
	glDeleteBuffers(1, &vertexBuffer);

	// delete index buffer
	glDeleteBuffers(1, &indexBuffer);

	// delete vertex arrays
	glDeleteVertexArrays(1, &vertexArray);

	// delete G-buffer textures
	glDeleteTextures(1, &positionGBT);
	glDeleteTextures(1, &normalGBT);
	glDeleteTextures(1, &depthGBT);

	// delete shadow map
	glDeleteTextures(1, &shadowArrayTex);

	// delete framebuffers
	glDeleteFramebuffers(1, &shadowFB);

	cerr << "Done!" << endl;

}

void DragonDemo::loadModels() {

	cerr << "> Loading models..." << endl;

	WaveFrontObj dragonObj("models/dragon.obj");

	cerr << "> Uploading mesh data to GPU..." << endl;

	// create empty index buffer
	int indexBufferSize = dragonObj.index.size() * sizeof(uint);
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

	// map index buffer and fill with data
	uint *index = (uint*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indexBufferSize,
						GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (uint i=0; i<dragonObj.index.size(); ++i)
		index[i] = dragonObj.index[i];

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// create empty vertex buffer
	int vertexBufferSize = dragonObj.vertex.size() * sizeof(Vertex);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

	// map vertex buffer and fill with data
	Vertex *vertex = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vertexBufferSize,
						GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (uint i=0; i<dragonObj.vertex.size(); ++i) {
		vertex[i].position[0] = dragonObj.vertex[i].x;
		vertex[i].position[1] = dragonObj.vertex[i].y;
		vertex[i].position[2] = dragonObj.vertex[i].z;
		vertex[i].normal[0] = dragonObj.normal[i].x;
		vertex[i].normal[1] = dragonObj.normal[i].y;
		vertex[i].normal[2] = dragonObj.normal[i].z;
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// setup vertex array
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glBindVertexArray(0);

	// setup draw command
	drawCommand.prim_type = GL_TRIANGLES;
	drawCommand.indexOffset = 0;
	drawCommand.indexCount = dragonObj.index.size();

	CHECK_GL_ERROR();

}

void DragonDemo::updateLights() {

	// update light positions and view-projection matrices
	mat4 ProjMat = perspective(90.f, 1.f, 0.01f, 40.f);
	for (int i=0; i<NUMBER_OF_LIGHTS; ++i) {
		float angle = (float)i/NUMBER_OF_LIGHTS + lightRotationFactor;
		lights[i].position = vec4(sin(angle * M_PI * 2.f) * 4.f, -2.5f,
								  cos(angle * M_PI * 2.f) * 4.f, 1.f);
		lightTransform[i].VPMatrix = ProjMat * translate(rotate(rotate(mat4(1.f), +30.f, vec3(1.f, 0.f, 0.f)), angle * -360.f, vec3(0.f, 1.f, 0.f)), -vec3(lights[i].position));
	}

	// update light uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, lightUB);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, NUMBER_OF_LIGHTS * sizeof(Light), lights);

	// update shadow rendering VP matrix uniform buffer
	glBindBuffer(GL_UNIFORM_BUFFER, lightTransformUB);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, NUMBER_OF_LIGHTS * sizeof(LightTransform), lightTransform);

}

void DragonDemo::createLights() {

	cerr << "> Creating lights..." << endl;

	// generate lights
	for (int i=0; i<NUMBER_OF_LIGHTS; ++i) {
		lights[i].color = vec4((rand() * 0.2f) / RAND_MAX + 0.1f,
							   (rand() * 0.2f) / RAND_MAX + 0.1f,
							   (rand() * 0.2f) / RAND_MAX + 0.1f, 1.f);
	}

	// create light uniform buffer
	glGenBuffers(1, &lightUB);
	glBindBuffer(GL_UNIFORM_BUFFER, lightUB);
	glBufferData(GL_UNIFORM_BUFFER, NUMBER_OF_LIGHTS * sizeof(Light), lights, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightUB);

	// create shadow rendering VP matrix uniform buffer
	glGenBuffers(1, &lightTransformUB);
	glBindBuffer(GL_UNIFORM_BUFFER, lightTransformUB);
	glBufferData(GL_UNIFORM_BUFFER, NUMBER_OF_LIGHTS * sizeof(LightTransform), lightTransform, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, lightTransformUB);

	// update light information
	lightRotationFactor = 0.f;
	updateLights();

	// create shadow map texture cube array
	glGenTextures(1, &shadowArrayTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowArrayTex);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16,
				SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, NUMBER_OF_LIGHTS,
				0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// create shadow rendering framebuffer
	glGenFramebuffers(1, &shadowFB);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFB);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArrayTex, 0);
	CHECK_FRAMEBUFFER_STATUS();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERROR();

}

GLuint DragonDemo::loadShaderProgramFromFile(const char* filename, GLenum shaderType) {

	ifstream file(filename);
	if (!file) {
		cerr << "Unable to open file: " << filename << endl;
		return 0;
	}

	char line[256];
	string source;

	while (file) {
		file.getline(line, 256);
		source += line;
		source += '\n';
	}

	if (!file.eof()) {
		cerr << "Error reading the file: " << filename << endl;
		return 0;
	}

	GLuint program = glCreateShaderProgramv(shaderType, 1, (const GLchar**)&source);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		cerr << "Failed to compile/link shader program:" << endl;
		GLchar log[10000];
		glGetProgramInfoLog(program, 10000, NULL, log);
		cerr << log << endl;
		exit(1);
	}

	return program;

}

GLuint DragonDemo::createProgramPipeline(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader) {

	GLuint pipeline;
	glGenProgramPipelines(1, &pipeline);

	if (vertexShader != 0) glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexShader);
	if (geometryShader != 0) glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, geometryShader);
	if (fragmentShader != 0) glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentShader);

	glValidateProgramPipeline(pipeline);

	GLint status;
	glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, &status);
	if (status != GL_TRUE) {
		cerr << "Failed to validate program pipeline:" << endl;
		GLchar log[10000];
		glGetProgramPipelineInfoLog(pipeline, 10000, NULL, log);
		cerr << log << endl;
		exit(1);
	}

	return pipeline;

}

void DragonDemo::loadShaders() {

	cerr << "> Loading shaders..." << endl;

	// load G-buffer shaders
	GBufferVSP = loadShaderProgramFromFile("shaders/gbuffer.vs", GL_VERTEX_SHADER);
	GBufferFSP = loadShaderProgramFromFile("shaders/gbuffer.fs", GL_FRAGMENT_SHADER);
	GBufferPPL = createProgramPipeline(GBufferVSP, 0, GBufferFSP);

	// load light shaders
	lightVSP = loadShaderProgramFromFile("shaders/light.vs", GL_VERTEX_SHADER);
	lightGSP = loadShaderProgramFromFile("shaders/light.gs", GL_GEOMETRY_SHADER);
	lightFSP = loadShaderProgramFromFile("shaders/light.fs", GL_FRAGMENT_SHADER);
	lightPPL = createProgramPipeline(lightVSP, lightGSP, lightFSP);

	// load shadow shaders
	shadowMultiVSP = loadShaderProgramFromFile("shaders/shadow_multi.vs", GL_VERTEX_SHADER);
	shadowMultiGSP = loadShaderProgramFromFile("shaders/shadow_multi.gs", GL_GEOMETRY_SHADER);
	shadowMultiPPL = createProgramPipeline(shadowMultiVSP, shadowMultiGSP, 0);

	shadowSingleVSP = loadShaderProgramFromFile("shaders/shadow_single.vs", GL_VERTEX_SHADER);
	shadowSinglePPL = createProgramPipeline(shadowSingleVSP, 0, 0);
	lightIDUniformLoc = glGetUniformLocation(shadowSingleVSP, "lightID");
}

void DragonDemo::setupGBuffer() {

	cerr << "> Setting up G-buffer..." << endl;

	// create normal buffer
	glGenTextures(1, &positionGBT);
	glBindTexture(GL_TEXTURE_2D, positionGBT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create normal buffer
	glGenTextures(1, &normalGBT);
	glBindTexture(GL_TEXTURE_2D, normalGBT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create depth buffer
	glGenTextures(1, &depthGBT);
	glBindTexture(GL_TEXTURE_2D, depthGBT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
			GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create framebuffer and setup attachments
	glGenFramebuffers(1, &GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionGBT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalGBT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthGBT, 0);

	const GLenum buffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	CHECK_FRAMEBUFFER_STATUS();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	CHECK_GL_ERROR();

}

void DragonDemo::moveCamera(float dx, float dy, float dz, float dtime) {
	mat4 rotation = mat4(1.0f);
    rotation = rotate(rotation, camera.rotation.x, vec3(1.0f, 0.0f, 0.0f));
    rotation = rotate(rotation, camera.rotation.y, vec3(0.0f, 1.0f, 0.0f));
    rotation = rotate(rotation, camera.rotation.z, vec3(0.0f, 0.0f, 1.0f));
	camera.position += vec3(vec4(dtime*dx, dtime*dy, dtime*dz, 1.0f) * rotation);
	camera.position = clamp(camera.position, vec3(-7.5f, -5.5f, -7.5f), vec3(+7.5f, +5.5f, +7.5f));
}

void DragonDemo::rotateCamera(float dx, float dy, float dz, float dtime) {
	camera.rotation += vec3(dtime*dx, dtime*dy, dtime*dz);
	camera.rotation.x = clamp(camera.rotation.x, -90.0f, 90.0f);
}

void DragonDemo::renderScene(float dtime, bool useMultiShadow) {

	// update camera data to uniform buffer
	transform.ModelViewMatrix = mat4(1.0f);
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.x, vec3(1.0f, 0.0f, 0.0f));
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.y, vec3(0.0f, 1.0f, 0.0f));
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.z, vec3(0.0f, 0.0f, 1.0f));
	transform.ModelViewMatrix = translate(transform.ModelViewMatrix, -camera.position);
	transform.MVPMatrix = transform.ProjectionMatrix * transform.ModelViewMatrix;
	glBindBuffer(GL_UNIFORM_BUFFER, transformUB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), &transform, GL_STATIC_DRAW);

	// update light information
	lightRotationFactor = fmodf(lightRotationFactor + dtime * 0.05, 1.f);
	updateLights();

	// render G-buffer
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);
	glBindProgramPipeline(GBufferPPL);
	glBindVertexArray(vertexArray);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT);

	glDrawElements(drawCommand.prim_type, drawCommand.indexCount, GL_UNSIGNED_INT, (GLvoid*)drawCommand.indexOffset);

	// render shadows
	if (useMultiShadow) {

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFB);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArrayTex, 0);
		glBindProgramPipeline(shadowMultiPPL);
		glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawElements(drawCommand.prim_type, drawCommand.indexCount, GL_UNSIGNED_INT, (GLvoid*)drawCommand.indexOffset);

	} else {

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFB);
		glBindProgramPipeline(shadowSinglePPL);
		glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
		for (int i=0; i<NUMBER_OF_LIGHTS; ++i) {
			glProgramUniform1i(shadowSingleVSP, lightIDUniformLoc, i);
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowArrayTex, 0, i);
			glClear(GL_DEPTH_BUFFER_BIT);
			glDrawElementsInstanced(drawCommand.prim_type, drawCommand.indexCount, GL_UNSIGNED_INT, (GLvoid*)drawCommand.indexOffset, 4);
		}

	}

	// render lights
	glBindProgramPipeline(lightPPL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, positionGBT);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalGBT);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, depthGBT);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowArrayTex);

	glDrawArraysInstanced(GL_POINTS, 0, 1, NUMBER_OF_LIGHTS);

	CHECK_GL_ERROR();

}

} /* namespace dragon */
