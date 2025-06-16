#include "DangerZone.h"

DangerZone::DangerZone(DangerType _type, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, float _difficultyModifier) : GameObject(_width, _height, _position, false)
{
	this->difficultyModifier = _difficultyModifier;

	this->dangerType = _type;

	this->sprite.setTexture(*_texture);
	this->sprite.setScale(1, 1);
	this->sprite.setTextureRect(sf::IntRect(0, 0, _width, _height));

	switch (_type) {
	case Flame:
		this->animationData.insert(std::make_pair(0, std::make_pair(1, 4)));
		this->damage = 40 * _difficultyModifier;
		this->sprite.setOrigin(112, 27);
		this->hitbox = sf::Vector2f(110, 26);
		break;
	case Laser:
		this->animationData.insert(std::make_pair(0, std::make_pair(1, 8)));
		this->animationData.insert(std::make_pair(1, std::make_pair(9, 12)));
		this->animationData.insert(std::make_pair(2, std::make_pair(13, 14)));
		this->stopAnimFrames = { 8,14 };
		this->damage = 45 * _difficultyModifier;
		this->sprite.setOrigin(474.5, 5.5);
		this->hitbox = sf::Vector2f(480, 12);
		break;
	}
}

const sf::Sprite& DangerZone::GetDrawData()
{
	return this->sprite;
}

void DangerZone::Activate(bool _value)
{
	this->active = _value;
	this->SetVisible(_value);
}

void DangerZone::UpdatePosition(sf::Vector2f _pos)
{
	this->position = _pos;
	this->sprite.setPosition(_pos);

	switch (dangerType) {
	case Flame:
		this->hitboxPos = this->position - sf::Vector2f(110, 24);
		this->fixedHitbox = sf::Vector2f(this->hitboxPos.x + hitbox.x / 2, this->hitboxPos.y + hitbox.y / 2);
		break;
	case Laser:
		this->hitboxPos = this->position - sf::Vector2f(480, 12);
		this->fixedHitbox = sf::Vector2f(this->hitboxPos.x + hitbox.x / 2, this->hitboxPos.y + hitbox.y / 2);
		break;
	}
}

void DangerZone::Animate() {

	std::pair<int, int> _currRange = this->animationData.at(animState);
	int _botFrame = _currRange.first;
	int _topFrame = _currRange.second;

	if (animationFrame == _topFrame && std::find(stopAnimFrames.begin(), stopAnimFrames.end(), this->animationFrame) != stopAnimFrames.end())
		return;

	if (animationFrame >= _botFrame && animationFrame < _topFrame) {
		this->animationFrame++;
	}
	else
		this->animationFrame = _botFrame;

	this->sprite.setTextureRect(sf::IntRect(this->width * (animationFrame - 1), 0, this->width, this->height));
}

bool DangerZone::CheckCollision(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox)
{
	bool _ifHit = false;

	if (active && abs(this->fixedHitbox.x - _hitboxPos.x) <= ((float)this->hitbox.x + _hitbox.x) / 2 && abs(this->fixedHitbox.y - _hitboxPos.y) <= ((float)this->hitbox.y + _hitbox.y) / 2) {
		_ifHit = true;
	}

	return _ifHit;
}

int DangerZone::GetDamage() const
{
	return this->damage;
}

void DangerZone::SetFrameState(int _animState)
{
	this->animState = _animState;
}


