#pragma once
#include "Bullet.h"

class RocketBullet : public Bullet{
private:
	float baseTurnSpeed = 0;
	float turnspeedLossPerSec = 20;

	void ApplyTargetingRotation(float _deltaTime);
public:
	RocketBullet(int _width, int _height, sf::Vector2f _position, Type _type, float _baseTurnSpeed,float _turnspeedLossPerSec);
	 std::vector<sf::Vector2f> UpdatePosition(float _deltaTime) override;


};