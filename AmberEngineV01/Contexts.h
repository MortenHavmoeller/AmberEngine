#pragma once
#include "stdafx.h"
#include "Time.h"

class Game; // forward declaration of 'Game'

class GameContext {
public:
	GameContext(Game& gameRef, Time& timeRef) : game(gameRef), time(timeRef) {}
	~GameContext() {}

	Game& game;
	Time& time;
private:

};

class CommandPoolContext {
public:
	CommandPoolContext(RenderDevice* pDeviceArg, VkQueue& queueArg, VkCommandPool& commandPoolArg) :
		pDevice(pDeviceArg), queue(queueArg), commandPool(commandPoolArg) {}
	~CommandPoolContext() {}

	RenderDevice* pDevice;
	VkQueue& queue;
	VkCommandPool& commandPool;
};