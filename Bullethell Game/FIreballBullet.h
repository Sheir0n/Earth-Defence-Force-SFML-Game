#pragma once
#include "Bullet.h"

class FireballBullet : public Bullet {
private:
	float speedModifier = 1.f;	
	float speedIncreasePerSec = 7.f;
	const float turnSpeed = 1.65f;

	void ApplyTargetingRotation(float _deltaTime);
public:
	FireballBullet(int _width, int _height, sf::Vector2f _position, Type _type, float _modifier, sf::Vector2f _targetPos);
	std::vector<sf::Vector2f> UpdatePosition(float _deltaTime) override;
};
