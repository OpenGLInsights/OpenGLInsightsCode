/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#include "prec.h"
/* author: Pyar Knowles, date: 14/05/10 */

#include "camera.h"
#include <stdlib.h>
#include <stdio.h>

static const float pi = 3.14159265f;

Camera::Camera()
{
	transform = inverse = projection = mat44::zero();
	position(0.0f);
	euler(0.0f);
	bump(0.0f);
	offset = 0.0f;
	setDistance(0.1f, 100.0f);
	setPerspective(pi * 0.416667f);
	setAspectRatio(1.0f);
	regen();
}
void Camera::regen()
{
	regenProjection();
	regenCamera();
}
void Camera::regenProjection()
{
	if (bPerspective)
	{
		float tanfov = tan(fov*0.5f);
		float t = nearPlane * tanfov;
		float r = nearPlane * tanfov * aspect;
		projection.m[0*4+0] = nearPlane / r;
		projection.m[1*4+1] = nearPlane / t;
		projection.m[2*4+2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		projection.m[3*4+2] = -2.0f * (farPlane * nearPlane) / (farPlane - nearPlane);
		projection.m[2*4+3] = -1.0f;
	}
	else
	{
		float t = size * 0.5f;
		float r = size * 0.5f * aspect;
		projection.m[0*4+0] = 1.0f / r;
		projection.m[1*4+1] = 1.0f / t;
		projection.m[2*4+2] = -2.0f / (farPlane - nearPlane);
		projection.m[3*4+2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
		projection.m[3*4+3] = 1.0f;
	}
}
void Camera::regenCamera()
{
	euler.x = fmod(euler.x, 2.0f * pi);
	euler.y = fmod(euler.y, 2.0f * pi);
	euler.z = fmod(euler.z, 2.0f * pi);
	transform = mat44::translate(position);
	transform *= mat44::rotate(euler.z, vec3f(0.0, 0.0, 1.0));
	transform *= mat44::rotate(euler.y, vec3f(0.0, 1.0, 0.0));
	transform *= mat44::rotate(euler.x, vec3f(1.0, 0.0, 0.0));
	if (offset > 0.0f)
		transform *= mat44::translate(0.0, 0.0, offset);
	if (bump.sizesq() > 0.001f)
	{
		transform *= mat44::rotate(bump.z, vec3f(0.0, 0.0, 1.0));
		transform *= mat44::rotate(bump.y, vec3f(0.0, 1.0, 0.0));
		transform *= mat44::rotate(bump.x, vec3f(1.0, 0.0, 0.0));
	}
	
	/*
	transform *= mat44::rotate(euler.x, vec3f(1.0, 0.0, 0.0));
	transform *= mat44::rotate(euler.x, vec3f(0.0, 1.0, 0.0));
	transform *= mat44::rotate(euler.x, vec3f(0.0, 0.0, 1.0));
	*/
	inverse = transform.inverse();
}
void Camera::setZoom(float d)
{
	offset = d;
}
void Camera::lookAt(vec3f from, vec3f to)
{
	vec3f dir = to - from;
	if (dir.sizesq() == 0.0f) return;
	setRotation(vec2f::fromVec(dir));
	setPosition(from);
	offset = 0.0f;
}
void Camera::zoomAt(vec3f from, vec3f to)
{
	vec3f dir = to - from;
	float lensq = dir.sizesq();
	if (lensq == 0.0f) return;
	setRotation(vec2f::fromVec(dir));
	setPosition(to);
	setZoom(sqrt(lensq));
}
void Camera::setAspectRatio(float r)
{
	aspect = r;
}
void Camera::setDistance(float n, float f)
{
	nearPlane = n;
	farPlane = f;
}
void Camera::setPerspective(float a)
{
	if (a > pi)
		printf("Error: Camera perspective greater than pi.\n");
	if (!bPerspective)
		projection = mat44::zero();
	bPerspective = true;
	fov = a;
}
void Camera::setOrthographic(float s)
{
	if (bPerspective)
		projection = mat44::zero();
	bPerspective = false;
	size = s;
}
void Camera::setPosition(const vec3f& pos)
{
	position = pos;
	regenCamera();
}
void Camera::setRotation(const vec3f& angle)
{
	euler = angle;
}
void Camera::setRotation(const vec2f& angle)
{
	euler = vec3f(angle, 0.0f);
}
void Camera::setOffset(const vec3f& angle)
{
	bump = angle;
}
void Camera::move(const vec3f& dir)
{
	position += dir;
}
void Camera::rotate(const vec3f& angle)
{
	euler += angle;
	fixEuler();
}
void Camera::rotate(const vec2f& angle)
{
	euler += vec3f(angle, 0.0f);
	fixEuler();
}
void Camera::fixEuler()
{
	if (euler.x >= pi*0.5f) euler.x = pi*0.5f-0.001f;
	if (euler.x <= -pi*0.5f) euler.x = -pi*0.5f+0.001f;
	if (euler.y > pi) euler.y -= pi*2.0f;
	if (euler.y <= -pi) euler.y += pi*2.0f;
	if (euler.z > pi) euler.y -= pi*2.0f;
	if (euler.z <= -pi) euler.y += pi*2.0f;
}
void Camera::zoom(float d)
{
	offset += d;
	if (offset < 0.0f)
		offset = 0.0f;
}
float Camera::getAspectRatio()
{
	return aspect;
}
vec3f& Camera::getPosition()
{
	return position;
}
vec3f& Camera::getRotation()
{
	return euler;
}
vec3f Camera::getZoomPos()
{
	return getPosition() - toVec() * offset;
}
vec2f Camera::getClipPlanes()
{
	return vec2f(nearPlane, farPlane);
}
mat44& Camera::getTransform()
{
	return transform;
}
mat44& Camera::getInverse()
{
	return inverse;
}
mat44& Camera::getProjection()
{
	return projection;
}
vec3f Camera::toVec()
{
	return vec2f(euler).toVec();
}
vec3f Camera::upVec()
{
	return (vec2f(euler) + vec2f(pi*0.5f, 0.0f)).toVec();
}
vec3f Camera::rightVec()
{
	return vec2f(0.0f, euler.y - pi*0.5f).toVec();
}
float Camera::getZoom()
{
	return offset;
}
