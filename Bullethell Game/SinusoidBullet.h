#pragma once
#include "Bullet.h"

class SinusoidBullet : public Bullet {
private:
	float baseYPos = 0;
	float sinOffset = 0;
	int offsetModifier = 0;
public:
	SinusoidBullet(int _width, int _height, sf::Vector2f _position, Type _type, int _offsetCount);
	 std::vector<sf::Vector2f> UpdatePosition(float _deltaTime) override;



};