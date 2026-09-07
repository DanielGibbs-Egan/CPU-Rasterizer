#pragma once

#include <vector>
#include <string>
#include <cmath>

#ifndef VECTOR2D_H_   /* Include guard */
#define VECTOR2D_H_

class Vector2D {
private:

	void init(double x, double y);

public:

	double x;
	double y;

	Vector2D();
	Vector2D(double shared);
	Vector2D(double x, double y);

	Vector2D operator + (Vector2D v);

	Vector2D operator - (Vector2D v);

	Vector2D operator - ();

	void operator -= (Vector2D v);

	Vector2D operator * (double s);

	Vector2D operator *= (double s);

	Vector2D operator / (double s);

	Vector2D operator /= (double s);

	double magnitude();

	Vector2D unit();

	Vector2D cross(Vector2D v);

	double dot(Vector2D v);

	Vector2D ceil();

	Vector2D floor();

	Vector2D round();

	std::string toString();

};

Vector2D sumVector2Ds(std::vector<Vector2D> vector2DVector);

#endif // VECTOR2D_H_
