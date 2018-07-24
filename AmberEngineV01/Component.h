#pragma once
class Component
{
public:
	Component();
	~Component();

	void init();
	void update();
	void heartbeat();
	void willRender();
	void end();

	void enable();
	void disable();
};

