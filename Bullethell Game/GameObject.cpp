#include "GameObject.h"

GameObject::GameObject(int _width, int _height, sf::Vector2f _position) {
	this->width = _width;
	this->height = _height;
	this->positionOffset.x = -1.f * _width / 2;
	this->positionOffset.y = -1.f * _height / 2;
	this->position = _position;
	this->isVisible = true;
}

GameObject::GameObject(int _width, int _height, sf::Vector2f _position, bool _isVisible) {
	this->width = _width;
	this->height = _height;
	this->positionOffset.x = -1.f * _width / 2;
	this->positionOffset.y = -1.f * _height / 2;
	this->position = _position;
	this->isVisible = _isVisible;
}

void GameObject::SetVisible(bool _isVisible)
{
	this->isVisible = _isVisible;
}

bool GameObject::IsVisible()
{
	return isVisible;
}

sf::Vector2f GameObject::GetPosition() const
{
	return this->position;
}

sf::Vector2f GameObject::GetOffset() const
{
	return this->positionOffset;
}
