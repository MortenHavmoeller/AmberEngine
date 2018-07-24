#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <list>


class PresentationEntity
{
public:
	PresentationEntity() {}
	~PresentationEntity() {}

	// functions
	void init();
	void update();
	void heartbeat();
	void willRender();
	void end();

	void enable();
	void disable();

	// basic variables - already covered by Entity
	//bool enabled;
	//glm::vec3 pos;
	//glm::quat rot;
	//glm::vec3 scale; // is this the best way?

	// components
	std::vector<Component> components;
};

