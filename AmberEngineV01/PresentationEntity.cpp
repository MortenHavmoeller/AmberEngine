#include "stdafx.h"
#include "PresentationEntity.h"


void PresentationEntity::init() {
	for (Component comp : components) {
		comp.init();
	}
}

void PresentationEntity::update() {
	for (Component comp : components) {
		comp.update();
	}
}

void PresentationEntity::heartbeat() {
	for (Component comp : components) {
		comp.heartbeat();
	}
}

void PresentationEntity::willRender() {
	for (Component comp : components) {
		comp.willRender();
	}
}

void PresentationEntity::end() {
	for (Component comp : components) {
		comp.end();
	}
}

void PresentationEntity::enable() {
	for (Component comp : components) {
		comp.enable();
	}
}

void PresentationEntity::disable() {
	for (Component comp : components) {
		comp.disable();
	}
}
