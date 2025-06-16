#include "EnemyGunner.h"

EnemyGunner::EnemyGunner(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty) : Enemy(_id, _enemySounds, 64, 32, _position, _texture, sf::Vector2(44, 17), sf::Vector2(9, 10))
{
	this->type = Gunner;
	this->animState = IdleAnim;
	this->difficultyModifier = _difficulty;
	this->moneyWorth = 250;
	this->contactDamage = 30 * _difficulty;

	//health
	this->SetHealth(135 * _difficulty);

	this->timerModifier = (1.f / _difficulty);

	this->LoadSounds(_enemySounds);

	this->animationData.insert(std::make_pair(IdleAnim, std::make_pair(1, 4)));
	this->animationData.insert(std::make_pair(FiringChargeAnim, std::make_pair(5, 8)));
	this->animationData.insert(std::make_pair(FiringAnim, std::make_pair(9, 12)));
	this->animationData.insert(std::make_pair(InterruptAnim, std::make_pair(13, 16)));
	this->animationData.insert(std::make_pair(FlamethrowerChargeAnim, std::make_pair(17, 20)));
	this->animationData.insert(std::make_pair(FlamethrowerAnim, std::make_pair(21, 24)));
	this->animationData.insert(std::make_pair(ExplosionAnim, std::make_pair(25, 29)));
	this->animationData.insert(std::make_pair(Nothing, std::make_pair(30, 30)));
}

void EnemyGunner::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds)
{
	//load gereric sounds
	Enemy::LoadSounds(_enemySounds);

	this->audioPlayer->LoadSound(EnemyHit, &(_enemySounds.at(DroneHit)), 40, 1.1f);
	this->audioPlayer->LoadSound(GunnerDeploy, &(_enemySounds.at(GunnerDeploy)), 40, 1.5f);
	this->audioPlayer->LoadSound(GunnerFire, &(_enemySounds.at(GunnerFire)), 25, 0.75f);
	this->audioPlayer->LoadSound(GunnerInterrupt, &(_enemySounds.at(GunnerInterrupt)), 35, 1.45f);
	this->audioPlayer->LoadSound(GunnerFlamethrower, &(_enemySounds.at(GunnerFlamethrower)), 90, 1.f);
}

void EnemyGunner::LoadDangerZone(const sf::Texture* _texture) {

	this->dangerZone = new DangerZone(DangerZone::Flame, 112, 32, this->position, _texture, this->difficultyModifier);
}

void EnemyGunner::OnUpdate(float _deltaTime)
{
	this->deltaTime = _deltaTime;
	this->startFiringTimer += _deltaTime * 1000;
	this->changeStateTimer += _deltaTime * 1000;
	this->firerateTimer += _deltaTime * 1000;
	this->UpdateUnitTimers(_deltaTime);

	switch (this->genState) {
	case (Entry):
		Entrance(_deltaTime);
		break;
	case (Logic):
		if (this->interruptionCount >= std::clamp(interruptThreshold / difficultyModifier, 3.f, (float)interruptThreshold)) {

			this->interruptionCount = 0;
			this->animState = FlamethrowerChargeAnim;
			this->uniqueLogicState = FlamethrowerCharge;
			this->targetPos = sf::Vector2f(playerPos.x + 30, playerPos.y);

			this->acc = sf::Vector2f(0, 0);
			this->moveDistance = sf::Vector2f(abs(this->position.x - targetPos.x), abs(this->position.y - targetPos.y));
			this->drawBlur = true;
			this->changeStateTimer = 0;
			this->audioPlayer->PlaySoundId(GunnerFlamethrower, PlayerPrefs::EFFECTS * 0.1f);
		}

		switch (this->uniqueLogicState) {
		case(Wander):
			this->CalculateAcc(_deltaTime, 1);
			if (this->moveDistance.x <= 0.f && this->moveDistance.y <= 0.f && abs(this->acc.x) < 0.1f && abs(this->acc.y) < 0.1f) {
				this->acc.x = 0;
				this->acc.y = 0;
				this->FindNewWanderPos();
			}

			if (this->CheckFireTimer()) {
				this->moveDistance = sf::Vector2f(0, 0);
				this->animState = FiringChargeAnim;
				this->uniqueLogicState = FiringCharge;
				this->changeStateTimer = 0;
				this->audioPlayer->PlaySoundId(GunnerDeploy, PlayerPrefs::EFFECTS * 0.1f);
			}
			break;

		case(FiringCharge):
			this->CalculateAcc(_deltaTime, 1);
			if (changeStateTimer >= 500) {
				this->stopFiringHpThreshold = this->health - (int)this->maxHealth * 0.125f;
				currFirerateTime = 0;
				this->firerateTimer = 0;
				this->animState = FiringAnim;
				this->uniqueLogicState = Firing;
			}
			break;

		case(Firing):
			this->CalculateAcc(_deltaTime, 1);
			if (this->moveDistance.y <= 0 && abs(this->acc.y) < 0.0001f) {
				this->FindNewFireFloatPos();
			}

			this->FireBullets();
			break;

		case(Interrupt):
			this->CalculateAcc(_deltaTime, 1);
			if (changeStateTimer >= 500) {
				this->SetFireTimer();
				this->animState = IdleAnim;
				this->uniqueLogicState = Wander;
			}
			break;
		case(FlamethrowerCharge):
			this->CalculateAcc(_deltaTime, 2 * difficultyModifier);
			this->blurOffset = sf::Vector2f(-acc.x * 2, -acc.y * 2);
			if (changeStateTimer >= 500) {
				this->animState = FlamethrowerAnim;
				this->uniqueLogicState = Flamethrower;
				this->dangerZone->Activate(true);
			}
			break;

		case(Flamethrower):
			this->CalculateAcc(_deltaTime, 3.25f * difficultyModifier);

			this->blurOffset = sf::Vector2f(-acc.x * 2, -acc.y * 2);

			if (changeStateTimer >= 1000 && abs(acc.x) < 0.3f && abs(acc.y) < 0.3f) {
				this->drawBlur = false;
				this->uniqueLogicState = GoBack;
				this->animState = IdleAnim;

				this->targetPos = sf::Vector2f(300, this->position.y);
				this->moveDistance = sf::Vector2f(abs(this->position.x - targetPos.x), abs(this->position.y - targetPos.y));
				this->dangerZone->Activate(false);
				this->audioPlayer->StopSound(GunnerFlamethrower);
			}
			break;
		case(GoBack):
			this->CalculateAcc(_deltaTime, 1.45 * difficultyModifier);

			if (this->position.x >= 300)
				this->moveDistance = sf::Vector2f(0, 0);

			if (this->position.x >= 300 && abs(acc.x) < 0.2f) {
				this->SetFireTimer();
				this->uniqueLogicState = Wander;
				interruptThreshold += 2;
			}
			break;
		default:
			break;
		}
		break;
	case (Death):
		this->animState = ExplosionAnim;
		break;
	}

	this->UpdatePosition(_deltaTime);

	if (dangerZone != nullptr) {
		dangerZone->UpdatePosition(this->position + sf::Vector2f(7, 22));
	}
}

void EnemyGunner::Entrance(float _deltaTime)
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());

	if (!this->entranceSet) {
		std::uniform_int_distribution<std::mt19937::result_type> _posxDist(336, 432);
		this->entrancePos.x = _posxDist(_rng);
		this->entranceSet = true;
		this->drawBlur = true;
	}

	if (entrancePos.x < this->fixedPosition.x) {
		acc.x -= this->speed * _deltaTime * 2;
		this->acc.x = std::clamp(acc.x, -accLimit / 2.5f, 0.f);
		this->blurOffset = sf::Vector2f(-2.5f * acc.x, 0);
	}
	else {
		this->SetFireTimer();
		this->genState = Logic;
		this->uniqueLogicState = Wander;
		this->drawBlur = false;
	}
}

void EnemyGunner::FindNewWanderPos()
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _posxDist(336, 420);
	if (this->fixedHitboxPos.y < 135.f) {
		std::uniform_int_distribution<std::mt19937::result_type> _posyDist(208, 240);
		this->targetPos = sf::Vector2f(_posxDist(_rng), _posyDist(_rng));
	}
	else {
		std::uniform_int_distribution<std::mt19937::result_type> _posyDist(0, 32);
		this->targetPos = sf::Vector2f(_posxDist(_rng), _posyDist(_rng));
	}

	this->moveDistance = sf::Vector2f(abs(this->position.x - targetPos.x), abs(this->position.y - targetPos.y));
}

void EnemyGunner::FindNewFireFloatPos() {
	if (this->floatDir == NotSet) {
		std::random_device _dev;
		std::mt19937 _rng(_dev());
		std::uniform_int_distribution<std::mt19937::result_type> _boolDist(1, 2);

		this->floatDir = (DirectionFloat)_boolDist(_rng);
	}

	switch (floatDir) {
	case Up:
		this->targetPos.y = 0;
		floatDir = Down;
		break;

	case Down:
		this->targetPos.y = 270;
		floatDir = Up;
		break;
	}
	this->moveDistance.y = 20;
}

void EnemyGunner::UpdatePosition(float _deltaTime)
{
	float _rad = atan2f(moveDistance.y, moveDistance.x);
	float _yModif = abs(sin(_rad));
	float _xModif = abs(cos(_rad));

	float _xChange = this->acc.x * _deltaTime * 100;
	float _yChange = this->acc.y * _deltaTime * 100;


	this->position.x += _xChange;
	this->position.y += _yChange;
	if (this->genState != Entry) {
		this->position.x = std::clamp(position.x, 0.f - this->hitboxOffset.x, 480.f - hitboxOffset.x - (float)hitbox.x);
		this->position.y = std::clamp(position.y, 0.f - this->hitboxOffset.y, 270.f - hitboxOffset.y - (float)hitbox.y);
	}

	this->moveDistance.x -= abs(_xChange);
	this->moveDistance.y -= abs(_yChange);

	Unit::UpdatePosition(_deltaTime);
}

void EnemyGunner::CalculateAcc(float _deltaTime, float _accModifier)
{
	float _rad = atan2f(moveDistance.y, moveDistance.x);

	float _yModif = 1.f;
	float _xModif = 1.f;
	if (this->uniqueLogicState != Firing) {
		_yModif = abs(sin(_rad)) * _accModifier;
		_xModif = abs(cos(_rad)) * _accModifier;
	}


	if (this->moveDistance.y > 0) {
		this->acc.y += this->GetSign(targetPos.y - this->position.y) * speed * _deltaTime * _yModif;
		this->acc.y = std::clamp(this->acc.y, -accLimit * _yModif, accLimit * _yModif);
	}
	else {
		if (acc.y >= 0) {
			acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 1.75f * _yModif;
			this->acc.y = std::clamp(acc.y, 0.f, accLimit * _yModif);
		}
		else {
			acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 1.75f * _yModif;
			this->acc.y = std::clamp(acc.y, -accLimit * _yModif, 0.f);
		}
	}

	if (this->moveDistance.x > 0) {
		this->acc.x += this->GetSign(targetPos.x - this->position.x) * speed * _deltaTime * _xModif;
		this->acc.x = std::clamp(this->acc.x, -accLimit * _xModif, accLimit * _xModif);
	}
	else {
		if (acc.x >= 0) {
			acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 1.75f * _xModif;
			this->acc.x = std::clamp(acc.x, 0.f, accLimit * _xModif);
		}
		else {
			acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 1.75f * _xModif;
			this->acc.x = std::clamp(acc.x, -accLimit * _xModif, 0.f);
		}
	}

	if (this->uniqueLogicState == Wander) {
		acc.y = std::clamp(acc.y, -accLimit * _yModif / 3.5f, accLimit * _yModif / 3.5f);
		acc.x = std::clamp(acc.x, -accLimit * _xModif / 3.5f, accLimit * _xModif / 3.5f);
	}
}

void EnemyGunner::Animate()
{
	if (this->animState > animationData.size())
		return;

	//animation one way switching
	if (this->animState == ExplosionAnim && this->animationFrame == animationData.at(ExplosionAnim).second) {
		this->animState = Nothing;
		this->animationFrame = animationData.at(Nothing).first;
	}

	//looping
	if (this->animState == IdleAnim || this->animState == FiringAnim || this->animState == FlamethrowerAnim) {
		this->AnimateLooping(animState);
	}

	//single instance animationss
	else if (this->animState == FiringChargeAnim || this->animState == FlamethrowerChargeAnim || this->animState == ExplosionAnim || this->animState == InterruptAnim)
		this->AnimateStraight(animState);

	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));

	if (this->dangerZone != nullptr && this->dangerZone->IsVisible())
		this->dangerZone->Animate();
}

void EnemyGunner::FireDeathSalvo()
{
	Bullet* _bll;
	for (int _angle = 0; _angle <= 360; _angle += 30) {
		_bll = new Bullet(5, 5, this->fixedHitboxPos, Bullet::GunnerB);
		_bll->LoadData(_bll->GetPosition(), _angle, true);
		this->newBullets.push_back(std::move(_bll));
	}
	this->deathSalvoFired = true;
}


void EnemyGunner::OnDeathLogic()
{
	this->acc = sf::Vector2f(0, 0);
	this->audioPlayer->StopSound(GunnerDeploy);
	this->audioPlayer->StopSound(GunnerInterrupt);
	this->audioPlayer->StopSound(GunnerFlamethrower);

	this->dangerZone->Activate(false);

	std::chrono::steady_clock::time_point _timeZero = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point _currTime;
	float _time = 0;

	this->FireDeathSalvo();

	do {
		_currTime = std::chrono::high_resolution_clock::now();
		_time = (float)std::chrono::duration_cast<std::chrono::milliseconds>(_currTime - _timeZero).count();
	} while ((!this->terminateDeathThread && _time <= 2000) || !this->deathSalvoFired);

	this->allowDestory = true;
}

void EnemyGunner::UniqueOnDamageLogic(bool _wasContact)
{
	this->acc.x *= 0.33f;
	this->acc.y *= 0.33f;

	if (this->uniqueLogicState == Wander) {
		PostponeFireOnDamage();
	}
	else if (this->uniqueLogicState == FiringCharge) {
		this->uniqueLogicState = Wander;
		this->animState = IdleAnim;
		SetFireTimer();
		this->interruptionCount += 3;
		this->audioPlayer->StopSound(GunnerDeploy);
	}
	else if (this->uniqueLogicState == Firing) {
		if (health > 0 && health < stopFiringHpThreshold) {
			this->changeStateTimer = 0;
			this->interruptionCount += 5;
			this->moveDistance = sf::Vector2f(0, 0);
			this->uniqueLogicState = Interrupt;
			this->animState = InterruptAnim;
			this->audioPlayer->StopSound(GunnerDeploy);
			this->audioPlayer->PlaySoundId(GunnerInterrupt, PlayerPrefs::EFFECTS * 0.1f);
		}
	}
}

void EnemyGunner::SetFireTimer()
{
	this->startFiringTimer = 0;

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _time(3500 * this->timerModifier, 7000 * this->timerModifier);
	this->timeToFireMs = _time(_rng);
}

void EnemyGunner::PostponeFireOnDamage()
{
	if (startFiringTimer > 4500.f * this->timerModifier) {
		this->SetFireTimer();
		this->interruptionCount += 2;
	}
}

bool EnemyGunner::CheckFireTimer() const
{
	if (startFiringTimer >= timeToFireMs) {
		return true;
	}
	return false;
}

sf::Vector2f EnemyGunner::CalculateBulletTargetPos(float _deltaTime) {
	sf::Vector2f _playerSpeed = this->playerPosDiff / _deltaTime;
	float _bulletSpeed = 0.4f * this->difficultyModifier * 500;
	sf::Vector2f _distanceVec = this->playerPos - (this->position + sf::Vector2f(7, 21));
	float _distance = sqrt(pow(_distanceVec.x, 2) + pow(_distanceVec.y, 2));

	float _bulletTime = _distance / _bulletSpeed;
	sf::Vector2f _estimatedTarget = this->playerPos - (_playerSpeed * _bulletTime);

	_estimatedTarget.x = std::clamp(_estimatedTarget.x, 8.f, 472.f);
	_estimatedTarget.y = std::clamp(_estimatedTarget.y, 8.f, 262.f);

	return _estimatedTarget;
}

void  EnemyGunner::FireBullets() {
	this->currFirerateTime += deltaTime * 1000;

	if (currFirerateTime > this->timeBetweenShots) {
		while (currFirerateTime >= timeBetweenShots)
			currFirerateTime -= timeBetweenShots;

		Bullet* _bll = new Bullet(5, 5, sf::Vector2f(this->position.x + 5, this->position.y + 19), Bullet::GunnerB);
		_bll->LoadData(_bll->GetPosition(), CalculateBulletTargetPos(this->deltaTime), true);
		this->newBullets.push_back(std::move(_bll));

		this->audioPlayer->PlaySoundId(GunnerFire, PlayerPrefs::EFFECTS * 0.1f, 0.1f);
	}
}