#pragma once
#include <iostream>
#include <thread> 
#include <memory>
#include <regex>
#include <mutex> 

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <windows.h>

#include "TimeManager.h"
#include "UIManager.h"
#include "Enums.h"
#include "PlayerProfile.h"
#include "GameplayManager.h"
#include "AudioPlayer.h"
#include "PlayerPrefs.h"

class Game {
private:
	//window variables
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::View view;
	sf::Event event;

	sf::Image icon;
	bool iconLoaded = false;

	//game manager pointers
	TimeManager* timeManager;
	UIManager* uiManager;
	GameplayManager* gameplayManager;


	AudioPlayer* audioPlayer;
	std::unordered_map<SoundId, sf::SoundBuffer> music;
	void LoadMusic(SoundId _name, std::string _path);

	//playerProfile pointer
	PlayerProfile* profile = nullptr;

	bool isWindowOpen;
	const int BASE_WIDTH = 480;
	const int BASE_HEIGHT = 270;

	std::string textEntered;

	//used for checking username validity
	std::regex nameCheck;
	//sets to true after first username fail
	bool regFailed = false;

	//set to true to show frame and time values
	bool debugDisplay = false;

	//tells render thread when to draw a frame
	int frameReady = 0;

	//thread variable to allow join in close window function
	std::unique_ptr<std::thread> rendererThreadPtr = nullptr;
	bool rendererThreadShutDown = false;
	//information that game is closed via a menu button
	bool gameClosed = false;

	void initializeVariables();
	void initializeObjects();

	void ApplyWindowPrefs();
	bool reloadingWindow = false;
	std::mutex windowMtx;

public:
	GameState _GAMESTATE;
	Game();
	~Game();
	
	void GameLoop();
	void Update();
	void RenderThread();
	void PollEvents();
};