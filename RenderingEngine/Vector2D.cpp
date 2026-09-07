

#include "Vector2D.h"

using namespace std;

void Vector2D::init(double x, double y) {
	this->x = x;
	this->y = y;
}

Vector2D::Vector2D()
{
	init(0, 0);
};
Vector2D::Vector2D(double shared)
{
	init(shared, shared);
};
Vector2D::Vector2D(double x, double y)
{
	init(x, y);
};

Vector2D Vector2D::operator + (Vector2D v) {
	return Vector2D(x + v.x, y + v.y);
}

Vector2D Vector2D::operator - (Vector2D v) {
	return Vector2D(x - v.x, y - v.y);
}

Vector2D Vector2D::operator - () {
	return Vector2D(x, y) * -1;
}

void Vector2D::operator -= (Vector2D v) {
	this->x -= v.x;
	this->y -= v.y;
}

Vector2D Vector2D::operator * (double s) {
	return Vector2D(x * s, y * s);
}

Vector2D Vector2D::operator *= (double s) {
	x *= s, y *= s;
	return *this;
}

Vector2D Vector2D::operator / (double s) {
	return Vector2D(x / s, y / s);
}

Vector2D Vector2D::operator /= (double s) {
	x /= s;
	y /= s;
	return *this;
}

double Vector2D::magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2));
}

Vector2D Vector2D::unit() {
	return operator / (magnitude());
}

double Vector2D::dot(Vector2D v) {
	return (x * v.x) + (y * v.y);
}

Vector2D Vector2D::ceil() {
	x = std::ceil(x);
	y = std::ceil(y);
	return *this;
}

Vector2D Vector2D::floor() {
	x = std::floor(x);
	y = std::floor(y);
	return *this;
}

Vector2D Vector2D::round() {
	x = std::round(x);
	y = std::round(y);
	return *this;
}

string Vector2D::toString() {
	string s = "Vector2D[" + to_string(x) + "," + to_string(y) + "]";
	return s;
}

Vector2D sumVector2Ds(vector<Vector2D> vector2DVector) {

	double x = 0;
	double y = 0;

	for (Vector2D& vector2D : vector2DVector) {
		x += vector2D.x;
		y += vector2D.y;
	}

	return Vector2D(x, y);

}

