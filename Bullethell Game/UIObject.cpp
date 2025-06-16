#include "UIObject.h"

UIObject::UIObject(int _width, int _height, sf::Vector2i _position) : GameObject(_width, _height, sf::Vector2f(_position.x, _position.y)) {
	this->sprite.setScale(1, 1);
	this->sprite.setPosition(_position.x, _position.y);
}

UIObject::UIObject(int _width, int _height, sf::Vector2i _position, bool _isClickable) : GameObject(_width, _height, sf::Vector2f(_position.x, _position.y)) {
	this->sprite.setScale(1, 1);
	this->sprite.setPosition(_position.x, _position.y);
	this->isClickable = _isClickable;
}

UIObject::UIObject(int _width, int _height, sf::Vector2i _position, int _frameCount) : GameObject(_width, _height, sf::Vector2f(_position.x, _position.y))
{
	this->sprite.setScale(1, 1);
	this->sprite.setPosition(_position.x, _position.y);
	this->isClickable = true;
}

UIObject::~UIObject() {
}

const sf::Sprite& UIObject::GetDrawData() {
	return this->sprite;
}

void UIObject::SetClickable(bool _isClickable)
{
	this->isClickable = _isClickable;
}

void UIObject::SetTexture(const sf::Texture* _texture)
{
	this->sprite.setTexture(*_texture);
	this->constraints = sf::Vector2<int>(0, 0);
}

void UIObject::SetTexture(const sf::Texture* _texture, sf::Vector2<int> _constraints)
{
	this->constraints = _constraints;
	this->sprite.setTextureRect(sf::IntRect(0, 0, _constraints.x, _constraints.y));
	this->sprite.setTexture(*_texture);
}

void UIObject::UpdateHoverSprite(sf::Vector2f _mousePos)
{
	if (!isClickable) {
		this->isHovered = false;
		return;
	}

	if (_mousePos.x >= this->position.x && _mousePos.x <= this->position.x + width && _mousePos.y >= this->position.y && _mousePos.y <= this->position.y + height) {
		if (isHovered)
			return;
		else
		{
			this->isHovered = true;
			this->frame = baseFrame + 1;
			this->sprite.setTextureRect(sf::IntRect(this->constraints.x * (frame - 1), 0, this->constraints.x, this->constraints.y));
		}
	}
	else {
		if (isHovered) {
			this->isHovered = false;
			this->frame = baseFrame;
			this->sprite.setTextureRect(sf::IntRect(this->constraints.x * (frame - 1), 0, this->constraints.x, this->constraints.y));
		}
		else
			return;
	}
}

bool UIObject::CheckIfHovered() const
{
	return this->isHovered;
}

void UIObject::SetFrame(int _frame)
{
	if (this->frame == _frame)
		return;

	this->frame = _frame;
	this->sprite.setTextureRect(sf::IntRect(this->constraints.x * (_frame - 1), 0, this->constraints.x, this->constraints.y));
}

void UIObject::SetColor(sf::Color _color)
{
	this->sprite.setColor(_color);
}

void UIObject::Flip(bool _flip)
{
	if (_flip) {
		this->sprite.setScale(-1, 1);
		this->sprite.setOrigin(width, 0);
	}
	else
	{
		this->sprite.setScale(1, 1);
		this->sprite.setOrigin(0, 0);
	}
}

void UIObject::SetBaseFrame(int _frame)
{
	this->baseFrame = _frame;
}

int UIObject::GetBaseFrame() const
{
	return this->baseFrame;
}
