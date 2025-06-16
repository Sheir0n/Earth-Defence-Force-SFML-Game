#include "Game.h"

//private initializers
void Game::initializeVariables() {
	this->_GAMESTATE = GameState::MainMenu;
	this->isWindowOpen = true;

	this->nameCheck = std::regex("^[A-Z0-9-_\\s]{3,20}$");

	LoadMusic(MenuSong, "Sounds\\menu_theme.ogg");
	LoadMusic(InGameSongLoop, "Sounds\\ingame_music.ogg");
}

void Game::initializeObjects() {
	//creates and applies window default settings
	if (icon.loadFromFile("Sprites\\Icon.png"))
		this->iconLoaded = true;

	this->ApplyWindowPrefs();

	this->timeManager = new TimeManager(PlayerPrefs::FRAMERATE);
	this->uiManager = new UIManager(this->window);
	this->gameplayManager = new GameplayManager(this->window);
	this->audioPlayer = new AudioPlayer();

	//loading songs
	this->audioPlayer->LoadSound(MenuSong, &(music.at(MenuSong)), 60, 1.f);
	this->audioPlayer->SetLoop(MenuSong, true);
	this->audioPlayer->LoadSound(InGameSongLoop, &(music.at(InGameSongLoop)), 100, 1.f);
	this->audioPlayer->SetLoop(InGameSongLoop, true);
}

void Game::LoadMusic(SoundId _name, std::string _path)
{
	sf::SoundBuffer _buffer;
	_buffer.loadFromFile(_path);

	this->music.insert(std::make_pair(_name, std::move(_buffer)));
}

void Game::ApplyWindowPrefs()
{
	this->reloadingWindow = true;

	windowMtx.lock();
	if (this->window != nullptr) {
		this->window->setActive(true);
		this->window->close();
	}

	this->videoMode = sf::VideoMode(BASE_WIDTH * (PlayerPrefs::RESOLUTION + 1), BASE_HEIGHT * (PlayerPrefs::RESOLUTION + 1));;

	switch (PlayerPrefs::WINDOW) {
	case PlayerPrefs::WindowTypes::Windowed:
		this->window = new sf::RenderWindow(videoMode, "Earth Defence Force 0-1", sf::Style::Close | sf::Style::Titlebar);
		break;
	case PlayerPrefs::WindowTypes::Fullscreen:
		this->window = new sf::RenderWindow(videoMode, "Earth Defence Force 0-1", sf::Style::Fullscreen);
		break;
	case PlayerPrefs::WindowTypes::Borderless:
		this->window = new sf::RenderWindow(videoMode, "Earth Defence Force 0-1", sf::Style::None);
		break;
	default:
		this->window = new sf::RenderWindow(videoMode, "Earth Defence Force 0-1", sf::Style::Close | sf::Style::Titlebar);
		break;
	}
	this->window->setMouseCursorVisible(false);

	this->window->setVerticalSyncEnabled(PlayerPrefs::VSYNC);

	this->view = sf::View(sf::FloatRect(0, 0, 480.f, 270.f));
	this->window->setView(view);

	if (this->iconLoaded)
		this->window->setIcon(64, 64, icon.getPixelsPtr());

	this->window->setActive(false);
	this->reloadingWindow = false;
	this->windowMtx.unlock();
}

//public
//constructor / destructor
Game::Game() {
	this->debugDisplay = false;
	this->initializeVariables();
	this->initializeObjects();
}

Game::~Game() {
	delete this->window;
	delete this->uiManager;
	delete this->timeManager;
	delete this->gameplayManager;
	delete this->audioPlayer;
}

//Game loop functions
void Game::GameLoop()
{
	//launch graphics renderer thread as a pointer
	this->window->setActive(false);
	this->rendererThreadPtr = std::unique_ptr<std::thread>(new std::thread([this] {this->RenderThread(); }));

	this->uiManager->InitializeMenu(GameState::MainMenu);

	this->audioPlayer->PlaySoundId(MenuSong, PlayerPrefs::MENUMUSIC * 0.1f);

	while (this->window->isOpen()) {
		if (frameReady > 0 || !this->timeManager->PushFrame())
			continue;

		if (debugDisplay)
			std::cout << "Fps: " << timeManager->GetFps() << " Delta: " << timeManager->GetDeltaTime() << " Total: " << timeManager->GetTotalTime() << std::endl;

		Update();

		//tells render thread to draw frame
		this->frameReady++;
	}
}

//graphics thread
void Game::RenderThread()
{
	this->window->setActive(true);
	windowMtx.lock();

	while (!rendererThreadShutDown && this->window != nullptr && this->window->isOpen()) {

		if (this->reloadingWindow) {
			if (this->window != nullptr) {
				this->window->setActive(false);

				this->windowMtx.unlock();
				while (reloadingWindow) { Sleep(10); };
				this->window->setActive(true);
				this->windowMtx.lock();
			}
		}
		if (frameReady == 0)
			continue;
		frameReady--;

		this->window->clear(sf::Color(0, 153, 219));

		//draw background and clouds
		// 
		//draw in game objects
		this->gameplayManager->DrawBackgroundObjects(this->window);

		if (_GAMESTATE == GameScreen || _GAMESTATE == GameOverScreen) {
			this->gameplayManager->DrawGameObjects(this->window);
		}
		this->uiManager->DrawUI();
		this->window->display();
	}
	this->window->setActive(false);
	this->windowMtx.unlock();
}

//logic update
void Game::Update()
{
	this->uiManager->UpdateCursor(_GAMESTATE);
	this->uiManager->MenuMouseHover(_GAMESTATE);
	this->PollEvents();
	float _deltaTime = this->timeManager->GetDeltaTime();

	this->gameplayManager->HandleBackgroundLogic(_deltaTime, this->uiManager);

	if (_GAMESTATE == GameScreen) {
		this->gameplayManager->HandleGameLogic(_deltaTime, this->uiManager);

		GameplayManager::GameplayState _state = this->gameplayManager->GetState();
		if (_state == GameplayManager::PlDead)
			this->audioPlayer->StopSound(InGameSongLoop);
		else if (_state == GameplayManager::ShowReturn) {
			this->uiManager->Uninitialize();

			this->profile->UpdateData(this->gameplayManager->GetGameOverStats());
			this->_GAMESTATE = GameOverScreen;
			this->uiManager->InitializeMenu(GameOverScreen);
			this->uiManager->UpdateGameOverText(this->gameplayManager->GetGameOverStats());
		}
	}

	this->gameplayManager->UpdateAnimator(_deltaTime);
}

//event management
void Game::PollEvents()
{
	//window events
	while (this->window->pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			//had to do this becouse renderer thread wouldn't close
			rendererThreadShutDown = true;
			rendererThreadPtr.get()->join();
			this->window->close();
			if (debugDisplay)
				std::cout << "zamkniete okno";
			break;

			// mouse button events
		case sf::Event::MouseButtonPressed:

			//handle Main Menu Events
			switch (_GAMESTATE) {
			case (GameState::MainMenu):

				switch (uiManager->CheckUIButtonPress(GameState::MainMenu)) {
				case this->uiManager->UiEvents::Play:
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::LoginScreen);
					this->textEntered = "";
					this->uiManager->EditText(1, textEntered, true);
					this->regFailed = false;
					this->_GAMESTATE = GameState::LoginScreen;
					break;

				case this->uiManager->UiEvents::Info:
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::InfoMenu);
					this->_GAMESTATE = GameState::InfoMenu;
					break;

				case this->uiManager->UiEvents::Settings:
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::SettingsMenu);
					this->_GAMESTATE = GameState::SettingsMenu;
					break;

				case this->uiManager->UiEvents::Credits:
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::CreditsMenu);
					this->_GAMESTATE = GameState::CreditsMenu;
					break;

				case this->uiManager->UiEvents::Exit:
					this->gameClosed = true;
					break;
				}
				break;

				//handle Info Menu Events
			case (GameState::InfoMenu):

				if (this->uiManager->CheckUIButtonPress(GameState::InfoMenu) == uiManager->UiEvents::ReturnToMain) {
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::MainMenu);
					this->_GAMESTATE = GameState::MainMenu;
				}
				break;

				//handle Credits Menu Events
			case (GameState::CreditsMenu):

				if (this->uiManager->CheckUIButtonPress(GameState::CreditsMenu) == uiManager->UiEvents::ReturnToMain) {
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::MainMenu);
					this->_GAMESTATE = GameState::MainMenu;
				}
				break;

				//handle Settings Menu Events
			case (GameState::SettingsMenu): {
				UIManager::UiEvents _currEvent = this->uiManager->CheckUIButtonPress(GameState::SettingsMenu);

				if (_currEvent == uiManager->UiEvents::ReturnToMain) {
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::MainMenu);
					this->_GAMESTATE = GameState::MainMenu;
				}
				else if (_currEvent == uiManager->UiEvents::ReloadWindow) {
					this->uiManager->Uninitialize();
					this->ApplyWindowPrefs();
					this->uiManager->UpdateWindowPtr(this->window);
					this->gameplayManager->UpdateWindowPtr(this->window);
					this->uiManager->InitializeMenu(GameState::SettingsMenu);
				}
				else if (_currEvent == uiManager->UiEvents::SettingsDefault) {
					this->timeManager->SetFps(PlayerPrefs::FRAMERATE);
					this->audioPlayer->SetVolume(MenuSong, 60 * PlayerPrefs::MENUMUSIC * 0.1f);
					this->uiManager->Uninitialize();
					this->ApplyWindowPrefs();
					this->uiManager->UpdateWindowPtr(this->window);
					this->gameplayManager->UpdateWindowPtr(this->window);
					this->uiManager->InitializeMenu(GameState::SettingsMenu);
				}
				else if (_currEvent == uiManager->UiEvents::ChangeFramerate) {
					this->timeManager->SetFps(PlayerPrefs::FRAMERATE);
				}
				else if (_currEvent == uiManager->UiEvents::ChangeMenuThemeVolume) {
					this->audioPlayer->SetVolume(MenuSong, 60 * PlayerPrefs::MENUMUSIC * 0.1f);
				}
				break;
			}

			case (GameState::LoginScreen):
				//handle Login Screen Events
				switch (this->uiManager->CheckUIButtonPress(GameState::LoginScreen)) {
				case(this->uiManager->UiEvents::ReturnToMain):
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::MainMenu);
					this->_GAMESTATE = GameState::MainMenu;
					break;

				case(this->uiManager->UiEvents::Sign):
					if (std::regex_match(textEntered, nameCheck)) {
						this->uiManager->ChangeTextVisibility(0, false);
						this->regFailed = false;

						//log into profile
						this->profile = new PlayerProfile(textEntered);
						this->uiManager->Uninitialize();
						this->uiManager->InitializeMenu(GameState::ProfileScreen);

						this->profile->WriteStatsToUI(this->uiManager);

						this->_GAMESTATE = GameState::ProfileScreen;
					}
					else {
						this->uiManager->ChangeTextVisibility(0, true);
					}

					break;

				}
				break;
			case (GameState::ProfileScreen):
				switch (this->uiManager->CheckUIButtonPress(GameState::ProfileScreen)) {
				case(this->uiManager->UiEvents::ProfilePlay):

					//START GAME HERE
					std::cout << "Gra zaczyna sie \n";

					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::GameScreen);
					this->uiManager->MakeBotGameUiTransparent(false);
					this->uiManager->MakeTopGameUiTransparent(false);

					this->gameplayManager->NewGame(this->uiManager, this->profile->GetEquippedWeapons());

					this->audioPlayer->StopSound(MenuSong);
					this->audioPlayer->PlaySoundId(InGameSongLoop, PlayerPrefs::INGAMEMUSIC * 0.1f);

					this->_GAMESTATE = GameState::GameScreen;

					break;

				case(this->uiManager->UiEvents::ProfileLogout):
					this->textEntered = this->profile->GetName();
					delete this->profile;
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::LoginScreen);
					this->uiManager->EditText(1, textEntered, true);
					this->_GAMESTATE = GameState::LoginScreen;
					break;

				case(this->uiManager->UiEvents::Exit):
					delete this->profile;
					this->gameClosed = true;
					break;

					//buy/equip weapons
				case(this->uiManager->UiEvents::Equip1):
					this->profile->EquipWeapon(0, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip2):
					this->profile->EquipWeapon(1, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip3):
					this->profile->EquipWeapon(2, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip4):
					this->profile->EquipWeapon(3, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip5):
					this->profile->EquipWeapon(4, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip6):
					this->profile->EquipWeapon(5, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip7):
					this->profile->EquipWeapon(6, this->uiManager);
					break;
				case(this->uiManager->UiEvents::Equip8):
					this->profile->EquipWeapon(7, this->uiManager);
					break;
				}
				break;

				//GAME MOUSE INPUTS
			case (GameState::GameScreen):
				if (event.mouseButton.button == sf::Mouse::Left)
					this->gameplayManager->PassPlayerInputEvent(GameplayManager::ShootLeft);
				if (event.mouseButton.button == sf::Mouse::Right)
					this->gameplayManager->PassPlayerInputEvent(GameplayManager::ShootRight);
				break;

			case (GameState::GameOverScreen):
				if (this->uiManager->CheckUIButtonPress(GameState::GameOverScreen) == uiManager->UiEvents::Sign) {
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::ProfileScreen);
					this->profile->WriteStatsToUI(this->uiManager);
					this->audioPlayer->PlaySoundId(MenuSong, PlayerPrefs::MENUMUSIC * 0.1f);
					this->_GAMESTATE = GameState::ProfileScreen;
				}
				break;
			}
			break;


			//handle Write Text Events
		case sf::Event::TextEntered:
			if (_GAMESTATE == GameState::LoginScreen) {

				bool _callIfChanged = false;

				if (event.text.unicode == '\b' && this->textEntered.size() >= 1) {
					this->textEntered.pop_back();
					_callIfChanged = true;
				}
				else if (event.text.unicode == ' ' && this->textEntered.size() == 0)
					continue;
				else if (event.text.unicode < 128 && event.text.unicode != '\b' && event.text.unicode != '\t' && event.text.unicode != 13 && event.text.unicode != '\n' && this->textEntered.size() < 12) {
					this->textEntered += std::toupper(event.text.unicode);
					_callIfChanged = true;
				}
				// login on enter press and corect regex match
				else if (event.text.unicode == 13 && std::regex_match(textEntered, nameCheck)) {
					this->uiManager->ChangeTextVisibility(0, false);
					this->regFailed = false;

					//log into profile
					this->profile = new PlayerProfile(textEntered);
					this->uiManager->Uninitialize();
					this->uiManager->InitializeMenu(GameState::ProfileScreen);

					this->profile->WriteStatsToUI(this->uiManager);

					this->_GAMESTATE = GameState::ProfileScreen;
				}

				//executes if text changed
				if (_callIfChanged) {
					//removes double whitespaces
					std::regex_replace(textEntered, std::regex("(  )"), " ");
					this->uiManager->EditText(1, textEntered, true);

					//shows and updates text after first regex fail
					if (std::regex_match(textEntered, nameCheck)) {
						this->uiManager->ChangeTextVisibility(0, false);
					}
					else if (regFailed || textEntered.size() > 0) {
						regFailed = true;
						this->uiManager->ChangeTextVisibility(0, true);
					}
				}

			}
			break;


			//Universal Key Events
		case (sf::Event::KeyPressed):
			// turn debug on/off
			if (event.key.code == sf::Keyboard::LControl) {
				this->debugDisplay = !this->debugDisplay;

				std::cout << "FPS DISPLAY - ";
				if (debugDisplay)
					std::cout << "ON" << std::endl;
				else
					std::cout << "OFF" << std::endl;
			}



			//Gameplay Keyboard Input Events
			if (_GAMESTATE == GameState::GameScreen) {
				if (event.key.code == sf::Keyboard::Space)
					this->gameplayManager->PassPlayerInputEvent(GameplayManager::Dodge);
				if (event.key.code == sf::Keyboard::P) {
					this->gameplayManager->Pause();
					this->uiManager->ShowPauseOverlay(this->gameplayManager->IsGamePaused());
				}
			}
		}
	}

	//close if exit button pressed
	if (this->gameClosed) {
		rendererThreadShutDown = true;
		rendererThreadPtr.get()->join();
		this->window->close();
		if (debugDisplay)
			std::cout << "zamkniete okno";
	}
}

