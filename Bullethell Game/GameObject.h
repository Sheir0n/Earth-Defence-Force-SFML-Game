#pragma once

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class GameObject {
protected:
	int width;
	int height;
	sf::Vector2f position;
	sf::Vector2f positionOffset;

	bool isVisible;

public:
	GameObject(int _width, int _height, sf::Vector2f _position);
	GameObject(int _width, int _height, sf::Vector2f _position, bool _isVisible);

	virtual void OnUpdate(float _deltaTime) = 0;

	void SetVisible(bool _isVisible);
	bool IsVisible();

	sf::Vector2f GetPosition() const;
	sf::Vector2f GetOffset() const;
};