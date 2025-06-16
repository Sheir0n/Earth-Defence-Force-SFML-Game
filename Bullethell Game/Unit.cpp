#include "Unit.h"
Unit::Unit(int _id, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, sf::Vector2<int> _hitbox, sf::Vector2<int> _hitboxOffset) : GameObject(_width, _height, _position)
{
	this->id = _id;
	this->hitbox = _hitbox;
	this->hitboxOffset = _hitboxOffset;

	this->audioPlayer = new AudioPlayer();

	this->fixedPosition = sf::Vector2f(this->position.x - this->positionOffset.x, this->position.y - this->positionOffset.y);

	this->sprite.setTexture(*_texture);
	this->sprite.setScale(1, 1);
	this->sprite.setTextureRect(sf::IntRect(0, 0, _width, _height));
	this->sprite.setPosition(fixedPosition);
	this->sprite.setOrigin(this->width / 2.f, this->height / 2.f);
}

void Unit::SetHitbox(sf::Vector2<int> _hitbox)
{
	this->hitbox = _hitbox;
}

void Unit::SetHitboxActive(bool _isActive)
{
	this->hitboxActive = _isActive;
}

const sf::Sprite& Unit::GetDrawData()
{
	return this->sprite;
}

sf::Vector2f Unit::GetFixedPos() const
{
	return this->fixedPosition;
}

int Unit::GetSign(float _value)
{
	if (_value >= 0)
		return 1;
	else
		return -1;
}

void Unit::UpdatePosition(float _deltaTime)
{
	this->fixedPosition = sf::Vector2f(this->position.x - this->positionOffset.x, this->position.y - this->positionOffset.y);

	this->fixedHitboxPos.x = this->position.x + hitboxOffset.x + (float)hitbox.x / 2;
	this->fixedHitboxPos.y = this->position.y + hitboxOffset.y + (float)hitbox.y / 2;

	this->sprite.setPosition(fixedPosition);
}

bool Unit::CheckIfHit(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox)
{
	bool _ifHit = false;

	if (hitboxActive && abs(this->fixedHitboxPos.x - _hitboxPos.x) <= ((float)this->hitbox.x + _hitbox.x) / 2 && abs(this->fixedHitboxPos.y - _hitboxPos.y) <= ((float)this->hitbox.y + _hitbox.y) / 2) {
		_ifHit = true;
	}

	return _ifHit;
}

bool Unit::IsHitboxActive() const
{
	return this->hitboxActive;
}

int Unit::GetId() const
{
	return this->id;
}

bool Unit::CheckDamagedHighlight() const
{
	if (damageHighlightTimer <= 100)
		return true;
	else
		return false;
}

sf::Vector2f Unit::GetFixedHitbox() const
{
	return this->fixedHitboxPos;
}

sf::Vector2i Unit::GetHitbox() const
{
	return this->hitbox;
}

int Unit::GetContactDamage() const
{
	return this->contactDamage;
}

bool Unit::DamageTimerCheck()
{
	if (this->damageInvTimer > this->damageInvMs) {
		this->damageInvTimer = 0;
		return true;
	}
	else
		return false;
}

void Unit::UpdateUnitTimers(float _deltaTime)
{
	damageInvTimer += 1000 * _deltaTime;
	damageHighlightTimer += 1000 * _deltaTime;
}
