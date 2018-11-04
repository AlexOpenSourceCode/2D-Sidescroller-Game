#include "Vector3.h"



void Vector3::init(float x_, float y_, float z_){
	x = x_;
	y = y_;
	z = z_;
}

Vector3::Vector3(float x_, float y_, float z_){
	init(x_, y_, z_);
}

Vector3::Vector3(){
	init(0, 0, 0);
}

float Vector3::length(){
	return sqrt(pow(x, 2) + pow(y, 2)) + pow(z, 2);
}

void Vector3::normalize(){
	float len = length();
	x /= len;
	y /= len;
	z /= len;
}

Vector3 Vector3::normalized(){
	float len = length();
	Vector3 v3(x / len, y / len, z / len);
	return v3;
}

//Vector3 Vector3::operator * (const Vector3 &v){
//
//}

Vector3 Vector3::operator * (const Matrix &m){
	float x_val = (m.m[0][0] * x) + (m.m[1][0] * y) + (m.m[2][0] * z);
	float y_val = (m.m[0][1] * x) + (m.m[1][1] * y) + (m.m[2][1] * z);
	float z_val = (m.m[0][2] * x) + (m.m[1][2] * y) + (m.m[2][2] * z);


	//float x_val = (m.m[0][0] * x) + (m.m[0][1] * y) + (m.m[0][2] * z);
	//float y_val = (m.m[1][0] * x) + (m.m[1][1] * y) + (m.m[1][2] * z);
	//float z_val = (m.m[2][0] * x) + (m.m[2][1] * y) + (m.m[2][2] * z);

	Vector3 new_v3(x_val, y_val, z_val);
	return new_v3;
}

void Vector3::clear(){
	init(0, 0, 0);
}
