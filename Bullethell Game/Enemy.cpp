#include "Enemy.h"

Enemy::Enemy(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, int _width, int _height, sf::Vector2f _position, const sf::Texture* _texture, sf::Vector2<int> _hitbox, sf::Vector2<int> _hitboxOffset) : Unit(_id, _width, _height, _position, _texture, _hitbox, _hitboxOffset)
{
	this->id = _id;
	this->genState = Entry;
}

Enemy::~Enemy() {
	delete this->audioPlayer;

	for (int i = 0; i < this->newBullets.size(); i++)
		delete newBullets[i];

	newBullets.clear();

	if (this->dangerZone != nullptr)
		delete dangerZone;

	if (this->deathThreadPtr != nullptr) {
		terminateDeathThread = true;

		deathThreadPtr.get()->join();
	}
}

void Enemy::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds)
{
	//load generic enemy sounds shared by all

	this->audioPlayer->LoadSound(EnemyDeath, &(_enemySounds.at(EnemyDeath)), 50, 1.15f);
}

void Enemy::TakeDamage(int _dmgValue, bool _wasContact) {

	if (_wasContact && !this->DamageTimerCheck())
		return;

	this->damageHighlightTimer = 0;

	UniqueOnDamageLogic(_wasContact);

	if (hitboxActive && health > 0) {
		this->health -= _dmgValue;
		this->audioPlayer->PlaySoundId(EnemyHit, PlayerPrefs::EFFECTS * 0.1f, 0.10f);
	}

	if (this->health <= 0 && !isDying) {
		this->isDying = true;
		this->hitboxActive = false;
		this->genState = Death;
		this->audioPlayer->PlaySoundId(EnemyDeath, PlayerPrefs::EFFECTS * 0.1f, 0.10f);
		this->drawBlur = false;

		this->deathThreadPtr = std::unique_ptr<std::thread>(new std::thread([this] {this->OnDeathLogic(); }));
	}
}

//used by child classes
void Enemy::AnimateLooping(int _animState)
{
	std::pair<int, int> _currRange = this->animationData.at(_animState);
	int _botFrame = _currRange.first;
	int _topFrame = _currRange.second;

	if (animationFrame >= _botFrame && animationFrame < _topFrame)
		this->animationFrame++;
	else
		this->animationFrame = _botFrame;
}

void Enemy::AnimateStraight(int _animState)
{
	std::pair<int, int> _currRange = this->animationData.at(_animState);
	int _botFrame = _currRange.first;
	int _topFrame = _currRange.second;

	if (animationFrame >= _botFrame && animationFrame < _topFrame)
		this->animationFrame++;
	else if (animationFrame < _topFrame)
		this->animationFrame = _botFrame;
}

void Enemy::SetHealth(int _health)
{
	this->maxHealth = _health;
	this->health = _health;
}

bool Enemy::CheckBlur() const
{
	return this->drawBlur;
}

sf::Vector2f Enemy::GetBlurOffset() const
{
	return this->blurOffset;
}

bool Enemy::AllowDestroy() const
{
	return this->allowDestory;
}

Enemy::EnemyType Enemy::GetType() const
{
	return this->type;
}

int Enemy::GetMoney()
{
	if (this->wasCashed || this->genState != Death)
		return 0;
	else {
		this->wasCashed = true;
		return this->moneyWorth;
	}
}

std::vector<Bullet*> Enemy::GetNewBullets()
{
	std::vector<Bullet*> _bullets = std::move(this->newBullets);
	this->newBullets.clear();
	return _bullets;
}

void Enemy::UpdatePlayerPos(sf::Vector2f _pos)
{
	this->playerPosDiff = playerPos - _pos;
	this->playerPos = _pos;
}

DangerZone* Enemy::GetDangerZonePtr()
{
	return this->dangerZone;
}

bool Enemy::CheckDangerZoneCollision(const sf::Vector2f _hitboxPos, const sf::Vector2i _hitbox)
{
	if (this->dangerZone == nullptr || !this->dangerZone->IsVisible())
		return false;
	else
		return this->dangerZone->CheckCollision(_hitboxPos, _hitbox);
}

int Enemy::GetDangerZoneDamage()
{
	if (dangerZone == nullptr)
		return 0;
	else
		return this->dangerZone->GetDamage();
}

void Enemy::OnDeathLogic()
{
	this->allowDestory = true;
}
