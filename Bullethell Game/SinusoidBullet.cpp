#include "SinusoidBullet.h"


SinusoidBullet::SinusoidBullet(int _width, int _height, sf::Vector2f _position, Type _type, int _offsetCount) : Bullet(_width, _height, _position, _type) {
	this->baseYPos = _position.y;

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	this->sinOffset = _position.x;
	this->offsetModifier = _offsetCount;
	this->angleRad = this->ConvertToRad(180);
}

std::vector<sf::Vector2f> SinusoidBullet::UpdatePosition(float _deltaTime)
{
	sf::Vector2f _movement(0, 0);

	std::vector<sf::Vector2f> _positionChecks;

	for (int i = 0; i < logicCheckSteps; i++) {
		_movement.x = this->speed * _deltaTime * 100 / logicCheckSteps;

		this->position.x -= _movement.x;
		this->position.y = baseYPos + 20 * sin(((this->position.x - sinOffset) / 20) + 3.14 * offsetModifier);
		_positionChecks.push_back(this->position);
	}
	this->sprite.setPosition(this->position);

	sf::Vector2f _pos = this->position;
	if (_pos.x < -10 || _pos.x > 490 || _pos.y < -10 || _pos.y > 280)
		this->despawn = true;

	return _positionChecks;
}
