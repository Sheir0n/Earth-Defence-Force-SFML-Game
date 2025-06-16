#include "Bullet.h"

Bullet::Bullet(int _width, int _height, sf::Vector2f _position, Type _type) : GameObject(_width, _height, _position)
{
	this->bulletType = _type;
	switch (_type) {
		//player
	case(BlasterB):
		this->damage = 14;
		this->speed = 3.4f;
		this->inaccuracyInDeg = 2.5;
		this->hitboxSize = 6;
		this->penetration = 0;
		this->owner = Player;

		this->animFrameCount = 2;
		break;
	case(SpitterB):
		this->damage = 8;
		this->speed = 2.85f;
		this->inaccuracyInDeg = 7;
		this->hitboxSize = 2;
		this->penetration = 0;
		this->owner = Player;

		this->animFrameCount = 4;
		break;
	case(RailB):
		this->damage = 60;
		this->speed = 6.75f;
		this->inaccuracyInDeg = 0.2;
		this->hitboxSize = 5;
		this->penetration = 2;
		this->owner = Player;

		this->animFrameCount = 2;
		break;
	case(RocketB):
		this->damage = 13;
		this->speed = 1.8f;
		this->inaccuracyInDeg = 0;
		this->hitboxSize = 3;
		this->penetration = 0;
		this->owner = Player;

		this->animFrameCount = 2;
		break;
		//enemy
	case(DroneB):
		this->damage = 15;
		this->speed = 1.5f;
		this->inaccuracyInDeg = 0.5f;
		this->hitboxSize = 2;
		this->penetration = 0;
		this->owner = Enemy;

		this->animFrameCount = 2;
		break;
	case(GunnerB):
		this->damage = 9;
		this->speed = 1.75f;
		this->inaccuracyInDeg = 1.5f;
		this->hitboxSize = 2;
		this->penetration = 0;
		this->owner = Enemy;

		this->animFrameCount = 2;
		break;
	case(RocketeerB):
		this->damage = 12;
		this->speed = 2.f;
		this->inaccuracyInDeg = 0.f;
		this->hitboxSize = 2;
		this->penetration = 1;
		this->owner = Enemy;

		this->animFrameCount = 2;
		break;
	case(RocketeerR):
		this->damage = 10;
		this->speed = 1.75f;
		this->inaccuracyInDeg = 0.f;
		this->hitboxSize = 2;
		this->penetration = 0;
		this->owner = Enemy;

		this->animFrameCount = 2;
		break;
	case(SeekerB):
		this->damage = 50;
		this->speed = 0.5f;
		this->inaccuracyInDeg = 0.f;
		this->hitboxSize = 4;
		this->penetration = 0;
		this->owner = Enemy;

		this->animFrameCount = 4;
		break;
	default:
		this->damage = 5;
		this->speed = 3.25f;
		this->inaccuracyInDeg = 3;
		this->hitboxSize = 3;
		this->penetration = 1;
		this->owner = Player;
	}

	this->SetVisible(true);
}

float Bullet::CalculateRotation(sf::Vector2f _relPos)
{
	sf::Vector2f _thisPos = this->position;
	float _angleRad = std::atan2f(_relPos.y - _thisPos.y, _relPos.x - _thisPos.x);

	std::mt19937 rng(std::random_device{}());
	float _rand = std::generate_canonical< float, 5 >(rng);
	float _randOffset = ConvertToRad(-this->inaccuracyInDeg + this->inaccuracyInDeg * _rand * 2);

	this->angleDeg = ConvertToDeg(_angleRad + _randOffset);
	this->angleRad = _angleRad + _randOffset;
	return this->angleRad;
}

float Bullet::CalculateRotation(float _angDeg)
{
	//angle with random offset
	std::mt19937 rng(std::random_device{}());
	float _rand = std::generate_canonical< float, 5 >(rng);
	float _randOffset = ConvertToRad(-this->inaccuracyInDeg + this->inaccuracyInDeg * _rand * 2);
	this->angleRad = ConvertToRad(_angDeg) + _randOffset;
	this->angleDeg = ConvertToDeg(angleRad);
	return this->angleRad;
}

float Bullet::ConvertToDeg(float _rad)
{
	return(_rad * (180.f / 3.14159));
}

float Bullet::ConvertToRad(float _deg)
{
	return _deg * (3.14159 / 180.f);
}

void Bullet::LoadData(sf::Vector2f _pos, sf::Vector2f _targetPos, bool _activeHitbox)
{
	this->position = _pos - positionOffset;
	this->activeHitbox = _activeHitbox;
	this->CalculateRotation(_targetPos);
}

void Bullet::LoadData(sf::Vector2f _pos, float _roationDeg, bool _activeHitbox)
{
	this->position = _pos - positionOffset;
	this->activeHitbox = _activeHitbox;
	this->CalculateRotation(_roationDeg);
}

void Bullet::LoadTextures(std::unordered_map<TextureId, sf::Texture>& _textures)
{
	switch (this->bulletType) {
	case (BlasterB):
		this->sprite.setTexture(_textures.at(BlasterBullet));
		break;
	case (SpitterB):
		this->sprite.setTexture(_textures.at(SpitterBullet));
		break;
	case (RailB):
		this->sprite.setTexture(_textures.at(PiercerBullet));
		break;
	case (RocketB):
		this->sprite.setTexture(_textures.at(LauncherBullet));
		break;
	case (DroneB):
		this->sprite.setTexture(_textures.at(DroneBullet));
		break;
	case (GunnerB):
		this->sprite.setTexture(_textures.at(GunnerBullet));
		break;
	case (RocketeerB):
		this->sprite.setTexture(_textures.at(RocketeerBullet));
		break;
	case (RocketeerR):
		this->sprite.setTexture(_textures.at(RocketeerRocket));
		break;
	case (SeekerB):
		this->sprite.setTexture(_textures.at(SeekerBullet));
		break;
	}

	this->LoadSpriteData();
}

void Bullet::LoadSpriteData()
{
	this->sprite.setScale(1, 1);
	this->sprite.setTextureRect(sf::IntRect(0, 0, this->width, this->height));
	this->sprite.setPosition(position);
	this->sprite.setOrigin(this->width / 2.f, this->height / 2.f);
	this->sprite.setRotation(this->angleDeg);
}

void Bullet::ApplyModifier(float _modifier)
{
	this->damage *= _modifier;
	this->speed *= _modifier;
}

const sf::Sprite& Bullet::GetDrawData()
{
	return this->sprite;
}

std::vector<sf::Vector2f> Bullet::UpdatePosition(float _deltaTime) {
	sf::Vector2f _movement(0, 0);

	std::vector<sf::Vector2f> _positionChecks;

	for (int i = 0; i < logicCheckSteps; i++) {
		_movement.x = cosf(angleRad) * (this->speed / logicCheckSteps) * _deltaTime * 100;
		_movement.y = sinf(angleRad) * (this->speed / logicCheckSteps) * _deltaTime * 100;

		this->position += _movement;

		_positionChecks.push_back(this->position);
	}
	this->sprite.setPosition(this->position);

	sf::Vector2f _pos = this->position;
	if (_pos.x < -20 || _pos.x > 500 || _pos.y < -50 || _pos.y > 320)
		this->despawn = true;

	return _positionChecks;
}

float Bullet::GetRadRotation() const
{
	return this->angleRad;
}

bool Bullet::CheckDespawn() const
{
	return this->despawn;
}

void Bullet::Animate()
{
	animFrame++;

	if (this->animFrame > this->animFrameCount)
		animFrame = 1;

	this->sprite.setTextureRect(sf::IntRect((width * (animFrame - 1)), 0, width, height));
}

sf::Vector2i Bullet::GetHitbox() const
{
	return sf::Vector2i(hitboxSize, hitboxSize);
}

Bullet::Owner Bullet::GetOwner() const
{
	return  this->owner;
}

bool Bullet::CheckIfIdHit(int _id, float _deltaTime)
{
	this->spawnTimer += _deltaTime * 1000;

	if (this->idsHit.find(_id) == idsHit.end())
		return false;

	else if (spawnTimer - this->idsHit.at(_id) >= 200) {
		this->idsHit.at(_id) = spawnTimer;
		return false;
	}
	else
		return true;
}

void Bullet::LowerPen()
{
	this->penetration--;
	if (penetration < 0)
		this->despawn = true;
}

void Bullet::AddId(int _id)
{
	this->idsHit.insert(std::make_pair(_id, spawnTimer));
}

int Bullet::GetDamage() const
{
	return this->damage;
}

void Bullet::UpdateTargetPos(sf::Vector2f _target)
{
	this->targetPos = _target;
}

