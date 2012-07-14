/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef JELTZ_FLY
#define JELTZ_FLY

#include "jeltz.h"
#include "camera.h"
#include "quaternion.h"

class Shader;

class JeltzFly : public JeltzPlugin
{
private:
	virtual void update(float dt);
	Quat interpFrom;
	Quat interpTo;
	Quat interpPos;
	float msnapSpeed;
	float interpRatio;
	bool interpolating;
public:
	bool saveOnExit; //switch off to stop auto saving to .camera.txt
	vec3f orbit;
	float snapSpeed; //speed to snap to orbit (rads per second)
	float sensitivity;
	float speed;
	Camera camera; //the public camera class. usage eg: glLoadMatrixf(jeltzFly.camera.getInverse().m);
	JeltzFly();
	virtual ~JeltzFly();
	void loadCamera();
	void saveCamera();
	void uploadCamera(Shader* shader); //sets "modelviewMat" and "projectionMat"
	void uploadCamera(); //sets deprecated GL projection/modelview matrices
};

#endif
