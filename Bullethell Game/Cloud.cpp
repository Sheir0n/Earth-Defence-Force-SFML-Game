#include "Cloud.h"

Cloud::Cloud(int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, int _layer, bool _isFlipped) : GameObject(_width, _height, _position)
{
	this->sprite.setTexture(*_texture);
	this->sprite.setPosition(_position);

	if (_isFlipped) {
		this->sprite.setScale(-1, 1);
		this->sprite.setOrigin(this->width, this->height / 2.f);
	}
	else
	{
		this->sprite.setScale(1, 1);
		this->sprite.setOrigin(0, this->height / 2.f);
	}

	this->layer = _layer;
	this->speed = 1.f / _layer;
}

void Cloud::UpdateOffset(float _playerPos)
{
	this->yOffset = (_playerPos - 135.f) / (15.f * (float)layer);
}

void Cloud::OnUpdate(float _deltaTime)
{
	this->position.x -= speed * _deltaTime * 50;
	this->sprite.setPosition(sf::Vector2f(position.x, position.y - this->yOffset));

	if (this->position.x + this->width < 0)
		this->allowDelete = true;
}

const sf::Sprite& Cloud::GetDrawData()
{
	return this->sprite;
}

bool Cloud::CheckDelete() const
{
	return this->allowDelete;
}
