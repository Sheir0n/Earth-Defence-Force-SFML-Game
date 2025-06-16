#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <future>

#include "UIObject.h"
#include "Enums.h"
#include "UIText.h"
#include "Crosshair.h"

#include "PlayerPrefs.h"

class UIManager {
private:
	sf::RenderWindow* window = nullptr;

	//loading from files
	sf::Font textFont;
	std::unordered_map<TextureId, sf::Texture> textures;
	void LoadTexture(TextureId _name, std::string _path);

	std::vector <UIObject*> objects;
	std::vector <UIText*> textObjects;
	std::vector <sf::RectangleShape*> shapes;
	Crosshair* cursor;
	bool isInitialized = false;

	sf::Vector2f mousePos;

	void InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position);

	void InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position, int _frame, bool _flipped);

	void InitializeUIObject(TextureId _txtId, int _width, int _height, sf::Vector2i _position, bool _isClickable);

	void InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color);

	void InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color, bool _isVisible);

	void InitializeUIText(std::string _text, sf::Vector2i _position, int _size, sf::Color _color, bool _isVisible, bool _isCentered);

	//prevents from accesing currently drawn images in update thread
	std::mutex objectsMtx;
	std::mutex textMtx;
	std::mutex shapesMtx;
	std::mutex cursorMtx;

	//timer for flashing ui
	std::chrono::steady_clock::time_point flashTimer;

	bool isBotUiTransparent = false;
	bool isTopUiTransparent = false;

	//fixes fps buttons in settings without full reload
	void FixFpsMenuButtons();
	//fixes sound slider
	void FixSoundMenuSliders();

	void UpdateMousePos();

	bool showPauseOverlay = false;
	sf::RectangleShape pauseGray;

	int currWeaponData = -1;
	void WeaponDataVisible(bool _value);
	void DisplayWeaponData();
public:

	enum UiEvents { Null, Play, Info, Settings, Credits, Exit, ReturnToMain, Sign, ProfilePlay, ProfileLogout, ReloadWindow, ChangeFramerate, SettingsDefault, ChangeMenuThemeVolume, Equip1, Equip2, Equip3, Equip4, Equip5, Equip6, Equip7, Equip8 };

	UIManager(sf::RenderWindow* _window);
	~UIManager();

	void UpdateWindowPtr(sf::RenderWindow* _window);

	//initializes menu objects
	void InitializeMenu(GameState _GAMESTATE);

	//destroys all menu objects
	void Uninitialize();
	void UninitializeObjects();
	void UninitializeText();
	void UninitializeShapes();

	//draws existing objects inside object vector
	void DrawUI();

	//checks mouse hovering over buttons
	void MenuMouseHover(GameState _state);
	void UpdateCursor(GameState _state);

	//checks mouse being clicked over button
	UiEvents CheckUIButtonPress(GameState _GAMESTATE);

	//allows to change items visibility
	void ChangeObjVisibility(int _index, bool _visible);
	void ChangeTextVisibility(int _index, bool _visible);

	void EditText(int _index, std::string _text);

	void EditText(int _index, std::string _text, bool _fixCenter);

	void EditText(int _index, sf::Color _color);

	void UpdateGameUiData(std::tuple<float, float, int, int, int, int>);
	void UpdateStatUiData(int, int, int, int);

	void MakeBotGameUiTransparent(bool _value);
	void MakeTopGameUiTransparent(bool _value);

	void UpdateGameOverText(std::tuple<int, int, int, int>);

	void ShowPauseOverlay(bool _value);

	void SelectGunSprites(int _left, int _right);

	void UpdateProfileGunButtons(int _data[8], const int _cost[4], int _playerBal);
};