/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


#ifndef MAT_STACK_H
#define MAT_STACK_H

#include <stack>
#include "matrix.h"

class MatrixStack : public mat44
{
private:
	std::stack<mat44> mats;
public:
	MatrixStack(const mat44& m = mat44::identity());
	mat44& operator=(const mat44& m);
	void push();
	void push(const mat44& m);
	void pop();
	void translate(vec3f v);
	void translate(float x, float y, float z);
	void rotate(float a, vec3f v);
	void rotate(float a, float x, float y, float z);
	void scale(vec3f v);
	void scale(float x, float y, float z);
	void clear();
	mat33 normalMatrix();
};

#endif
