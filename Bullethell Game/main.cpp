#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <windows.h>

#include "Game.h"

int main() {
	std::cout << "Start\n";
	Game game;

	game.GameLoop();
	std::cout << "Exit\n";
}