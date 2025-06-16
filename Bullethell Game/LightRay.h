#pragma once

#include "GameObject.h"
#include "random"

class LightRay : public GameObject {
private:
	sf::Sprite sprite;

	float rotation = 0.f;
	int speed = 5;
public:
	LightRay(sf::Vector2f _pos,const sf::Texture* _texture);
	void OnUpdate(float _deltaTime);

	const sf::Sprite& GetDrawData();
};