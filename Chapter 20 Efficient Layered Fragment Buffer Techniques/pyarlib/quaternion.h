/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */

#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

#include "vec.h"
#include "matrix.h"

struct Quat {
	float x;
	float y;
	float z;
	float w;
	Quat();
	Quat(float nx, float ny, float nz, float nw);
	Quat(float angle, vec3f vec);
	explicit Quat(mat33 m);
	explicit Quat(mat44 m);
	Quat operator+(const Quat& A) const; //sum quat components
	Quat operator-(const Quat& A) const; //sub quat components
	Quat operator*(float d) const; //scale quat components
	Quat operator*(const Quat& A) const; //quaternion rotation
	vec3f operator*(const vec3f& v) const; //rotate vector (via mat33)
	Quat operator/(float d) const; //divide quat components
	Quat operator/(const Quat& A) const; //return Quat rotation from A to *this
	void operator+=(const Quat& A); //sum quat components
	void operator-=(const Quat& A); //sub quat components
	void operator*=(float d); //scale quat components
	void operator*=(const Quat& A); //quaternion rotation
	void operator/=(float d); //divide quat components
	float operator[](int index);
	bool operator==(const Quat& A); //test if all elements match. FIXME: use epsilon
	bool operator!=(const Quat& A);
	void angleAxis(float& a, vec3f& v) const;
	float getAngle(void) const;
	vec3f getAxis(void) const;
	Quat inverse(void) const;
	float dot(const Quat& A) const;
	vec3f euler(void) const; //pitch, yaw, roll
	float sqsize(void) const; //this squared + A squared
	float size(void) const; //length of vector
	Quat unit(void) const; //returns normalized quat
	void normalize(void);
	mat44 getMatrix(); //returns the matrix representation of this quaternion. Note: normalize first!
	Quat slerp(Quat q, float d) const;
	Quat lerp(Quat q, float d) const;
	Quat nlerp(Quat q, float d) const;
	static Quat identity();
	static Quat fromEuler(float pitch, float yaw, float roll);
	static Quat fromEuler(const vec2f& angles);
	static Quat fromEuler(const vec3f& angles);
	static Quat fromTo(const vec3f& from, const vec3f& to);
	static Quat dirUp(const vec3f& dir, const vec3f& up = vec3f(0, 1, 0));
};

Quat operator-(const Quat& q);

#endif
