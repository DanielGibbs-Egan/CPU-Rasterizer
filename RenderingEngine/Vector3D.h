
#pragma once
#include <vector>
#include <string>

#ifndef VECTOR3D_H_   /* Include guard */
#define VECTOR3D_H_

class Vector3D {
private:

	void init(double x, double y, double z);

public:

	double x;
	double y;
	double z;

	Vector3D();
	Vector3D(double shared);
	Vector3D(double x, double y, double z);

	Vector3D operator + (Vector3D v);

	Vector3D operator - (Vector3D v);

	Vector3D operator - ();

	void operator -= (Vector3D v);

	Vector3D operator * (double d);

	Vector3D operator / (double d);

	double magnitude();

	Vector3D unit();

	Vector3D cross(Vector3D v);

	double dot(Vector3D v);

	Vector3D ceil();

	Vector3D floor();

	Vector3D round();

	std::string toString();

};


Vector3D sumVector3Ds(std::vector<Vector3D> vector3DVector);

#endif // VECTOR3D_H_
