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
#include "buddha.h"

#define CHECK_GL_ERROR()												\
{																		\
	GLenum glError;														\
	if ((glError = glGetError()) != GL_NO_ERROR) {						\
		cerr << "OpenGL error code in '" << __FILE__ << "' at line " << __LINE__ << ": " << gluErrorString(glError) << endl;	\
	}																	\
}

using namespace glm;
using namespace demo;

namespace buddha {

BuddhaDemo::BuddhaDemo() {

	cerr << "> Initializing scene data..." << endl;

	// initialize camera data
	cameraRotationFactor = 0.f;
	transform.ProjectionMatrix = perspective(45.f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 40.f);

	// create uniform buffer and store camera data
	glGenBuffers(1, &transformUB);
	glBindBuffer(GL_UNIFORM_BUFFER, transformUB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), &transform, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, transformUB);

	loadShaders();
	loadModels();

	glEnable(GL_FRAMEBUFFER_SRGB);

	cerr << "> Configuring rendering environment..." << endl;

	glEnable(GL_CULL_FACE);

	cerr << "> Done!" << endl;

	CHECK_GL_ERROR();
}

BuddhaDemo::~BuddhaDemo() {

	cerr << "> Freeing graphics resources... ";

	// delete shader programs
	glDeleteProgram(vertexProg[FFX_MODE]);
	glDeleteProgram(vertexProg[FETCHER_MODE]);
	glDeleteProgram(vertexProg[PULLER_MODE]);
	glDeleteProgram(fragmentProg);

	// delete program pipelines
	glDeleteProgramPipelines(NUMBER_OF_MODES, progPipeline);

	// delete uniform buffers
	glDeleteBuffers(1, &transformUB);

	// delete vertex buffer
	glDeleteBuffers(1, &vertexBuffer);

	// delete index buffer
	glDeleteBuffers(1, &indexBuffer);

	// delete vertex arrays
	glDeleteVertexArrays(1, &vertexArray);

	// delete buffer textures
	glDeleteTextures(1, &indexTexBuffer);
	glDeleteTextures(1, &vertexTexBuffer);

	cerr << "Done!" << endl;

}

void BuddhaDemo::loadModels() {

	cerr << "> Loading models..." << endl;

	WaveFrontObj buddhaObj("models/buddha.obj");

	cerr << "> Uploading mesh data to GPU..." << endl;

	// create empty index buffer
	int indexBufferSize = buddhaObj.index.size() * sizeof(uint);
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

	// map index buffer and fill with data
	uint *index = (uint*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indexBufferSize,
						GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (uint i=0; i<buddhaObj.index.size(); ++i)
		index[i] = buddhaObj.index[i];

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// create empty vertex buffer
	int vertexBufferSize = buddhaObj.vertex.size() * sizeof(Vertex);
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

	// map vertex buffer and fill with data
	Vertex *vertex = (Vertex*)glMapBufferRange(GL_ARRAY_BUFFER, 0, vertexBufferSize,
						GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (uint i=0; i<buddhaObj.vertex.size(); ++i) {
		vertex[i].position[0] = buddhaObj.vertex[i].x;
		vertex[i].position[1] = buddhaObj.vertex[i].y;
		vertex[i].position[2] = buddhaObj.vertex[i].z;
		vertex[i].normal[0] = buddhaObj.normal[i].x;
		vertex[i].normal[1] = buddhaObj.normal[i].y;
		vertex[i].normal[2] = buddhaObj.normal[i].z;
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

	// setup draw command for fixed-function vertex pulling
	drawCmd[FFX_MODE].useIndices = true;
	drawCmd[FFX_MODE].prim_type = GL_TRIANGLES;
	drawCmd[FFX_MODE].indexOffset = 0;
	drawCmd[FFX_MODE].indexCount = buddhaObj.index.size();

	// setup draw command for programmable attribute fetching
	drawCmd[FETCHER_MODE].useIndices = true;
	drawCmd[FETCHER_MODE].prim_type = GL_TRIANGLES;
	drawCmd[FETCHER_MODE].indexOffset = 0;
	drawCmd[FETCHER_MODE].indexCount = buddhaObj.index.size();

	// setup draw command for fully programmable vertex pulling
	drawCmd[PULLER_MODE].useIndices = false;
	drawCmd[PULLER_MODE].prim_type = GL_TRIANGLES;
	drawCmd[PULLER_MODE].firstVertex = 0;
	drawCmd[PULLER_MODE].vertexCount = buddhaObj.index.size();

	// create auxiliary texture buffers
	glGenTextures(1, &indexTexBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, indexTexBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, indexBuffer);
	glGenTextures(1, &vertexTexBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, vertexTexBuffer);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, vertexBuffer);

	CHECK_GL_ERROR();

}

GLuint BuddhaDemo::loadShaderProgramFromFile(const char* filename, GLenum shaderType) {

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

GLuint BuddhaDemo::createProgramPipeline(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader) {

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

void BuddhaDemo::loadShaders() {

	cerr << "> Loading shaders..." << endl;

	// load common fragment shader
	fragmentProg = loadShaderProgramFromFile("shaders/common.fs", GL_FRAGMENT_SHADER);

	// load fixed-function vertex pulling shader
	vertexProg[FFX_MODE] = loadShaderProgramFromFile("shaders/ffx.vs", GL_VERTEX_SHADER);
	progPipeline[FFX_MODE] = createProgramPipeline(vertexProg[FFX_MODE], 0, fragmentProg);

	// load programmable attribute fetching shader
	vertexProg[FETCHER_MODE] = loadShaderProgramFromFile("shaders/fetcher.vs", GL_VERTEX_SHADER);
	progPipeline[FETCHER_MODE] = createProgramPipeline(vertexProg[FETCHER_MODE], 0, fragmentProg);

	// load fully programmable vertex pulling shader
	vertexProg[PULLER_MODE] = loadShaderProgramFromFile("shaders/puller.vs", GL_VERTEX_SHADER);
	progPipeline[PULLER_MODE] = createProgramPipeline(vertexProg[PULLER_MODE], 0, fragmentProg);

}

void BuddhaDemo::renderScene(float dtime, VertexPullingMode mode) {

	// update camera position
	cameraRotationFactor = fmodf(cameraRotationFactor + dtime * 0.3, 2.f * M_PI);
	camera.position = vec3(sin(cameraRotationFactor) * 5.f, 0.f, cos(cameraRotationFactor) * 5.f);
	camera.rotation = vec3(0.f, (-cameraRotationFactor * 180.f) / M_PI, 0.f);

	// update camera data to uniform buffer
	transform.ModelViewMatrix = mat4(1.0f);
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.x, vec3(1.0f, 0.0f, 0.0f));
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.y, vec3(0.0f, 1.0f, 0.0f));
	transform.ModelViewMatrix = rotate(transform.ModelViewMatrix, camera.rotation.z, vec3(0.0f, 0.0f, 1.0f));
	transform.ModelViewMatrix = translate(transform.ModelViewMatrix, -camera.position);
	transform.MVPMatrix = transform.ProjectionMatrix * transform.ModelViewMatrix;
	glBindBuffer(GL_UNIFORM_BUFFER, transformUB);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(transform), &transform, GL_STATIC_DRAW);

	// render scene
	glBindProgramPipeline(progPipeline[mode]);

	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (drawCmd[mode].useIndices) {

		glDrawElements(drawCmd[mode].prim_type, drawCmd[mode].indexCount, GL_UNSIGNED_INT, (GLvoid*)drawCmd[mode].indexOffset);

	} else {

		glDrawArrays(drawCmd[mode].prim_type, drawCmd[mode].firstVertex, drawCmd[mode].vertexCount);

	}

	CHECK_GL_ERROR();

}

} /* namespace buddha */
