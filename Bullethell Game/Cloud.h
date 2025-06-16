#pragma once
#include "GameObject.h"
#include <iostream>
class Cloud : public GameObject {
private:
	sf::Sprite sprite;
	//lower number = closer and moves faster
	int layer = 0;
	float speed = 1.f;
	bool allowDelete = false;
	float yOffset = 0.f;

public:
	Cloud(int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, int _layer, bool _isFlipped);
	void UpdateOffset(float _playerY);
	void OnUpdate(float _deltaTime);
	const sf::Sprite& GetDrawData();

	bool CheckDelete() const;
};