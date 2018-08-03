#pragma once

#include "stdafx.h"

#include "WindowView.h"
#include "RenderDevice.h"
#include "RenderPipeline.h"
#include "Time.h"
#include "World.h"
#include "AmbEnums.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <cstdlib>

class Game
{
public:
	Game(AmbAppType type) : ambAppType(type) {}
	~Game() {}

	AmbAppType ambAppType;

	int run();
};