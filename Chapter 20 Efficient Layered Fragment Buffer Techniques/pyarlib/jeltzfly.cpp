/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"

#include "jeltzfly.h"

#include <fstream>
#include <ostream>

#include "includegl.h"
#include "shader.h"

using namespace std;

const float pi = 3.14159265f;
const float to_rad = pi/180.0f;

JeltzFly::JeltzFly()
{
	camera.setPerspective(60.0f * (pi/180.0f));
	camera.setDistance(0.01f, 100.0f);
	camera.regen();
	
	sensitivity = 0.5;
	speed = 1.0;
	interpolating = false;
	snapSpeed = 3.0;
	orbit = vec3f(0.0f);
	saveOnExit = true;
	loadCamera();
}
JeltzFly::~JeltzFly()
{
	if (saveOnExit)
		saveCamera();
}
void JeltzFly::update(float dt)
{
	bool moved = false;
	float mspeed = speed;
	
	//increase wasd speed with shift
	if (jeltz->button("Left Shift"))
		mspeed *= 10.0;
		
	//standard wasd fly movement
	if (jeltz->button("A"))
		{camera.move(camera.rightVec() * mspeed * -dt); moved = true;}
	if (jeltz->button("D"))
		{camera.move(camera.rightVec() * mspeed * dt); moved = true;}
	if (jeltz->button("S"))
		{camera.move(camera.toVec() * mspeed * -dt); moved = true;}
	if (jeltz->button("W"))
		{camera.move(camera.toVec() * mspeed * dt); moved = true;}
	
	//un-orbit with wasd, toggle wth space
	bool toggleOrbit = jeltz->buttonDown("Space");
	bool orbiting = camera.getZoom() > 0.0;
	if ((toggleOrbit || moved) && orbiting)
	{
		camera.setPosition(camera.getZoomPos());
		camera.setZoom(0.0);
		moved = true;
	}
	else if (toggleOrbit && !orbiting)
	{
		interpolating = true;
		interpFrom = Quat(camera.getTransform());
		//interpFrom = Quat::dirUp(camera.toVec());
		//interpFrom = Quat::fromEuler(camera.getRotation());
		interpTo = Quat::dirUp(orbit - camera.getPosition());
		interpFrom.normalize();
		interpTo.normalize();
		interpRatio = 0.0f;
		float a = (interpTo / interpFrom).unit().getAngle();
		a = std::max(0.001f, a);
		msnapSpeed = snapSpeed / a;
	}
	
	//middle mouse pan
	if (jeltz->button("MButton"))
	{
		vec2i move = jeltz->mouseMove();
		if (move.x != 0 || move.y != 0)
		{
			float msensitivity = mspeed * sensitivity * 0.01f;
			if (jeltz->button("Left Ctrl"))
				msensitivity *= 0.1f;
			vec2f dir = vec2f(-move.x, move.y) * msensitivity;
			camera.move(camera.upVec() * dir.y + camera.rightVec() * dir.x);
			moved = true;
		}
	}
	
	//left mouse rotate
	if (jeltz->button("LButton"))
	{
		vec2i move = jeltz->mouseMove();
		if (move.x != 0 || move.y != 0)
		{
			float msensitivity = sensitivity;
			if (jeltz->button("Left Ctrl"))
				msensitivity *= 0.1;
			camera.rotate(vec2f(-move.y, -move.x) * to_rad * msensitivity);
			moved = true;
		}
	}
	
	//right mouse zoom
	if (jeltz->button("RButton"))
	{
		vec2i move = jeltz->mouseMove();
		if (move.y != 0)
		{
			float msensitivity = sensitivity * 0.01;
			if (jeltz->button("Left Ctrl"))
				msensitivity *= 0.1;
			camera.zoom(-move.y * (camera.getZoom() + 1.0) * msensitivity);
			moved = true;
		}
	}
	
	//smooth snap to orbit position
	if (interpolating)
	{
		if (moved)
		{
			//remove roll and stop interpolating
			camera.setRotation(camera.getRotation() * vec3f(1,1,0));
			interpolating = false;
		}
		else
		{
			interpRatio += dt * msnapSpeed;
			if (interpRatio >= 1.0f)
			{
				interpolating = false;
				camera.zoomAt(camera.getPosition(), orbit);
			}
			else
			{
				interpPos = interpFrom.slerp(interpTo, interpRatio);
				//camera.lookAt(camera.getPosition(), camera.getPosition() + interpPos * vec3f(0,0,-1));
				camera.setRotation(interpPos.euler()); //roll too :D
			}
			moved = true;
		}
	}
	
	//only regen matrix if the camera has changed
	if (moved)
		camera.regenCamera();
		
	//update projection aspect ratio if window resized
	if (jeltz->resized())
	{
		camera.setAspectRatio(jeltz->winSize().x/(float)jeltz->winSize().y);
		camera.regenProjection();
	}
}
void JeltzFly::loadCamera()
{
	vec3f pos;
	vec3f rot;
	float z;
	ifstream ifile(".camera.txt");
	if (ifile.is_open())
	{
		ifile >> pos.x >> pos.y >> pos.z;
		ifile >> rot.x >> rot.y >> rot.z;
		ifile >> z;
		ifile.close();
		camera.setZoom(z);
		camera.setRotation(rot);
		camera.setPosition(pos);
		camera.regenCamera();
	}
}
void JeltzFly::saveCamera()
{
	vec3f pos = camera.getPosition();
	vec3f rot = camera.getRotation();
	float z = camera.getZoom();
	ofstream ofile(".camera.txt");
	ofile << pos.x << " " << pos.y << " " << pos.z << endl;
	ofile << rot.x << " " << rot.y << " " << rot.z << endl;
	ofile << z << endl;
	ofile.close();
}
void JeltzFly::uploadCamera(Shader* shader)
{
	if (shader->active == shader)
	{
		shader->set("normalMat", mat33(camera.getTransform().transpose()));
		shader->set("modelviewMat", camera.getInverse());
		shader->set("projectionMat", camera.getProjection());
	}
	else
		printf("Please \"%s\".use() before .uploadCamera()\n", shader->name().c_str());
}
void JeltzFly::uploadCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(camera.getProjection().m);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera.getInverse().m);
}
