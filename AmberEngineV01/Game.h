#pragma once

#include "stdafx.h"

#include "WindowView.h"
#include "RenderDevice.h"
#include "RenderPipeline.h"
#include "Time.h"
#include "World.h"
#include "AmbEnums.h"

class Game
{
public:
	Game() { ambAppType = AmbAppType::APPLICATION_TYPE_UNDEFINED; }
	Game(AmbAppType ambAppTypeArg) : ambAppType(ambAppTypeArg) {}
	~Game() {}

	AmbAppType ambAppType;

	WindowView windowView;
	RenderDevice renderDevice;
	RenderPipeline renderPipeline;

	World world;
	Time time;

	int run();
};