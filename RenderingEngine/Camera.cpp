#include <iostream>

#include <cmath>
#include "Vector3D.h"

class Camera {
public:

	double pitchTheta = 0;
	double yawTheta = 0;

	Vector3D position = Vector3D();

	Vector3D up, look, right;

	Camera() {
		update();
	}

	void update() {

		double sinP = sin(pitchTheta), cosP = cos(pitchTheta);
		double sinY = sin(yawTheta), cosY = cos(yawTheta);

		up = Vector3D(cosY * (-sinP), cosP, sinY * (-sinP)).unit();
		look = Vector3D(cosY * cosP, sinP, sinY * cosP).unit();
		right = Vector3D(sinY, 0, -cosY);

	}

	Vector3D getProjected(Vector3D point) {

		Vector3D point2 = -point - position;

		double x = right.dot(point2);
		double y = up.dot(point2);
		double z = look.dot(point2);

		Vector3D projected = Vector3D(x * 1600 / z, y * 1600 / z, z);

		return projected;

	}

	/*CFrame operator + (CFrame cframe)
	{
		return CFrame(pitchTheta + cframe.pitchTheta, yawTheta + cframe.yawTheta, position + cframe.position);
	}*/
};
