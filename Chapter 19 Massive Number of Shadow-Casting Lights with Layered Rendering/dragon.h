/*
 * dragon.h
 *
 *  Created on: Sep 24, 2011
 *      Author: aqnuep
 */

#ifndef DRAGON_H_
#define DRAGON_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include "wavefront.h"

#define SCREEN_WIDTH         1024
#define SCREEN_HEIGHT        768

#define NUMBER_OF_LIGHTS	   10
#define SHADOW_MAP_SIZE		 1024

namespace dragon {

struct Transform {
	glm::mat4 ModelViewMatrix;		// modelview matrix of the transformation
	glm::mat4 ProjectionMatrix;		// projection matrix of the transformation
	glm::mat4 MVPMatrix;			// modelview-projection matrix
	glm::vec4 Viewport;				// viewport dimensions
	GLfloat zNear;					// z near distance
	GLfloat zFar;					// z far distance
	GLfloat nearTop;				// near plane's top limit
	GLfloat nearRight;				// near plane's right limit
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
	GLenum prim_type;				// primitive type
	glm::uint indexOffset;			// offset into the index buffer
	glm::uint indexCount;			// number of indices
};

struct Light {
	glm::vec4 position;				// light position
	glm::vec4 color;				// light color
};

struct LightTransform {
	glm::mat4 VPMatrix;				// light's shadow rendering view-projection matrix
};

class DragonDemo {
protected:

	Camera camera;					// camera data

	Transform transform;			// transformation data
	GLuint transformUB;				// uniform buffer for the transformation

	GLuint GBufferVSP;				// G-Buffer vertex shader program
	GLuint GBufferFSP;				// G-Buffer fragment shader program
	GLuint GBufferPPL;				// G-Buffer program pipeline

	GLuint lightVSP;				// light vertex shader program
	GLuint lightGSP;				// light geometry shader program
	GLuint lightFSP;				// light fragment shader program
	GLuint lightPPL;				// light program pipeline

	GLuint shadowSingleVSP;			// single shadow vertex shader program
	GLuint shadowSinglePPL;			// single shadow program pipeline
	GLint lightIDUniformLoc;		// location of the lightID uniform

	GLuint shadowMultiVSP;			// multi shadow vertex shader program
	GLuint shadowMultiGSP;			// multi shadow geometry shader program
	GLuint shadowMultiPPL;			// multi shadow program pipeline

	GLuint indexBuffer;				// index buffer for the mesh
	GLuint vertexBuffer;			// vertex buffer for the mesh
	GLuint vertexArray;				// vertex array for the mesh

	DrawCommand drawCommand;		// draw command for the scene

	GLuint positionGBT;				// position texture for G-buffer
	GLuint normalGBT;				// normal texture for G-buffer
	GLuint depthGBT;				// depth texture for G-buffer
	GLuint GBuffer;					// G-buffer framebuffer

	GLuint lightUB;					// uniform buffer for the light array
	Light lights[NUMBER_OF_LIGHTS];	// local light array
	GLuint lightTransformUB;		// uniform buffer for shadow rendering VP matrices
	LightTransform lightTransform[NUMBER_OF_LIGHTS]; // local shadow rendering VP matrices

	GLuint shadowArrayTex;			// shadow map texture cube array
	GLuint shadowFB;				// shadow rendering framebuffer

	float lightRotationFactor;		// light rotation factor between [0,1)

	void loadModels();
	void loadShaders();
	void setupGBuffer();
	void createLights();
	void updateLights();

	GLuint loadShaderProgramFromFile(const char* filename, GLenum shaderType);
	GLuint createProgramPipeline(GLuint vertexShader, GLuint geometryShader, GLuint fragmentShader);

public:

	DragonDemo();
	~DragonDemo();

	void moveCamera(float dx, float dy, float dz, float dtime);
	void rotateCamera(float dx, float dy, float dz, float dtime);

	void renderScene(float dtime, bool useMultiShadow);

};

} /* namespace dragon */

#endif /* DRAGON_H_ */
