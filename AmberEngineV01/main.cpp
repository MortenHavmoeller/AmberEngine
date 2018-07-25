#include "stdafx.h"
#include "Game.h"

int main() {
	Game game(APPLICATION_TYPE_SERVER_AND_CLIENT); // TODO: make this dynamic
	game.run();
}