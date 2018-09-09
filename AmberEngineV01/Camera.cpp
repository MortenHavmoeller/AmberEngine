#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{
}


Camera::~Camera()
{
}

void Camera::setAim(glm::vec3& aimPoint, glm::vec3& up) {
	setHeading(pos, aimPoint - pos, up);
}
