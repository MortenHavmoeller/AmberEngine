#pragma once

#include "GlmHeader.h"
#include "Object3D.h"

class Camera : public Object3D
{
public:
	Camera();
	~Camera();

	float fov;
	float aspect;
	float nearFieldClip;
	float farPlaneClip;

	void setAim(glm::vec3& aimPoint, glm::vec3& up);
};

