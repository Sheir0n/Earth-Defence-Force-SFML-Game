#include "EnemyDrone.h"
EnemyDrone::EnemyDrone(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty) : Enemy(_id, _enemySounds, 64, 32, _position, _texture, sf::Vector2(31, 18), sf::Vector2(17, 7))
{
	this->type = Drone;
	this->animState = Idle;
	this->difficultyModifier = _difficulty;
	this->moneyWorth = 100;
	this->contactDamage = 20 * _difficulty;

	//health
	this->SetHealth(50 * _difficulty);

	this->timerModifier = (1.f / (_difficulty * 3));
	LoadSounds(_enemySounds);


	this->animationData.insert(std::make_pair(Idle, std::make_pair(1, 4)));
	this->animationData.insert(std::make_pair(Powerup, std::make_pair(5, 7)));
	this->animationData.insert(std::make_pair(Charge, std::make_pair(8, 11)));
	this->animationData.insert(std::make_pair(Explosion, std::make_pair(12, 16)));
	this->animationData.insert(std::make_pair(ExplosionRam, std::make_pair(17, 21)));
	this->animationData.insert(std::make_pair(Nothing, std::make_pair(22, 22)));
}

void EnemyDrone::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds)
{
	//load gereric sounds
	//example hit sound
	Enemy::LoadSounds(_enemySounds);

	//load unique sound here
	this->audioPlayer->LoadSound(EnemyHit, &(_enemySounds.at(DroneHit)), 40, 1.32f);
	this->audioPlayer->LoadSound(DroneFire, &(_enemySounds.at(DroneFire)), 45, 1.f);
	this->audioPlayer->LoadSound(DroneRam, &(_enemySounds.at(DroneRam)), 45, 1.f);
}

void EnemyDrone::OnUpdate(float _deltaTime)
{
	this->moveCooldownTimer += _deltaTime * 1000;
	this->targetingTimer += _deltaTime * 1000;
	this->UpdateUnitTimers(_deltaTime);

	switch (this->genState) {
	case (Entry):
		Entrance(_deltaTime);
		break;
	case (Logic):
		//also fire bullet after new pos selected
		switch (uniqueLogicState) {
		case Regular:
			SelectNextPos();
			CalculateAcc(_deltaTime);

			//check if switch to targeting player state
			//depending on chance and difficulty modifier
			if (this->health < this->maxHealth * 0.45f && !this->rolled)
				if (this->RollToRam()) {
					this->uniqueLogicState = Targeting;
					this->animState = Powerup;
					this->moveDistance = 0;
					this->ramTargetPos = this->playerPos;
					targetingTimer = 0;
					this->audioPlayer->PlaySoundId(DroneRam, PlayerPrefs::EFFECTS * 0.1f);
				}
			break;
		case Targeting:
			CalculateAcc(_deltaTime);
			TargetingRotation(_deltaTime);

			if (targetingTimer > 375) {
				this->uniqueLogicState = Ramming;
				this->animState = Charge;
				this->hitbox = sf::Vector2i(29, 25);
				this->hitboxOffset = sf::Vector2i(19, 3);
				this->contactDamage *= 3.5;
				this->drawBlur = true;
			}
			break;
		case Ramming:
			RammingAcc(_deltaTime);
			break;
		}
		break;
	case (Death):
		this->audioPlayer->StopSound(DroneRam);

		if (this->uniqueLogicState == Ramming)
			this->animState = ExplosionRam;
		else
			this->animState = Explosion;
		break;
	}

	this->UpdatePosition(_deltaTime);
}

void EnemyDrone::FireBullets()
{
	Bullet* _bll = new Bullet(7, 5, sf::Vector2f(this->fixedPosition.x - 15, this->fixedPosition.y - 7), Bullet::DroneB);
	_bll->LoadData(_bll->GetPosition(), 180, true);
	this->newBullets.push_back(std::move(_bll));

	_bll = new Bullet(7, 5, sf::Vector2f(this->fixedPosition.x - 15, this->fixedPosition.y + 7), Bullet::DroneB);
	_bll->LoadData(_bll->GetPosition(), 180, true);
	this->newBullets.push_back(std::move(_bll));

	this->audioPlayer->PlaySoundId(DroneFire, PlayerPrefs::EFFECTS * 0.1f, 0.1f);
}

void EnemyDrone::UniqueOnDamageLogic(bool _wasContact)
{
	if (this->uniqueLogicState != Ramming) {
		this->acc.x *= 0.5f;
		this->acc.y *= 0.5f;
	}
	else {
		this->acc.x *= 0.95f;
		this->acc.y *= 0.95f;
	}

	if (this->uniqueLogicState == Ramming && _wasContact)
		this->health = 0;
}

void EnemyDrone::CalculateAcc(float _deltaTime)
{
	if (this->moveDistance > 0) {
		this->acc.y += this->GetSign(targetPos - this->position.y) * this->speed * _deltaTime;
		this->acc.y = std::clamp(this->acc.y, -accLimit, accLimit);
	}
	else {
		if (acc.y >= 0) {
			acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 2.f;
			this->acc.y = std::clamp(acc.y, 0.f, accLimit);
		}
		else {
			acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 2.f;
			this->acc.y = std::clamp(acc.y, -accLimit, 0.f);
		}
	}

	if (acc.x >= 0) {
		acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 2.f;
		this->acc.x = std::clamp(acc.x, 0.f, accLimit);
	}
	else {
		acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 2.f;
		this->acc.x = std::clamp(acc.x, -accLimit, 0.f);
	}
}


void EnemyDrone::Entrance(float _deltaTime)
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());

	if (!this->entranceSet) {
		std::uniform_int_distribution<std::mt19937::result_type> _posxDist(290, 470);
		this->entrancePos.x = _posxDist(_rng);
		this->entranceSet = true;
		this->drawBlur = true;
	}

	if (entrancePos.x < this->fixedPosition.x) {
		acc.x -= this->speed * _deltaTime * 0.55;
		this->acc.x = std::clamp(acc.x, -accLimit / 1.5f, 0.f);
		this->blurOffset = sf::Vector2f(-3.f * acc.x, 0);
	}
	else {
		this->genState = Logic;
		this->uniqueLogicState = Regular;

		//calculate new random movement cooldown
		this->moveCooldownTimer = 0;
		std::uniform_int_distribution<std::mt19937::result_type> _randDelay(1000 * timerModifier, 2500 * timerModifier);
		this->moveCooldownInMs = _randDelay(_rng);
		this->drawBlur = false;
	}
}

bool EnemyDrone::RollToRam()
{
	if (this->rolled)
		return false;

	this->rolled = true;
	const int _treshold = 30;

	std::random_device _dev;
	std::mt19937 _rng(_dev());

	std::uniform_int_distribution<std::mt19937::result_type> _rollDist(0, 100);
	int _roll = _rollDist(_rng);
	if (_roll <= _treshold * pow(this->difficultyModifier, 3)) {
		return true;
	}
	else
		return false;
}

void EnemyDrone::TargetingRotation(float _deltaTime)
{
	float _rad = atan2f(this->fixedHitboxPos.y - this->ramTargetPos.y, this->fixedHitboxPos.x - this->ramTargetPos.x);
	float _targetRotation = _rad * (180.f / 3.14159);

	while (this->currRotation >= 180)
		this->currRotation -= 360;
	while (this->currRotation <= -180)
		this->currRotation += 360;

	this->rotationModifier += 50.f * _deltaTime;

	float _difference = _targetRotation - this->currRotation;
	if (abs(_difference) <= 180)
		this->currRotation += _difference * _deltaTime * rotationModifier;
	else if (_difference >= 0)
		this->currRotation -= (360.f - _difference) * _deltaTime * rotationModifier;
	else
		this->currRotation += (360.f + _difference) * _deltaTime * rotationModifier;

	this->sprite.setRotation(this->currRotation);
}

void EnemyDrone::RammingAcc(float _deltaTime) {
	float _yModif = sin(this->currRotation * 3.14159 / 180) * this->speed * 1.6f;
	float _xModif = cos(this->currRotation * 3.14159 / 180) * this->speed * 1.6f;

	this->acc.x -= _xModif * _deltaTime * difficultyModifier;
	this->acc.y -= _yModif * _deltaTime * difficultyModifier;

	this->acc.x = std::clamp(acc.x, (-this->accLimit) * abs(_xModif), this->accLimit * abs(_xModif));
	this->acc.y = std::clamp(acc.y, (-this->accLimit) * abs(_yModif), this->accLimit * abs(_yModif));

	this->blurOffset = sf::Vector2f(-2.5f * acc.x, -2.5f * acc.y);
}


void EnemyDrone::SelectNextPos()
{
	if (this->moveDistance <= 0) {
		if (startCooldownTimer) {
			this->moveCooldownTimer = 0;
			startCooldownTimer = false;
		}

		if (this->moveCooldownTimer > this->moveCooldownInMs) {
			std::random_device _dev;
			std::mt19937 _rng(_dev());

			int _clampedY = 254 - hitboxOffset.y - hitbox.y;

			std::uniform_int_distribution<std::mt19937::result_type> _posyDist(std::clamp((int)this->position.y - 133, 16, _clampedY), std::clamp((int)this->position.y + 133, 48, _clampedY));

			float _distance = 0;
			int _newPos;
			do {
				_newPos = _posyDist(_rng);
				_distance = abs(this->position.y - _newPos);
			} while (_distance <= 33);

			this->targetPos = _newPos;
			this->moveDistance = _distance;

			startCooldownTimer = true;

			//random delay next time
			std::uniform_int_distribution<std::mt19937::result_type> _randDelay(1500 * timerModifier, 4000 * timerModifier);
			this->moveCooldownInMs = _randDelay(_rng);


			//fire a bullet
			this->FireBullets();
		}
	}
}


void EnemyDrone::UpdatePosition(float _deltaTime)
{
	float _xChange = this->acc.x * _deltaTime * 100;
	float _yChange = this->acc.y * _deltaTime * 100;

	this->position.x += _xChange;
	this->position.y += _yChange;
	if (this->genState != Entry && this->uniqueLogicState != Ramming) {
		this->position.x = std::clamp(position.x, 192.f - this->hitboxOffset.x, 480.f - hitboxOffset.x - (float)hitbox.x);
		this->position.y = std::clamp(position.y, 0.f - this->hitboxOffset.y, 270.f - hitboxOffset.y - (float)hitbox.y);
	}
	this->moveDistance -= sqrt(pow(_xChange, 2) + pow(_yChange, 2));

	Unit::UpdatePosition(_deltaTime);

	//delete if out of bounds
	if (this->genState != Entry && (this->fixedHitboxPos.x < -250 || this->fixedHitboxPos.x > 730 || this->fixedHitboxPos.y < -250 || this->fixedHitboxPos.y > 520)) {
		this->wasCashed = true;
		this->allowDestory = true;
	}
}

void EnemyDrone::Animate()
{
	if (this->animState > animationData.size())
		return;

	//animation one way switching
	if (this->animState == Explosion && this->animationFrame == animationData.at(Explosion).second || this->animState == ExplosionRam && this->animationFrame == animationData.at(ExplosionRam).second) {
		this->animState = Nothing;
		this->animationFrame = animationData.at(Nothing).first;
	}

	//looping
	if (this->animState == Idle || this->animState == Charge) {
		this->AnimateLooping(animState);
	}
	//single instance animationss
	else if (this->animState == Powerup || this->animState == Explosion || this->animState == ExplosionRam)
		this->AnimateStraight(animState);

	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));
}

void EnemyDrone::OnDeathLogic()
{
	this->acc = sf::Vector2f(0, 0);

	std::chrono::steady_clock::time_point _timeZero = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point _currTime;
	float _time = 0;

	do {
		_currTime = std::chrono::high_resolution_clock::now();
		_time = (float)std::chrono::duration_cast<std::chrono::milliseconds>(_currTime - _timeZero).count();
	} while (!this->terminateDeathThread && _time <= 2000);

	this->allowDestory = true;
}
