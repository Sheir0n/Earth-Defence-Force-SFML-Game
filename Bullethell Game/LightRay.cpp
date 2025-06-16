#include "LightRay.h"

LightRay::LightRay(sf::Vector2f _pos, const sf::Texture* _texture) : GameObject(560, 48, _pos)
{
	this->sprite.setTexture(*_texture);
	this->sprite.setTextureRect(sf::IntRect(0, 0, 560, 48));
	this->sprite.setOrigin(0, this->height / 2.f);
	this->sprite.setPosition(_pos);

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _angleDist(0, 360);
	std::uniform_int_distribution<std::mt19937::result_type> _speedDist(2, 8);
	std::uniform_int_distribution<std::mt19937::result_type> _alfaDist(32, 64);
	std::uniform_int_distribution<std::mt19937::result_type> _sizeWDist(100, 200);
	std::uniform_int_distribution<std::mt19937::result_type> _sizeLDist(100, 200);
	this->rotation = _angleDist(_rng);
	this->speed = _speedDist(_rng);

	this->sprite.setRotation(this->rotation);
	this->sprite.setColor(sf::Color(255, 255, 255, _alfaDist(_rng)));
	this->sprite.setScale(_sizeWDist(_rng) / 100.f, _sizeLDist(_rng) / 100.f);
}

const sf::Sprite& LightRay::GetDrawData()
{
	return this->sprite;
}

void LightRay::OnUpdate(float _deltaTime) {

	this->rotation += speed * _deltaTime * 5.f;
	this->sprite.setRotation(this->rotation);
}
