#ifndef Vector3H
#define Vector3H

#include "Matrix.h"
#include <math.h>

class Vector3 {
public:
	float x;
	float y;
	float z;

	void init(float x_, float y_, float z_);
	Vector3(float x_, float y_, float z_);

	Vector3();

	float length();

	void normalize();

	Vector3 normalized();
	//Vector3 operator * (const Vector3 &v);

	Vector3 operator * (const Matrix &m);

	void clear();
};

#endif