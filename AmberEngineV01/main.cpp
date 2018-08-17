#include "stdafx.h"
#include "Game.h"

int main() {
	Game game(AmbAppType::APPLICATION_TYPE_SERVER_AND_CLIENT);
	game.run();
}