#include "Background.h"

Background::Background(const sf::Texture* _texture) : GameObject(480, 270, sf::Vector2f(0, 0))
{
	this->sprite.setTexture(*_texture);
	this->sprite.setScale(1, 1);
	this->sprite.setTextureRect(sf::IntRect(0, 0, 480, 270));
}

void Background::Animate() {
	animationFrame++;

	if (this->animationFrame > 8)
		this->animationFrame = 1;

	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));
}

const sf::Sprite& Background::GetDrawData()
{
	return this->sprite;
}
