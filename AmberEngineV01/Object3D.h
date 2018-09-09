#pragma once
#include "GlmHeader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/vector_query.hpp>

static float EPSILON = 0.001f; // 0.1mm error tolerance

static glm::vec3 vec3_zero = glm::vec3(1.0f, 1.0f, 0.0f);
static glm::vec3 vec3_right = glm::vec3(1.0f, 0.0f, 0.0f);
static glm::vec3 vec3_forward = glm::vec3(0.0f, 1.0f, 0.0f);
static glm::vec3 vec3_up = glm::vec3(0.0f, 0.0f, 1.0f);

//static glm::quat quat_ident = glm::quatLookAt(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // lookAt uses a different vector basis assumption
static glm::quat quat_ident = glm::quat_identity<glm::quat::value_type, glm::highp>(); // lookAt uses a different vector basis assumption

//static void getVulkanVector(glm::vec3* vkVec, glm::vec3& original) {
//	*vkVec = glm::vec3(original.x, original.z, -original.y);
//}

class Object3D
{
public:
	Object3D();
	Object3D(glm::vec3 position, glm::quat rotation);
	~Object3D();

	glm::vec3 pos;
	glm::quat rot;

	glm::vec3 forward();
	glm::vec3 up();
	glm::vec3 right();

	void setHeading(glm::vec3& position, glm::vec3& heading, glm::vec3& up);

	
};

