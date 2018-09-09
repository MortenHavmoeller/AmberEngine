#pragma once

#include "stdafx.h"

#include "InputManager.h"
#include "WindowView.h"
#include "RenderDevice.h"
#include "RenderPipeline.h"
#include "Time.h"
#include "World.h"
#include "AmbEnums.h"
#include "Object3D.h"

class Game
{
public:
	Game() { ambAppType = AmbAppType::APPLICATION_TYPE_UNDEFINED; }
	Game(AmbAppType ambAppTypeArg) : ambAppType(ambAppTypeArg) {}
	~Game() {}

	AmbAppType ambAppType;

	InputManager inputManager;

	WindowView windowView;
	RenderDevice renderDevice;
	RenderPipeline renderPipeline;

	World world;
	Time time;

	int run();

private:
	bool keepRunning;
};