#include "UIText.h"

UIText::UIText(std::string _text, int _width, int _height, sf::Vector2i _position, const sf::Font* _font, int _size, sf::Color _color) : GameObject(_width, _height, sf::Vector2f(_position.x, _position.y)) {

	this->text.setString(_text);
	this->text.setPosition(_position.x, _position.y);
	this->text.setCharacterSize(_size*5);
	this->text.scale(0.2f, 0.2f);
	this->text.setFillColor(_color);
	this->text.setFont(*_font);
}

UIText::UIText(std::string _text, int _width, int _height, sf::Vector2i _position, const sf::Font* _font, int _size, sf::Color _color, bool _isVisible) : GameObject(_width, _height, sf::Vector2f(_position.x, _position.y)) {

	this->text.setString(_text);
	this->text.setPosition(_position.x, _position.y);
	this->text.setCharacterSize(_size * 5);
	this->text.scale(0.2f, 0.2f);
	this->text.setFillColor(_color);
	this->text.setFont(*_font);

	this->isVisible = _isVisible;
}

UIText::~UIText() {};

const sf::Text& UIText::GetDrawData()
{
	return this->text;
}

void UIText::SetText(std::string _text)
{
	this->text.setString(_text);
}

void UIText::SetText(std::string _text, bool _fixCenter)
{
	this->text.setString(_text);

	if (_fixCenter)
		this->SetCentered();
	else
		this->text.setPosition(this->position);
}

void UIText::SetColor(sf::Color _color)
{
	this->text.setFillColor(_color);
}

void UIText::SetCentered()
{
	int _centerOffset = this->text.getString().toAnsiString().length() * this->text.getCharacterSize() / 8.375f;
	this->text.setPosition(sf::Vector2f(this->position.x - _centerOffset, this->position.y));
}
