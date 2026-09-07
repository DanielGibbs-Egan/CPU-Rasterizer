#include "Vector3D.h"

using namespace std;

void Vector3D::init(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::Vector3D()
{
	init(0, 0, 0);
};
Vector3D::Vector3D(double shared)
{
	init(shared, shared, shared);
};
Vector3D::Vector3D(double x, double y, double z)
{
	init(x, y, z);
};

Vector3D Vector3D::operator + (Vector3D v) {
	return Vector3D(x + v.x, y + v.y, z + v.z);
}

Vector3D Vector3D::operator - (Vector3D v) {
	return Vector3D(x - v.x, y - v.y, z - v.z);
}

Vector3D Vector3D::operator - () {
	return Vector3D(x, y, z) * -1;
}

void Vector3D::operator -= (Vector3D v) {
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
}

Vector3D Vector3D::operator * (double s) {
	return Vector3D(x * s, y * s, z * s);
}

Vector3D Vector3D::operator *= (double s) {
	x *= s, y *= s, z *= s;
	return *this;
}

Vector3D Vector3D::operator / (double s) {
	return Vector3D(x / s, y / s, z / s);
}

Vector3D Vector3D::operator /= (double s) {
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

double Vector3D::magnitude() {
	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Vector3D Vector3D::unit() {
	return operator / (magnitude());
}

Vector3D Vector3D::cross(Vector3D v) {

	double dx = (y * v.z) - (z * v.y), 
		   dy = (z * v.x) - (x * v.z), 
		   dz = (x * v.y) - (y * v.x);

	x = dx;
	y = dy;
	z = dz;

	return *this;
}

double Vector3D::dot(Vector3D v) {
	return (x * v.x) + (y * v.y) + (z * v.z);
}

Vector3D Vector3D::ceil() {
	x = std::ceil(x);
	y = std::ceil(y);
	z = std::ceil(z);
	return *this;
}

Vector3D Vector3D::floor() {
	x = std::floor(x);
	y = std::floor(y);
	z = std::floor(z);
	return *this;
}

Vector3D Vector3D::round() {
	x = std::round(x);
	y = std::round(y);
	z = std::round(z);
	return *this;
}

string Vector3D::toString() {
	string s = "Vector3D[" + to_string(x) + "," + to_string(y) + "," + to_string(z) + "]";
	return s;
}

Vector3D sumVector3Ds(vector<Vector3D> vector3DVector) {

	double x = 0;
	double y = 0;
	double z = 0;

	for (Vector3D& vector3D : vector3DVector) {
		x += vector3D.x;
		y += vector3D.y;
		z += vector3D.z;
	}

	return Vector3D(x, y, z);

}

