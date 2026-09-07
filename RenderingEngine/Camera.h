#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include "Vector3D.h"
#include <iostream>
#include <float.h>

#ifndef CAMERA_H_   /* Include guard */
#define CAMERA_H_

class Camera {
public:

	double pitchTheta = 0;
	double yawTheta = 0;

	Vector3D position = Vector3D();

	Vector3D up, look, right;

	Camera();

	void update();

	Vector3D getProjected(Vector3D point);

	/*CFrame operator + (CFrame cframe)
	{
		return CFrame(pitchTheta + cframe.pitchTheta, yawTheta + cframe.yawTheta, position + cframe.position);
	}*/
};


#endif // VECTOR2D_H_