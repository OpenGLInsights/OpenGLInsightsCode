/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#pragma once
/* author: Pyar Knowles, date: 14/05/10 */

#ifndef CAMERA_H
#define CAMERA_H

#include "vec.h"
#include "matrix.h"

struct Camera
{
private:
	//projection matrices
	mat44 projection;
	bool bPerspective;
	float aspect;

	//camera matrices
	mat44 transform;
	mat44 inverse;
	float offset;
	
	//camera vars
	vec3f position;
	vec3f euler;
	vec3f bump;
	
	//projection depth vars
	float nearPlane;
	float farPlane;
	
	//projection width/height vars
	float size;
	float fov;
	
	void fixEuler();
public:
	Camera();
	void regen();
	void regenProjection();
	void regenCamera();
	void setZoom(float d);
	void lookAt(vec3f from, vec3f to);
	void zoomAt(vec3f from, vec3f to);
	void setAspectRatio(float r);
	void setDistance(float n, float f);
	void setPerspective(float a);
	void setOrthographic(float s);
	void setPosition(const vec3f& pos);
	void setRotation(const vec2f& angle);
	void setRotation(const vec3f& angle);
	void setOffset(const vec3f& angle);
	void move(const vec3f& dir);
	void rotate(const vec2f& angle);
	void rotate(const vec3f& angle);
	void zoom(float d);
	float getAspectRatio();
	vec3f& getPosition();
	vec3f& getRotation();
	vec3f getZoomPos();
	vec2f getClipPlanes();
	mat44& getTransform();
	mat44& getInverse();
	mat44& getProjection();
	vec3f toVec();
	vec3f upVec();
	vec3f rightVec();
	float getZoom();
};

#endif
