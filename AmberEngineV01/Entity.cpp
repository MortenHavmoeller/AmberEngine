#include "stdafx.h"
#include "Entity.h"

void Entity::init() {
	for (Component comp : components) {
		comp.init();
	}
}

void Entity::update() {
	for (Component comp : components) {
		comp.update();
	}
}

void Entity::heartbeat() {
	for (Component comp : components) {
		comp.heartbeat();
	}
}

void Entity::willRender() {
	for (Component comp : components) {
		comp.willRender();
	}
}

void Entity::end() {
	for (Component comp : components) {
		comp.end();
	}
}

void Entity::enable() {
	for (Component comp : components) {
		comp.enable();
	}
}

void Entity::disable() {
	for (Component comp : components) {
		comp.disable();
	}
}
