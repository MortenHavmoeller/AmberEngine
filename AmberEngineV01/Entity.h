#pragma once
#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <list>

class Entity
{
public:
	Entity() { 
		enabled = true; 
	}

	Entity(glm::vec3 position) : Entity() {}

	~Entity() {}

	// functions
	void init();
	void update();
	void heartbeat();
	void willRender();
	void end();

	void enable();
	void disable();

	// basic variables
	bool enabled;
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 scale; // is this the best way?

	// components
	std::vector<Component> components;
};

