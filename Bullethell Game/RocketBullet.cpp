#include "RocketBullet.h"

RocketBullet::RocketBullet(int _width, int _height, sf::Vector2f _position, Type _type, float _baseTurnSpeed, float _turnspeedLossPerSec) : Bullet(_width, _height, _position, _type)
{
	this->baseTurnSpeed = _baseTurnSpeed;
	this->turnspeedLossPerSec = _turnspeedLossPerSec;
	this->angleDeg = 180;
	this->angleRad = this->ConvertToRad(angleDeg);
}

std::vector<sf::Vector2f> RocketBullet::UpdatePosition(float _deltaTime)
{
	sf::Vector2f _movement(0, 0);

	std::vector<sf::Vector2f> _positionChecks;

	for (int i = 0; i < logicCheckSteps; i++) {
		this->ApplyTargetingRotation(_deltaTime);

		_movement.x = cosf(angleRad) * (this->speed / logicCheckSteps) * _deltaTime * 100;
		_movement.y = sinf(angleRad) * (this->speed / logicCheckSteps) * _deltaTime * 100;

		this->position += _movement;

		_positionChecks.push_back(this->position);
	}
	this->sprite.setPosition(this->position);

	sf::Vector2f _pos = this->position;
	if (_pos.x < -50 || _pos.x > 530 || _pos.y < -50 || _pos.y > 320)
		this->despawn = true;

	return _positionChecks;
}

void RocketBullet::ApplyTargetingRotation(float _deltaTime)
{
	float _rad = atan2f(this->targetPos.y - this->position.y, this->targetPos.x - this->position.x);
	float _targetRotation = _rad * (180.f / 3.14159);

	while (this->angleDeg >= 180)
		this->angleDeg -= 360;
	while (this->angleDeg <= -180)
		this->angleDeg += 360;

	float _difference = _targetRotation - this->angleDeg;
	if (abs(_difference) <= 180)
		this->angleDeg += _difference * _deltaTime * baseTurnSpeed/ logicCheckSteps;
	else if (_difference >= 0)
		this->angleDeg -= (360.f - _difference) * _deltaTime * baseTurnSpeed / logicCheckSteps;
	else
		this->angleDeg += (360.f + _difference) * _deltaTime * baseTurnSpeed / logicCheckSteps;

	this->baseTurnSpeed = std::clamp(baseTurnSpeed - turnspeedLossPerSec * _deltaTime / logicCheckSteps, 0.f, 360.f);

	this->angleRad = this->ConvertToRad(angleDeg);
	this->sprite.setRotation(this->angleDeg);
}
