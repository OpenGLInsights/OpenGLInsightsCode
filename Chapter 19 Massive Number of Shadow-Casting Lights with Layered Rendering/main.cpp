/*
 * main.cpp
 *
 *  Created on: Sep 24, 2011
 *      Author: aqnuep
 */

#include <iostream>
#include <sstream>
#include <string>
#include <cstdio>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>
#include "wavefront.h"
#include "dragon.h"

#define MAIN_TITLE			"OpenGL 4.2 Dragon demo"
#define SINGLE_MODE			"Traditional shadow map generation"
#define MULTI_MODE			"Layered shadow map generation"

using namespace std;

int close() {
	glfwTerminate();
	exit(0);
}

int main() {

	if (glfwInit() != GL_TRUE) {
		cerr << "Error: unable to initialize GLFW" << endl;
		return 0;
	}

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	if (glfwOpenWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 8,8,8,8, 24,8, GLFW_WINDOW) != GL_TRUE) {
		cerr << "Error: unable to open GLFW window" << endl;
		glfwTerminate();
		return 0;
	}

	glfwSetWindowTitle(MAIN_TITLE);

	glfwSetWindowCloseCallback(&close);

	GLenum glewError;
	if ((glewError = glewInit()) != GLEW_OK) {
		cerr << "Error: " << glewGetErrorString(glewError) << endl;
		glfwTerminate();
		return 0;
	}

	if (!GLEW_VERSION_4_2) {
		cerr << "Error: OpenGL 4.2 is required" << endl;
		glfwTerminate();
		return 0;
	}

	// Set color and depth clear value
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 0.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	double frameTime = 0.01;
	double lastFrameEnd = glfwGetTime();
	double lastFPS = glfwGetTime();
	int frameCount = 0;
	bool mouseActive = false;
	bool useMultiShadow = false;

	dragon::DragonDemo demoScene;

	while (true) {
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
            mouseActive = true;
            glfwSetMousePos( SCREEN_WIDTH/2, SCREEN_HEIGHT/2 );
            glfwDisable(GLFW_MOUSE_CURSOR);
		}
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
            mouseActive = false;
            glfwEnable(GLFW_MOUSE_CURSOR);
		}
		if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
			useMultiShadow = not useMultiShadow;
			while (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
				glfwPollEvents();
			}
		}

		if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) break;

		if (glfwGetKey('W') == GLFW_PRESS) demoScene.moveCamera(  0.0, 0.0,-50.0, 0.001 );
		if (glfwGetKey('S') == GLFW_PRESS) demoScene.moveCamera(  0.0, 0.0, 50.0, 0.001 );
		if (glfwGetKey('A') == GLFW_PRESS) demoScene.moveCamera(-50.0, 0.0,  0.0, 0.001 );
		if (glfwGetKey('D') == GLFW_PRESS) demoScene.moveCamera( 50.0, 0.0,  0.0, 0.001 );

		if ( mouseActive ) {
			int mx, my;
			glfwGetMousePos(&mx, &my);
			demoScene.rotateCamera( (my-SCREEN_HEIGHT/2)*30.0, (mx-SCREEN_WIDTH/2)*30.0, 0.0, 0.01 );
            glfwSetMousePos( SCREEN_WIDTH/2, SCREEN_HEIGHT/2 );
		}

		demoScene.renderScene(frameTime, useMultiShadow);

		frameCount++;
		if (glfwGetTime() - lastFPS > 1.0) {
			double fps = (double)frameCount / (glfwGetTime() - lastFPS);
			frameCount = 0;
			char title[256];
			sprintf(title, "%s - %s (press SPACE to switch) - %.2f fps", MAIN_TITLE, useMultiShadow ? MULTI_MODE : SINGLE_MODE, fps );
			glfwSetWindowTitle(title);
			lastFPS = glfwGetTime();
		}

       	glfwSwapBuffers();

       	double newFrameEnd = glfwGetTime();
       	frameTime = newFrameEnd - lastFrameEnd;
       	lastFrameEnd = newFrameEnd;
	}

	glfwTerminate();
	return 0;
}
