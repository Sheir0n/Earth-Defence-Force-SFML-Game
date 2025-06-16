#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GameObject.h"

#include <string>
#include <iostream>
//this is a wrapper class for easier logic processing of text
class UIText : public GameObject {
private:
	sf::Text text;
	std::string textString;
public:
	UIText() = delete;

	UIText(std::string _name, int _width, int _height, sf::Vector2i _position, const sf::Font* _font, int _size, sf::Color _color);

	UIText(std::string _name, int _width, int _height, sf::Vector2i _position, const sf::Font* _font, int _size, sf::Color _color, bool _isVisible);

	~UIText();
	void OnUpdate(float _deltaTime) {};

	const sf::Text& GetDrawData();

	void SetText(std::string _text);
	void SetText(std::string _text, bool _fixCenter);

	void SetColor(sf::Color _color);

	void SetCentered();
};

