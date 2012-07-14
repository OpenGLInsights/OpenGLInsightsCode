/*
 * buddha.h
 *
 *  Created on: Sep 24, 2011
 *      Author: aqnuep
 */

#ifndef BUDDHA_H_
#define BUDDHA_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include "wavefront.h"

#define SCREEN_WIDTH         1024
#define SCREEN_HEIGHT        768

namespace buddha {

struct Transform {
	glm::mat4 ModelViewMatrix;		// modelview matrix of the transformation
	glm::mat4 ProjectionMatrix;		// projection matrix of the transformation
	glm::mat4 MVPMatrix;			// modelview-projection matrix
};

struct Camera {
	glm::vec3 position;				// camera position
	glm::vec3 rotation;				// camera rotation
};

struct Vertex {
	glm::vec3 position;				// vertex position
	glm::vec3 normal;				// vertex normal
};

struct DrawCommand {
	bool useIndices;				// specifies whether this is an indexed draw command
	GLenum prim_type;				// primitive type
	union {
		struct {
			glm::uint indexOffset;	// offset into the index buffer
			glm::uint indexCount;	// number of indices
		};
		struct {
			glm::uint firstVertex;	// first vertex index
			glm::uint vertexCount;	// number of vertices
		};
	};
};

enum VertexPullingMode
{
	FFX_MODE,			// fixed-function vertex pulling
	FETCHER_MODE,		// programmable attribute fetching
	PULLER_MODE,		// fully programmable vertex pulling
	NUMBER_OF_MODES
};

class BuddhaDemo {
protected:

	Camera camera;							// camera data

	Transform transform;					// transformation data
	GLuint transformUB;						// uniform buffer for the transformation

	GLuint fragmentProg;					// common fragment shader program
	GLuint vertexProg[NUMBER_OF_MODES];		// vertex shader programs for the three vertex pulling modes
	GLuint progPipeline[NUMBER_OF_MODES];	// program pipelines for the three vertex pulling modes

	GLuint indexBuffer;						// index buffer for the mesh
	GLuint vertexBuffer;					// vertex buffer for the mesh

	GLuint vertexArray;						// vertex array for the three vertex pulling modes

	GLuint indexTexBuffer;					// index buffer texture
	GLuint vertexTexBuffer;					// vertex buffer texture

	DrawCommand drawCmd[NUMBER_OF_MODES];	// draw command for the three vertex pulling modes

	float cameraRotationFactor;		// camera rotation factor between [0,2*PI)

	void loadModels();
	void loadShaders();

	GLuint loadShaderProgramFromFile(const char* filename, GLenum shaderType);
	GLuint createProgramPipeline(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);

public:

	BuddhaDemo();
	~BuddhaDemo();

	void renderScene(float dtime, VertexPullingMode mode);

};

} /* namespace buddha */

#endif /* BUDDHA_H_ */
