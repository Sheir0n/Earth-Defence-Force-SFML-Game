#include "FIreballBullet.h"

FireballBullet::FireballBullet(int _width, int _height, sf::Vector2f _position, Type _type, float _modifier, sf::Vector2f _targetPos) : Bullet(_width, _height, _position, _type)
{
	this->speedIncreasePerSec *= _modifier;
	this->CalculateRotation(_targetPos);
}

std::vector<sf::Vector2f> FireballBullet::UpdatePosition(float _deltaTime)
{
	sf::Vector2f _movement(0, 0);

	std::vector<sf::Vector2f> _positionChecks;

	for (int i = 0; i < logicCheckSteps; i++) {
		this->ApplyTargetingRotation(_deltaTime);

		_movement.x = cosf(angleRad) * (this->speed * speedModifier / logicCheckSteps) * _deltaTime * 100;
		_movement.y = sinf(angleRad) * (this->speed * speedModifier / logicCheckSteps) * _deltaTime * 100;

		this->position += _movement;

		_positionChecks.push_back(this->position);
	}
	this->speedModifier += speedIncreasePerSec * _deltaTime;

	this->sprite.setPosition(this->position);

	sf::Vector2f _pos = this->position;
	if (_pos.x < -20 || _pos.x > 500 || _pos.y < -20 || _pos.y > 290)
		this->despawn = true;

	return _positionChecks;
}

void FireballBullet::ApplyTargetingRotation(float _deltaTime)
{
	float _rad = atan2f(this->targetPos.y - this->position.y, this->targetPos.x - this->position.x);
	float _targetRotation = _rad * (180.f / 3.14159);

	while (this->angleDeg >= 180)
		this->angleDeg -= 360;
	while (this->angleDeg <= -180)
		this->angleDeg += 360;

	float _difference = _targetRotation - this->angleDeg;
	if (abs(_difference) <= 180)
		this->angleDeg += _difference * _deltaTime * turnSpeed / logicCheckSteps;
	else if (_difference >= 0)
		this->angleDeg -= (360.f - _difference) * _deltaTime * turnSpeed / logicCheckSteps;
	else
		this->angleDeg += (360.f + _difference) * _deltaTime * turnSpeed / logicCheckSteps;

	this->angleRad = this->ConvertToRad(angleDeg);
}
