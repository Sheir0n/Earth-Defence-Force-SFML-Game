#include "UIManager.h"
#include <iostream>
#include <filesystem>

UIManager::UIManager(sf::RenderWindow* _window) {
	this->window = _window;

	//load font
	this->textFont.loadFromFile("Fonts\\Quinquefive-ALoRM.ttf");

	//load textures
	//main menu
	LoadTexture(MainBanner, "Sprites\\Menu_Banner.png");
	LoadTexture(MainPlayButton, "Sprites\\Menu_Play_Button.png");
	LoadTexture(MainInfoButton, "Sprites\\Menu_Info_Button.png");
	LoadTexture(MainSettingsButton, "Sprites\\Menu_Settings_Button.png");
	LoadTexture(MainCreditsButton, "Sprites\\Menu_Credits_Button.png");
	LoadTexture(MainExitButton, "Sprites\\Menu_Exit_Button.png");

	//info menu
	LoadTexture(InfoBanner, "Sprites\\Menu_Info_Main_Window.png");
	LoadTexture(InfoReturn, "Sprites\\Menu_Button_Return.png");

	//credits menu
	LoadTexture(CreditsBanner, "Sprites\\Menu_Credits_Main_Window.png");

	//settings screen
	LoadTexture(SettingsWindow, "Sprites\\Menu_Settings_Window.png");
	LoadTexture(ResetButton, "Sprites\\Menu_Reset_Button.png");
	LoadTexture(SliderDec, "Sprites\\Settings_Reduce_Slider.png");
	LoadTexture(SliderInc, "Sprites\\Settings_Increase_Slider.png");
	LoadTexture(Slider, "Sprites\\Settings_Slider.png");
	LoadTexture(ButResolution1, "Sprites\\Resolution1.png");
	LoadTexture(ButResolution2, "Sprites\\Resolution2.png");
	LoadTexture(ButResolution3, "Sprites\\Resolution3.png");
	LoadTexture(ButResolution4, "Sprites\\Resolution4.png");
	LoadTexture(ButFps30, "Sprites\\Fps30.png");
	LoadTexture(ButFps60, "Sprites\\Fps60.png");
	LoadTexture(ButFps75, "Sprites\\Fps75.png");
	LoadTexture(ButFps144, "Sprites\\Fps144.png");
	LoadTexture(ButFpsUnlimited, "Sprites\\FpsUnlimited.png");
	LoadTexture(ButVsyncOff, "Sprites\\VsyncOff.png");
	LoadTexture(ButVsyncOn, "Sprites\\VsyncOn.png");
	LoadTexture(ButWindowed, "Sprites\\ScreenWindowed.png");
	LoadTexture(ButFullscreen, "Sprites\\ScreenFullscreen.png");
	LoadTexture(ButBorderless, "Sprites\\ScreenBorderless.png");

	//login window
	LoadTexture(LoginWindow, "Sprites\\Menu_Login_Main_Window.png");
	LoadTexture(LoginReturn, "Sprites\\Menu_Button_Return.png");
	LoadTexture(SignIn, "Sprites\\Menu_Sign_Button.png");

	//profile screen
	LoadTexture(ProfileLogo, "Sprites\\Player_Profile_Logo.png");
	LoadTexture(ProfileStats, "Sprites\\Player_Profile_Stats.png");
	LoadTexture(ProfileParts, "Sprites\\Player_Parts_Menu_Background.png");
	LoadTexture(ProfilePlayButton, "Sprites\\Player_Profile_Play_Button.png");
	LoadTexture(ProfileLogoutButton, "Sprites\\Player_Profile_Logout_Button.png");
	LoadTexture(ProfileExitButton, "Sprites\\Player_Profile_Exit_Button.png");
	LoadTexture(ProfileWeaponButtons, "Sprites\\Profile_Weapons_Buttons.png");
	LoadTexture(Guns, "Sprites\\Guns.png");
	LoadTexture(ProfileExtras, "Sprites\\Profile_Weapon_Extras.png");
	LoadTexture(WeaponBar, "Sprites\\Profile_Weapon_Statbar.png");

	//game ui
	LoadTexture(GameUIBg, "Sprites\\Ui_Bottom_Background.png");
	LoadTexture(GameUIShip, "Sprites\\Ui_Bottom_Ship.png");
	LoadTexture(GameUIWarning, "Sprites\\Ui_Warning.png");
	LoadTexture(GameUITopStats, "Sprites\\Ui_Top.png");

	//weapons
	LoadTexture(Guns, "Sprites\\Guns.png");
	LoadTexture(WeaponBlaster, "Sprites\\Weapon_Blaster.png");

	//mouse
	LoadTexture(Cursor, "Sprites\\Crosshair.png");

	cursor = new Crosshair(&textures.at(Cursor));

	//game over screen
	LoadTexture(GameOver, "Sprites\\Game_Over_Background.png");


	this->pauseGray.setPosition(0, 0);
	this->pauseGray.setFillColor(sf::Color(165, 151, 161, 64));
	this->pauseGray.setSize(sf::Vector2f(480, 270));
}

void UIManager::LoadTexture(TextureId _name, std::string _path) {
	sf::Texture _texture;
	_texture.loadFromFile(_path);
	this->textures.insert(std::make_pair(_name, std::move(_texture)));
}

UIManager::~UIManager() {
	this->isInitialized = false;
	this->window = nullptr;

	this->Uninitialize();
}

void UIManager::UpdateWindowPtr(sf::RenderWindow* _window)
{
	this->window = _window;
}


//static ui element initializer
void UIManager::InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position)
{
	UIObject* _currButtonPtr = new UIObject(_width, _height, _position);
	_currButtonPtr->SetTexture(&textures.at(_txtId), sf::Vector2<int>(_width, _height));
	this->objects.push_back(std::move(_currButtonPtr));
}

void UIManager::InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position, int _frame, bool _flipped)
{
	UIObject* _currButtonPtr = new UIObject(_width, _height, _position);
	_currButtonPtr->SetTexture(&textures.at(_txtId), sf::Vector2<int>(_width, _height));
	_currButtonPtr->SetFrame(_frame);
	_currButtonPtr->Flip(_flipped);
	this->objects.push_back(std::move(_currButtonPtr));
}

//non static ui element initializer
void UIManager::InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position, bool _isClickable)
{
	UIObject* _currButtonPtr = new UIObject(_width, _height, _position);
	_currButtonPtr->SetTexture(&textures.at(_txtId), sf::Vector2<int>(_width, _height));
	_currButtonPtr->SetClickable(true);
	this->objects.push_back(std::move(_currButtonPtr));
}

void UIManager::InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color)
{
	UIText* _currTextPtr = new UIText(_text, 0, 0, _position, &textFont, _size, _color);
	this->textObjects.push_back(std::move(_currTextPtr));
}

void UIManager::InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color, bool _isVisible)
{
	UIText* _currTextPtr = new UIText(_text, 0, 0, _position, &textFont, _size, _color, _isVisible);
	this->textObjects.push_back(std::move(_currTextPtr));
}

void UIManager::InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color, bool _isVisible, bool _isCentered)
{
	UIText* _currTextPtr = new UIText(_text, 0, 0, _position, &textFont, _size, _color, _isVisible);
	if (_isCentered)
		_currTextPtr->SetCentered();
	this->textObjects.push_back(std::move(_currTextPtr));
}

//checks if ui already initialized, if not - destroys existing objects and creates new ones
void UIManager::InitializeMenu(GameState _GAMESTATE)
{
	if (isInitialized)
		return;

	switch (_GAMESTATE) {
	case (GameState::MainMenu):
		InitializeUIObject(MainBanner, 400, 77, sf::Vector2i(15, 10));
		InitializeUIObject(MainPlayButton, 192, 36, sf::Vector2i(15, 96), true);
		InitializeUIObject(MainInfoButton, 192, 36, sf::Vector2i(15, 140), true);
		InitializeUIObject(MainSettingsButton, 192, 36, sf::Vector2i(15, 184), true);
		InitializeUIObject(MainCreditsButton, 192, 36, sf::Vector2i(15, 228), true);
		InitializeUIObject(MainExitButton, 145, 36, sf::Vector2i(320, 228), true);
		break;

	case(GameState::InfoMenu):
		InitializeUIObject(InfoBanner, 460, 202, sf::Vector2i(10, 10));
		InitializeUIObject(InfoReturn, 154, 36, sf::Vector2i(163, 212), true);
		break;
	case(GameState::CreditsMenu):
		InitializeUIObject(CreditsBanner, 460, 202, sf::Vector2i(10, 10));
		InitializeUIObject(InfoReturn, 154, 36, sf::Vector2i(163, 212), true);

		InitializeUIText("Jakub Haberek", sf::Vector2i(240, 40), 5, sf::Color(255, 255, 255), true, true);
		InitializeUIText("github.com/Sheir0n", sf::Vector2i(240, 50), 5, sf::Color(255, 255, 255), true, true);


		InitializeUIText("melissapons - freesound.org/s/171588", sf::Vector2i(140, 105), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Speedenza - freesound.org/people/Speedenza/packs/10440/", sf::Vector2i(340, 105), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("WhatChar - freesound.org/s/725542", sf::Vector2i(140, 110), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Tazy - freesound.org/s/369935", sf::Vector2i(340, 110), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("StormwaveAudio - freesound.org/s/330629", sf::Vector2i(140, 115), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("JustInvoke - freesound.org/s/446125", sf::Vector2i(340, 115), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("DrMrSir - freesound.org/s/529558", sf::Vector2i(140, 120), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("211redman112 - freesound.org/s/234083", sf::Vector2i(340, 120), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("sonicboom_sfx - freesound.org/s/223785", sf::Vector2i(140, 125), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("madamdata - freesound.org/s/479363", sf::Vector2i(340, 125), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Hybrid_V - freesound.org/s/321215", sf::Vector2i(140, 130), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Wax_vibe - freesound.org/s/546625", sf::Vector2i(340, 130), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Debsound - freesound.org/s/725109", sf::Vector2i(140, 135), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("MiceHead - freesound.org/s/13294", sf::Vector2i(340, 135), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("Aegersum - freesound.org/s/387924", sf::Vector2i(140, 140), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("dpren - freesound.org/s/440145", sf::Vector2i(340, 140), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("smcameron - freesound.org/s/51468", sf::Vector2i(140, 145), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("sonically_sound - freesound.org/s/612898", sf::Vector2i(340, 145), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("qubodup - freesound.org/s/442827", sf::Vector2i(140, 150), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("alexo400 - freesound.org/s/543653", sf::Vector2i(340, 150), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("hardwareshaba - freesound.org/s/577875", sf::Vector2i(140, 155), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("humanoide9000  - freesound.org/s/422440", sf::Vector2i(340, 155), 3, sf::Color(192, 203, 220), true, true);
		InitializeUIText("C3Sabertooth  - freesound.org/s/480870", sf::Vector2i(140, 160), 3, sf::Color(192, 203, 220), true, true);
		break;
	case(GameState::SettingsMenu):
		InitializeUIObject(SettingsWindow, 460, 202, sf::Vector2i(10, 10));
		InitializeUIObject(InfoReturn, 154, 36, sf::Vector2i(10, 217), true);
		InitializeUIObject(ResetButton, 244, 36, sf::Vector2i(205, 16), true);

		//menu music
		InitializeUIObject(SliderDec, 13, 9, sf::Vector2i(234, 74), true);
		InitializeUIObject(SliderInc, 13, 9, sf::Vector2i(388, 74), true);
		InitializeUIObject(Slider, 139, 9, sf::Vector2i(248, 74));
		//in game music
		InitializeUIObject(SliderDec, 13, 9, sf::Vector2i(234, 88), true);
		InitializeUIObject(SliderInc, 13, 9, sf::Vector2i(388, 88), true);
		InitializeUIObject(Slider, 139, 9, sf::Vector2i(248, 88));
		//effects
		InitializeUIObject(SliderDec, 13, 9, sf::Vector2i(234, 102), true);
		InitializeUIObject(SliderInc, 13, 9, sf::Vector2i(388, 102), true);
		InitializeUIObject(Slider, 139, 9, sf::Vector2i(248, 102));
		//resolution
		InitializeUIObject(ButResolution1, 62, 11, sf::Vector2i(187, 146), true);
		InitializeUIObject(ButResolution2, 62, 11, sf::Vector2i(254, 146), true);
		InitializeUIObject(ButResolution3, 62, 11, sf::Vector2i(321, 146), true);
		InitializeUIObject(ButResolution4, 62, 11, sf::Vector2i(388, 146), true);
		//fps
		InitializeUIObject(ButFps30, 30, 11, sf::Vector2i(187, 160), true);
		InitializeUIObject(ButFps60, 30, 11, sf::Vector2i(223, 160), true);
		InitializeUIObject(ButFps75, 30, 11, sf::Vector2i(259, 160), true);
		InitializeUIObject(ButFps144, 30, 11, sf::Vector2i(295, 160), true);
		InitializeUIObject(ButFpsUnlimited, 61, 11, sf::Vector2i(331, 160), true);
		//vsync
		InitializeUIObject(ButVsyncOff, 30, 11, sf::Vector2i(187, 174), true);
		InitializeUIObject(ButVsyncOn, 30, 11, sf::Vector2i(223, 174), true);
		//window
		InitializeUIObject(ButWindowed, 55, 11, sf::Vector2i(187, 188), true);
		InitializeUIObject(ButFullscreen, 67, 11, sf::Vector2i(247, 188), true);
		InitializeUIObject(ButBorderless, 67, 11, sf::Vector2i(318, 188), true);

		//update the frame for already selected options
		//resolution
		this->objects[12 + PlayerPrefs::RESOLUTION]->SetFrame(2);
		this->objects[12 + PlayerPrefs::RESOLUTION]->SetClickable(false);
		//vsync
		this->objects[21 + PlayerPrefs::VSYNC]->SetFrame(2);
		this->objects[21 + PlayerPrefs::VSYNC]->SetClickable(false);
		//window
		this->objects[23 + PlayerPrefs::WINDOW]->SetFrame(2);
		this->objects[23 + PlayerPrefs::WINDOW]->SetClickable(false);
		//fps
		this->FixFpsMenuButtons();

		//update sound sliders
		this->FixSoundMenuSliders();

		break;
	case(GameState::LoginScreen): {
		InitializeUIObject(LoginWindow, 354, 202, sf::Vector2i(63, 10));
		InitializeUIObject(LoginReturn, 154, 36, sf::Vector2i(163, 212), true);
		InitializeUIObject(SignIn, 93, 35, sf::Vector2i(278, 140), true);

		std::string tempString = "Must contain between 3-12 letters, numbers, or spaces!";
		InitializeUIText(tempString, sf::Vector2i(83, 187), 5, sf::Color(255, 0, 68), false);
		InitializeUIText("", sf::Vector2i(183, 155), 5, sf::Color(255, 255, 255));
		break;
	}
	case(GameState::ProfileScreen):
		InitializeUIObject(ProfileLogo, 281, 60, sf::Vector2i(182, 18));
		InitializeUIObject(ProfileStats, 162, 98, sf::Vector2i(15, 158));
		InitializeUIObject(ProfileParts, 256, 162, sf::Vector2i(207, 94));
		InitializeUIObject(ProfilePlayButton, 135, 36, sf::Vector2i(15, 15), true);
		InitializeUIObject(ProfileLogoutButton, 135, 36, sf::Vector2i(15, 63), true);
		InitializeUIObject(ProfileExitButton, 135, 36, sf::Vector2i(15, 111), true);

		//guns
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(238, 175), 1, false);
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(387, 175), 1, true);

		InitializeUIObject(Guns, 46, 11, sf::Vector2i(238, 192), 2, false);
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(387, 192), 2, true);

		InitializeUIObject(Guns, 46, 11, sf::Vector2i(238, 209), 3, false);
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(387, 209), 3, true);

		InitializeUIObject(Guns, 46, 11, sf::Vector2i(238, 226), 4, false);
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(387, 226), 4, true);

		//guns buttons
		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(225, 173), true);
		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(374, 173), true);

		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(225, 190), true);
		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(374, 190), true);

		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(225, 207), true);
		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(374, 207), true);

		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(225, 224), true);
		InitializeUIObject(ProfileWeaponButtons, 73, 15, sf::Vector2i(374, 224), true);


		//Stats
		InitializeUIText("", sf::Vector2i(89, 194), 5, sf::Color(255, 0, 68));
		InitializeUIText("", sf::Vector2i(89, 212), 5, sf::Color(255, 0, 68));
		InitializeUIText("", sf::Vector2i(89, 230), 5, sf::Color(255, 0, 68));

		//Rating
		InitializeUIText("S", sf::Vector2i(144, 169), 5, sf::Color(255, 0, 68));

		//Balance
		InitializeUIText("", sf::Vector2i(300, 104), 5, sf::Color(254, 174, 52));

		//Username
		InitializeUIText("User:", sf::Vector2i(345, 104), 5, sf::Color(255, 255, 255));
		InitializeUIText("", sf::Vector2i(375, 104), 5, sf::Color(44, 232, 245));

		//weapons info
		InitializeUIObject(ProfileExtras, 89, 39, sf::Vector2i(222, 118));
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(240, 124));
		InitializeUIText("", sf::Vector2i(262, 141), 5, sf::Color(254, 174, 52), false);
		//weapon name
		InitializeUIText("", sf::Vector2i(262, 148), 5, sf::Color(44, 232, 245), false);

		InitializeUIText("Damage", sf::Vector2i(354, 120), 5, sf::Color(255, 255, 255), false);
		InitializeUIText("Firerate", sf::Vector2i(342, 129), 5, sf::Color(255, 255, 255), false);
		InitializeUIText("Accuracy", sf::Vector2i(342, 138), 5, sf::Color(255, 255, 255), false);
		InitializeUIText("Ammunition", sf::Vector2i(330, 147), 5, sf::Color(255, 255, 255), false);

		InitializeUIObject(WeaponBar, 55, 5, sf::Vector2i(393, 121));
		InitializeUIObject(WeaponBar, 55, 5, sf::Vector2i(393, 130));
		InitializeUIObject(WeaponBar, 55, 5, sf::Vector2i(393, 139));
		InitializeUIObject(WeaponBar, 55, 5, sf::Vector2i(393, 148));

		this->WeaponDataVisible(false);
		this->currWeaponData = -1;
		break;
	case (GameState::GameScreen): {
		//Bottom Ui
		InitializeUIObject(GameUIBg, 176, 65, sf::Vector2i(0, 205));
		InitializeUIObject(GameUIShip, 44, 43, sf::Vector2i(23, 214));
		//Weapon Icons
		//temporary
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(91, 244));
		InitializeUIObject(Guns, 46, 11, sf::Vector2i(91, 257));

		//warning
		InitializeUIObject(GameUIWarning, 36, 40, sf::Vector2i(27, 220));
		this->objects[4]->SetVisible(false);

		//hp text
		InitializeUIText("100%", sf::Vector2i(15, 260), 5, sf::Color(255, 255, 255));
		InitializeUIText("100%", sf::Vector2i(77, 260), 5, sf::Color(255, 255, 255));
		InitializeUIText("", sf::Vector2i(140, 246), 5, sf::Color(44, 232, 245));
		InitializeUIText("", sf::Vector2i(140, 259), 5, sf::Color(44, 232, 245));


		//ui hp bars
		sf::RectangleShape* _shieldRec = new sf::RectangleShape(sf::Vector2f(5, 32));
		sf::RectangleShape* _healthRec = new sf::RectangleShape(sf::Vector2f(5, 32));

		_shieldRec->setPosition(sf::Vector2f(15, 256));
		_healthRec->setPosition(sf::Vector2f(70, 256));

		_shieldRec->setFillColor(sf::Color(44, 232, 245));
		_healthRec->setFillColor(sf::Color(255, 0, 68));

		//negative so sprite is easier to scale
		_shieldRec->setScale(1, -1);
		_healthRec->setScale(1, -1);
		this->shapes.push_back(std::move(_shieldRec));
		this->shapes.push_back(std::move(_healthRec));

		//flash for blinking ui
		this->flashTimer = std::chrono::high_resolution_clock::now();


		//top UI
		InitializeUIObject(GameUITopStats, 164, 32, sf::Vector2i(0, 0));
		//top UI stats
		InitializeUIText("0Km", sf::Vector2i(72, 3), 5, sf::Color(192, 203, 220));
		InitializeUIText("0", sf::Vector2i(72, 11), 5, sf::Color(192, 203, 220));
		InitializeUIText("0", sf::Vector2i(72, 19), 5, sf::Color(192, 203, 220));
		InitializeUIText("0", sf::Vector2i(140, 12), 10, sf::Color(255, 0, 68));
		EditText(7, "0", true);
		break;
	}
	case(GameState::GameOverScreen): {
		InitializeUIObject(GameOver, 320, 162, sf::Vector2i(80, 32));
		InitializeUIObject(InfoReturn, 154, 36, sf::Vector2i(163, 212), true);

		InitializeUIText("", sf::Vector2i(278, 116), 5, sf::Color(255, 255, 255));
		InitializeUIText("", sf::Vector2i(278, 129), 5, sf::Color(255, 255, 255));
		InitializeUIText("", sf::Vector2i(278, 142), 5, sf::Color(255, 255, 255));
		InitializeUIText("", sf::Vector2i(278, 155), 5, sf::Color(255, 255, 255));
		break;
	}
	}

	this->isInitialized = true;
}

void UIManager::FixFpsMenuButtons()
{
	int _exceptId = 0;

	switch (PlayerPrefs::FRAMERATE) {
	case 30:
		this->objects[16]->SetFrame(2);
		this->objects[16]->SetClickable(false);
		_exceptId = 16;
		break;
	case 60:
		this->objects[17]->SetFrame(2);
		this->objects[17]->SetClickable(false);
		_exceptId = 17;
		break;
	case 75:
		this->objects[18]->SetFrame(2);
		this->objects[18]->SetClickable(false);
		_exceptId = 18;
		break;
	case 144:
		this->objects[19]->SetFrame(2);
		this->objects[19]->SetClickable(false);
		_exceptId = 19;
		break;
	case 720:
		this->objects[20]->SetFrame(2);
		this->objects[20]->SetClickable(false);
		_exceptId = 20;
		break;
	}

	for (int i = 16; i <= 20; i++) {
		if (i == _exceptId)
			continue;

		this->objects[i]->SetFrame(1);
		this->objects[i]->SetClickable(true);
	}
}

void UIManager::FixSoundMenuSliders()
{
	this->objects[5]->SetFrame(PlayerPrefs::MENUMUSIC + 1);
	this->objects[8]->SetFrame(PlayerPrefs::INGAMEMUSIC + 1);
	this->objects[11]->SetFrame(PlayerPrefs::EFFECTS + 1);
}

//check what action to take on button press
UIManager::UiEvents UIManager::CheckUIButtonPress(GameState _GAMESTATE)
{
	switch (_GAMESTATE) {
	case (GameState::MainMenu):
		if (objects[1]->CheckIfHovered()) {
			return UiEvents::Play;
		}
		else if (objects[2]->CheckIfHovered()) {
			return UiEvents::Info;
		}
		else if (objects[3]->CheckIfHovered()) {
			return UiEvents::Settings;
		}
		else if (objects[4]->CheckIfHovered()) {
			return UiEvents::Credits;
		}
		else if (objects[5]->CheckIfHovered()) {
			return UiEvents::Exit;
		}
		break;
	case (GameState::InfoMenu):
		if (objects[1]->CheckIfHovered()) {
			return UiEvents::ReturnToMain;
		}
		break;
	case (GameState::CreditsMenu):
		if (objects.size() < 1)
			return UiEvents::Null;

		if (objects[1]->CheckIfHovered()) {
			return UiEvents::ReturnToMain;
		}
		break;
	case (GameState::SettingsMenu): {
		if (objects[1]->CheckIfHovered()) {
			return UiEvents::ReturnToMain;
		}

		//reload settings
		if (objects[2]->CheckIfHovered()) {
			PlayerPrefs::SetDefaults();
			return UiEvents::SettingsDefault;
		}

		//lowering volume
		if (objects[3]->CheckIfHovered() && PlayerPrefs::MENUMUSIC > 0) {
			PlayerPrefs::MENUMUSIC--;
			this->FixSoundMenuSliders();
			return UiEvents::ChangeMenuThemeVolume;
		}
		else if (objects[4]->CheckIfHovered() && PlayerPrefs::MENUMUSIC < 10) {
			PlayerPrefs::MENUMUSIC++;
			this->FixSoundMenuSliders();
			return UiEvents::ChangeMenuThemeVolume;
		}

		if (objects[6]->CheckIfHovered() && PlayerPrefs::INGAMEMUSIC > 0) {
			PlayerPrefs::INGAMEMUSIC--;
			this->FixSoundMenuSliders();
			return UiEvents::Null;
		}
		else if (objects[7]->CheckIfHovered() && PlayerPrefs::INGAMEMUSIC < 10) {
			PlayerPrefs::INGAMEMUSIC++;
			this->FixSoundMenuSliders();
			return UiEvents::Null;
		}

		if (objects[9]->CheckIfHovered() && PlayerPrefs::EFFECTS > 0) {
			PlayerPrefs::EFFECTS--;
			this->FixSoundMenuSliders();
			return UiEvents::Null;
		}
		else if (objects[10]->CheckIfHovered() && PlayerPrefs::EFFECTS < 10) {
			PlayerPrefs::EFFECTS++;
			this->FixSoundMenuSliders();
			return UiEvents::Null;
		}

		//resolution
		if (objects[12]->CheckIfHovered()) {
			PlayerPrefs::RESOLUTION = 0;
			return UiEvents::ReloadWindow;
		}
		else if (objects[13]->CheckIfHovered()) {
			PlayerPrefs::RESOLUTION = 1;
			return UiEvents::ReloadWindow;
		}
		else if (objects[14]->CheckIfHovered()) {
			PlayerPrefs::RESOLUTION = 2;
			return UiEvents::ReloadWindow;
		}
		else if (objects[15]->CheckIfHovered()) {
			PlayerPrefs::RESOLUTION = 3;
			return UiEvents::ReloadWindow;
		}

		//framerate
		if (objects[16]->CheckIfHovered()) {
			PlayerPrefs::FRAMERATE = 30;
			this->FixFpsMenuButtons();
			return UiEvents::ChangeFramerate;
		}
		else if (objects[17]->CheckIfHovered()) {
			PlayerPrefs::FRAMERATE = 60;
			this->FixFpsMenuButtons();
			return UiEvents::ChangeFramerate;
		}
		else if (objects[18]->CheckIfHovered()) {
			PlayerPrefs::FRAMERATE = 75;
			this->FixFpsMenuButtons();
			return UiEvents::ChangeFramerate;
		}
		else if (objects[19]->CheckIfHovered()) {
			PlayerPrefs::FRAMERATE = 144;
			this->FixFpsMenuButtons();
			return UiEvents::ChangeFramerate;
		}
		else if (objects[20]->CheckIfHovered()) {
			PlayerPrefs::FRAMERATE = 720;
			this->FixFpsMenuButtons();
			return UiEvents::ChangeFramerate;
		}

		//vsync
		if (objects[21]->CheckIfHovered()) {
			PlayerPrefs::VSYNC = false;
			return UiEvents::ReloadWindow;
		}
		else if (objects[22]->CheckIfHovered()) {
			PlayerPrefs::VSYNC = true;
			return UiEvents::ReloadWindow;
		}

		//window
		if (objects[23]->CheckIfHovered()) {
			PlayerPrefs::WINDOW = PlayerPrefs::Windowed;
			return UiEvents::ReloadWindow;
		}
		else if (objects[24]->CheckIfHovered()) {
			PlayerPrefs::WINDOW = PlayerPrefs::Fullscreen;
			return UiEvents::ReloadWindow;
		}
		else if (objects[25]->CheckIfHovered()) {
			PlayerPrefs::WINDOW = PlayerPrefs::Borderless;
			return UiEvents::ReloadWindow;
		}
		break;
	}
	case (GameState::LoginScreen):
		if (objects[1]->CheckIfHovered()) {
			return UiEvents::ReturnToMain;
		}
		else if (objects[2]->CheckIfHovered()) {
			return UiEvents::Sign;
		}
		break;
	case (GameState::ProfileScreen):
		if (objects[3]->CheckIfHovered()) {
			return UiEvents::ProfilePlay;
		}
		else if (objects[4]->CheckIfHovered()) {
			return UiEvents::ProfileLogout;
		}
		else if (objects[5]->CheckIfHovered()) {
			return UiEvents::Exit;
		}
		else if (objects[14]->CheckIfHovered()) {
			int _frame = objects[14]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip1;
		}
		else if (objects[15]->CheckIfHovered()) {
			int _frame = objects[15]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip2;
		}
		else if (objects[16]->CheckIfHovered()) {
			int _frame = objects[16]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip3;
		}
		else if (objects[17]->CheckIfHovered()) {
			int _frame = objects[17]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip4;
		}
		else if (objects[18]->CheckIfHovered()) {
			int _frame = objects[18]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip5;
		}
		else if (objects[19]->CheckIfHovered()) {
			int _frame = objects[19]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip6;
		}
		else if (objects[20]->CheckIfHovered()) {
			int _frame = objects[20]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip7;
		}
		else if (objects[21]->CheckIfHovered()) {
			int _frame = objects[21]->GetBaseFrame();
			if (_frame == 3 || _frame == 5)
				return UiEvents::Equip8;
		}
		break;
	case (GameState::GameOverScreen):
		if (objects[1]->CheckIfHovered()) {
			return UiEvents::Sign;
		}
		break;
	default:
		return UiEvents::Null;
		break;
	}
	return UiEvents::Null;
}

// dastroy all menu objects, allows new ones to be created
void UIManager::Uninitialize()
{
	if (!isInitialized)
		return;
	this->isInitialized = false;

	std::future<void> _unObj = std::async(std::launch::async, &UIManager::UninitializeObjects, this);
	std::future<void> _unText = std::async(std::launch::async, &UIManager::UninitializeText, this);
	std::future<void> _unShp = std::async(std::launch::async, &UIManager::UninitializeShapes, this);

	_unObj.get();
	_unText.get();
	_unShp.get();
}

void UIManager::UninitializeObjects() {
	this->objectsMtx.lock();
	for (int i = 0; i < objects.size(); i++) {
		delete this->objects[i];
	}
	this->objects.clear();
	this->objectsMtx.unlock();
}

void UIManager::UninitializeText() {

	this->textMtx.lock();
	for (int i = 0; i < textObjects.size(); i++) {
		delete this->textObjects[i];
	}
	this->textObjects.clear();
	this->textMtx.unlock();
}

void UIManager::UninitializeShapes() {
	this->shapesMtx.lock();
	for (int i = 0; i < shapes.size(); i++) {
		delete this->shapes[i];
	}
	this->shapes.clear();
	this->shapesMtx.unlock();
}

void UIManager::DrawUI()
{
	if (!isInitialized)
		return;

	this->objectsMtx.lock();
	//images
	for (int i = 0; i < objects.size(); i++) {
		if (objects[i]->IsVisible())
			this->window->draw(objects[i]->GetDrawData());
	}
	this->objectsMtx.unlock();

	this->textMtx.lock();
	//text
	for (int i = 0; i < textObjects.size(); i++) {
		if (textObjects[i]->IsVisible())
			this->window->draw(textObjects[i]->GetDrawData());
	}
	this->textMtx.unlock();

	this->shapesMtx.lock();
	for (int i = 0; i < shapes.size(); i++) {
		this->window->draw(*shapes[i]);
	}
	this->shapesMtx.unlock();

	if (this->showPauseOverlay)
		this->window->draw(pauseGray);

	//mouse
	this->cursorMtx.lock();
	if (this->cursor->IsVisible())
		this->window->draw(cursor->GetDrawData());
	this->cursorMtx.unlock();
}

void UIManager::MenuMouseHover(GameState _state)
{
	this->UpdateMousePos();
	//std::cout << _position.x << " " << position.y << "\n";
	for (int i = 0; i < objects.size(); i++) {
		objects[i]->UpdateHoverSprite(mousePos);
	}

	if (_state == GameState::ProfileScreen)
		DisplayWeaponData();
}

void UIManager::WeaponDataVisible(bool _value)
{
	for (int i = 22; i <= 27; i++)
		this->objects[i]->SetVisible(_value);

	for (int i = 7; i <= 12; i++)
		this->textObjects[i]->SetVisible(_value);
}

void UIManager::DisplayWeaponData()
{
	if (objects.size() < 27)
		return;

	if (objects[14]->CheckIfHovered() || objects[15]->CheckIfHovered()) {
		if (this->currWeaponData != 0) {
			this->currWeaponData = 0;

			this->objects[23]->SetFrame(1);

			this->objects[24]->SetFrame(5);
			this->objects[25]->SetFrame(4);
			this->objects[26]->SetFrame(5);
			this->objects[27]->SetFrame(4);

			EditText(7, "Free", true);
			EditText(8, "Las-Blaster", true);

			WeaponDataVisible(true);
		}
	}
	else if (objects[16]->CheckIfHovered() || objects[17]->CheckIfHovered()) {
		if (this->currWeaponData != 1) {
			this->currWeaponData = 1;

			this->objects[23]->SetFrame(2);

			this->objects[24]->SetFrame(2);
			this->objects[25]->SetFrame(9);
			this->objects[26]->SetFrame(2);
			this->objects[27]->SetFrame(10);

			EditText(7, "5000$", true);
			EditText(8, "Devotion", true);

			WeaponDataVisible(true);
		}
	}
	else if (objects[18]->CheckIfHovered() || objects[19]->CheckIfHovered()) {
		if (this->currWeaponData != 2) {
			this->currWeaponData = 2;

			this->objects[23]->SetFrame(3);

			this->objects[24]->SetFrame(10);
			this->objects[25]->SetFrame(2);
			this->objects[26]->SetFrame(9);
			this->objects[27]->SetFrame(1);

			EditText(7, "7000$", true);
			EditText(8, "Railpiercer", true);

			WeaponDataVisible(true);
		}
	}
	else if (objects[20]->CheckIfHovered() || objects[21]->CheckIfHovered()) {
		if (this->currWeaponData != 3) {
			this->currWeaponData = 3;

			this->objects[23]->SetFrame(4);

			this->objects[24]->SetFrame(4);
			this->objects[25]->SetFrame(6);
			this->objects[26]->SetFrame(10);
			this->objects[27]->SetFrame(3);

			EditText(7, "10000$", true);
			EditText(8, "Seeker R-System", true);

			WeaponDataVisible(true);
		}
	}
}

void UIManager::UpdateMousePos() {
	sf::Vector2i _mousePos = sf::Mouse::getPosition(*window);
	mousePos = window->mapPixelToCoords(_mousePos);
}

void UIManager::UpdateCursor(GameState _state)
{
	this->cursorMtx.lock();
	if (this->cursor == nullptr) {
		this->cursorMtx.unlock();
		return;
	}

	this->UpdateMousePos();
	this->cursor->UpdatePos(mousePos);

	if (_state != GameScreen) {
		this->cursor->SetInGameOffset(false);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
			cursor->SetFrame(2);
		else
			cursor->SetFrame(1);
	}
	else
		this->cursor->SetInGameOffset(true);

	this->cursorMtx.unlock();
}

void UIManager::ChangeObjVisibility(int _index, bool _visible)
{
	if (_index < this->objects.size() - 1 && this->objects[_index] != nullptr)
		this->objects[_index]->SetVisible(_visible);
}

void UIManager::ChangeTextVisibility(int _index, bool _visible)
{
	if (_index < this->textObjects.size() && this->textObjects[_index] != nullptr) {
		this->textObjects[_index]->SetVisible(_visible);
	}
}

void UIManager::EditText(int _index, std::string _text)
{
	if (_index < this->textObjects.size() && this->textObjects[_index] != nullptr) {
		this->textObjects[_index]->SetText(_text);
	}
}

void UIManager::EditText(int _index, std::string _text, bool _fixCenter)
{
	if (_index < this->textObjects.size() && this->textObjects[_index] != nullptr) {
		this->textObjects[_index]->SetText(_text, _fixCenter);
	}
}

void UIManager::EditText(int _index, sf::Color _color)
{
	if (_index < this->textObjects.size() && this->textObjects[_index] != nullptr) {
		this->textObjects[_index]->SetColor(_color);
	}
}

void UIManager::UpdateGameUiData(std::tuple<float, float, int, int, int, int> _data)
{
	//health bars
	//weird convertion for pixel perfect sizes
	float _shieldLevel = std::clamp(std::get<0>(_data), 0.f, 1.f);
	float _healthLevel = std::clamp(std::get<1>(_data), 0.f, 1.f);


	float _shieldBar = (float)((int)(_shieldLevel * 32)) / 32;
	float _healthBar = (float)((int)(_healthLevel * 32)) / 32;

	bool _flash = false;
	std::chrono::steady_clock::time_point _currTime = std::chrono::high_resolution_clock::now();

	float _time = (float)std::chrono::duration_cast<std::chrono::milliseconds>(_currTime - this->flashTimer).count();

	if (_time > 1000) {
		this->flashTimer = _currTime;
	}
	else if (_time > 650 && _time <= 1000) {
		_flash = true;
	}

	int _alfaChannel = this->textObjects[0]->GetDrawData().getFillColor().a;
	if (this->shapes.size() > 0 && this->shapes.size() <= 3) {

		//flash shield bar

		if (_shieldLevel == 0.f) {
			this->shapes[0]->setScale(1, -_shieldBar);
			this->EditText(0, "BRKN", true);
			this->EditText(0, sf::Color(255, 0, 68, _alfaChannel));
			this->objects[4]->SetVisible(true);
		}
		else if (_flash && _shieldLevel <= 0.35f && _shieldLevel > 0.f) {
			this->shapes[0]->setScale(1, -_shieldBar);
			this->EditText(0, "LOW!", true);
			this->EditText(0, sf::Color(255, 0, 68, _alfaChannel));
			this->objects[4]->SetVisible(true);
		}
		else if (_shieldLevel > 0.f) {
			this->shapes[0]->setScale(1, -_shieldBar);
			this->EditText(0, std::to_string((int)(_shieldLevel * 100)) + "%", true);
			this->EditText(0, sf::Color(255, 255, 255, _alfaChannel));
			this->objects[4]->SetVisible(false);
		}


		//flash health bar
		_alfaChannel = this->textObjects[1]->GetDrawData().getFillColor().a;

		if (_flash && _healthLevel <= 0.35f) {
			this->shapes[1]->setScale(1, -_healthBar);
			this->EditText(1, "LOW!", true);
			this->EditText(1, sf::Color(255, 0, 68, _alfaChannel));
		}
		else {
			this->shapes[1]->setScale(1, -_healthBar);
			this->EditText(1, std::to_string((int)(_healthLevel * 100)) + "%", true);
			this->EditText(1, sf::Color(255, 255, 255, _alfaChannel));
		}
	}


	//Ammo count
	int _crosshairFrame = 3;
	int _leftMul = 0;
	int _rightMul = 0;

	int _maxAmmoL = std::get<2>(_data);
	int _ammoL = std::get<3>(_data);
	int _maxAmmoR = std::get<4>(_data);
	int _ammoR = std::get<5>(_data);

	if (_ammoL >= _maxAmmoL * 0.34f) {
		this->EditText(2, std::to_string(_ammoL) + "/" + std::to_string(_maxAmmoL));
		this->EditText(2, sf::Color(44, 232, 245, _alfaChannel));
		_leftMul = 0;
	}
	else if (_ammoL != 0) {
		this->EditText(2, std::to_string(_ammoL) + "/" + std::to_string(_maxAmmoL));
		this->EditText(2, sf::Color(254, 174, 52, _alfaChannel));
		_leftMul = 1;
	}
	else {
		this->EditText(2, "RLAD!");
		this->EditText(2, sf::Color(255, 0, 68, _alfaChannel));
		_leftMul = 2;
	}

	if (_ammoR >= _maxAmmoR * 0.34f) {
		this->EditText(3, std::to_string(_ammoR) + "/" + std::to_string(_maxAmmoR));
		this->EditText(3, sf::Color(44, 232, 245, _alfaChannel));
		_rightMul = 0;
	}
	else if (_ammoR != 0) {
		this->EditText(3, std::to_string(_ammoR) + "/" + std::to_string(_maxAmmoR));
		this->EditText(3, sf::Color(254, 174, 52, _alfaChannel));
		_rightMul = 1;
	}
	else {
		this->EditText(3, "RLAD!");
		this->EditText(3, sf::Color(255, 0, 68, _alfaChannel));
		_rightMul = 2;
	}

	this->cursor->SetFrame(_crosshairFrame + _leftMul + _rightMul * 3);
}

void UIManager::UpdateStatUiData(int _statKills, int _statDistance, int _statEarned, int _statWave)
{
	_statKills = std::clamp(_statKills, 0, 99999999);
	_statDistance = std::clamp(_statDistance, 0, 99999);
	_statEarned = std::clamp(_statEarned, 0, 9999999);
	_statWave = std::clamp(_statWave, 0, 999);

	EditText(4, std::to_string(_statKills) + "Km");
	EditText(5, std::to_string(_statDistance));
	EditText(6, std::to_string(_statEarned));
	EditText(7, std::to_string(_statWave), true);
}

void UIManager::MakeBotGameUiTransparent(bool _value)
{
	sf::Color _color;
	int _transparency = 255;

	if (_value != this->isBotUiTransparent) {
		if (_value) {
			_transparency = 64;
			this->isBotUiTransparent = true;
		}
		else {
			_transparency = 255;
			this->isBotUiTransparent = false;
		}

		for (int i = 0; i < 5; i++) {
			_color = objects[i]->GetDrawData().getColor();
			objects[i]->SetColor(sf::Color(_color.r, _color.g, _color.b, _transparency));
		}

		for (int i = 0; i < 4; i++) {
			_color = textObjects[i]->GetDrawData().getFillColor();
			textObjects[i]->SetColor(sf::Color(_color.r, _color.g, _color.b, _transparency));
		}

		for (int i = 0; i < 2; i++) {
			_color = (*shapes[i]).getFillColor();
			(*shapes[i]).setFillColor(sf::Color(_color.r, _color.g, _color.b, _transparency));
		}
	}
}

void UIManager::MakeTopGameUiTransparent(bool _value)
{
	sf::Color _color;
	int _transparency = 255;

	if (_value != this->isTopUiTransparent) {
		if (_value) {
			_transparency = 64;
			this->isTopUiTransparent = true;
		}
		else {
			_transparency = 255;
			this->isTopUiTransparent = false;
		}

		_color = objects[5]->GetDrawData().getColor();
		objects[5]->SetColor(sf::Color(_color.r, _color.g, _color.b, _transparency));

		for (int i = 4; i < 8; i++) {
			_color = textObjects[i]->GetDrawData().getFillColor();
			textObjects[i]->SetColor(sf::Color(_color.r, _color.g, _color.b, _transparency));
		}
	}
}

void UIManager::UpdateGameOverText(std::tuple<int, int, int, int> _data)
{
	std::cout << std::get<0>(_data);

	if (this->textObjects.size() != 4)
		return;

	this->EditText(0, std::to_string(std::get<0>(_data)) + "KM");
	this->EditText(1, std::to_string(std::get<1>(_data)));
	this->EditText(2, std::to_string(std::get<2>(_data)));
	this->EditText(3, std::to_string(std::get<3>(_data)));
}

void UIManager::SelectGunSprites(int _left, int _right)
{
	if (this->objects[2] == nullptr || this->objects[3] == nullptr)
		return;

	this->objects[2]->SetFrame(_left);
	this->objects[3]->SetFrame(_right);
}

void UIManager::UpdateProfileGunButtons(int _data[8], const int _cost[4], int _playerBal)
{
	//replacing base frames
	for (int i = 0; i < 8; i++) {
		if (_data[i] != 2) {
			this->objects[14 + i]->SetBaseFrame(1 + _data[i] * 2);
			this->objects[14 + i]->SetFrame(1 + _data[i] * 2);
		}
		else {
			if (_cost[(int)std::ceil(i / 2)] <= _playerBal) {
				this->objects[14 + i]->SetBaseFrame(5);
				this->objects[14 + i]->SetFrame(5);
			}
			else {
				this->objects[14 + i]->SetBaseFrame(7);
				this->objects[14 + i]->SetFrame(7);
			}

		}
	}
}

void UIManager::ShowPauseOverlay(bool _value)
{
	if (_value != showPauseOverlay) {
		this->showPauseOverlay = _value;
		std::cout << "pause overlay " << this->showPauseOverlay << std::endl;
	}
}
