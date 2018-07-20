#pragma once

#include "stdafx.h"
#include "WindowView.h"
#include "RenderPipeline.h"
#include "Time.h"

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
	Game();
	~Game();

	int run();
};

