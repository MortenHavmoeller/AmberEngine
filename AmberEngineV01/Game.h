#pragma once

#include "stdafx.h"
#include "WindowView.h"
#include "RenderPipeline.h"
#include "Time.h"
#include "World.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <cstdlib>

enum AmbAppType {
	APPLICATION_TYPE_UNDEFINED = 0,
	APPLICATION_TYPE_CLIENT = 1, 
	APPLICATION_TYPE_SERVER = 2, 
	APPLICATION_TYPE_SERVER_AND_CLIENT = 3 // 1 + 2
};

class Game
{
public:
	Game(AmbAppType type) : applicationType(type) {}
	~Game() {}

	AmbAppType applicationType;

	int run();
};