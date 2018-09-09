#include "stdafx.h"
#include "Object3D.h"

Object3D::Object3D()
{
	pos = vec3_zero;
	rot = quat_ident;
}

Object3D::Object3D(glm::vec3 position, glm::quat rotation)
{
	pos = position;
	rot = rotation;
}


Object3D::~Object3D()
{
}

glm::vec3 Object3D::right() {
	return rot * vec3_right;
}

glm::vec3 Object3D::forward() {
	return rot * vec3_forward;
}

glm::vec3 Object3D::up() {
	return rot * vec3_up;
}

void Object3D::setHeading(glm::vec3& position, glm::vec3& heading, glm::vec3& up) {
	if (!glm::isNormalized(heading, EPSILON)) {
		rot = glm::quatLookAt(glm::normalize(heading), up);
		return;
	}

	rot = glm::quatLookAt(heading, up);
}
