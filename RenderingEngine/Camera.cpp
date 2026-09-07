#include "Camera.h"

Camera::Camera() 
{
	update();
}

void Camera::update() {

	double sinP = sin(pitchTheta), cosP = cos(pitchTheta);
	double sinY = sin(yawTheta), cosY = cos(yawTheta);

	up = Vector3D(sinY * sinP, -cosP, cosY * sinP).unit();
	look = Vector3D(-sinY * cosP, -sinP, -cosY * cosP).unit();
	right = Vector3D(cosY, 0, -sinY);

}

Vector3D Camera::getProjected(Vector3D point) {

	Vector3D point2 = position - point;

	double x = right.dot(point2);
	double y = up.dot(point2);
	double z = look.dot(point2);
	
	Vector3D projected = Vector3D(x * 1600 / z, y * 1600 / z, z);

	return projected;

}
