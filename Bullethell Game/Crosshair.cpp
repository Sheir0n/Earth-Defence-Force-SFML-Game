#include "Crosshair.h"

Crosshair::Crosshair(const sf::Texture* _texture) : GameObject(19, 19, sf::Vector2f(0, 0))
{
	this->sprite.setTexture(*_texture);
	this->sprite.setScale(1, 1);
	this->sprite.setTextureRect(sf::IntRect(0, 0, 19, 19));
	this->sprite.setOrigin(0.f, 0.f);
}

void Crosshair::UpdatePos(sf::Vector2f _pos)
{
	this->position = _pos;
	this->sprite.setPosition(_pos);
}

const sf::Sprite& Crosshair::GetDrawData()
{
	return this->sprite;
}

void Crosshair::SetFrame(int _frame)
{
	if (_frame == this->frame)
		return;

	this->frame = _frame;
	this->sprite.setTextureRect(sf::IntRect(this->width * (_frame - 1), 0, 19, 19));

}

void Crosshair::SetInGameOffset(bool _isInGame)
{
	if (this->inGameOffset == _isInGame)
		return;

	this->inGameOffset = _isInGame;

	if (_isInGame)
		this->sprite.setOrigin(this->width / 2.f, this->height / 2.f);
	else
		this->sprite.setOrigin(0.f, 0.f);
}
