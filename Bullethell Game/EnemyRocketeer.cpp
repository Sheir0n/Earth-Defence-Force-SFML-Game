#include "EnemyRocketeer.h"

EnemyRocketeer::EnemyRocketeer(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty) : Enemy(_id, _enemySounds, 64, 32, _position, _texture, sf::Vector2(39, 16), sf::Vector2(13, 10))
{
	this->type = Rocketeer;
	this->animState = IdleAnim;
	this->difficultyModifier = _difficulty;
	this->moneyWorth = 125;
	this->contactDamage = 18 * _difficulty;
	this->moveCooldownInMs /= _difficulty;
	this->dodgeCooldown = std::clamp(dodgeCooldown /= _difficulty, 1500, 4000);

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _timerDist(500 * this->difficultyModifier, 2000 * this->difficultyModifier);
	this->moveCooldownInMs = _timerDist(_rng);


	std::uniform_int_distribution<std::mt19937::result_type> _moveDist(3 / _difficulty, 5 / _difficulty);
	this->fireMoveThreshold = _moveDist(_rng);

	//health
	this->SetHealth(35 * _difficulty);

	this->LoadSounds(_enemySounds);

	this->animationData.insert(std::make_pair(IdleAnim, std::make_pair(1, 6)));
	this->animationData.insert(std::make_pair(RocketfireAnim, std::make_pair(7, 24)));
	this->animationData.insert(std::make_pair(GunfireAnim, std::make_pair(25, 30)));
	this->animationData.insert(std::make_pair(ExplosionAnim, std::make_pair(31, 35)));
	this->animationData.insert(std::make_pair(Nothing, std::make_pair(36, 36)));
}

void EnemyRocketeer::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds)
{
	//load gereric sounds
	//example hit sound
	Enemy::LoadSounds(_enemySounds);

	//load unique sound here
	this->audioPlayer->LoadSound(EnemyHit, &(_enemySounds.at(DroneHit)), 40, 1.45f);
	this->audioPlayer->LoadSound(RocketeerDodge, &(_enemySounds.at(RocketeerDodge)), 100, 1.2f);
	this->audioPlayer->LoadSound(RocketeerGunfire, &(_enemySounds.at(RocketeerGunfire)), 30, 1.5f);
	this->audioPlayer->LoadSound(RocketeerRocketfire, &(_enemySounds.at(RocketeerRocketfire)), 50, 1.5f);
}

void EnemyRocketeer::OnUpdate(float _deltaTime)
{
	this->changeStateTimer += _deltaTime * 1000;
	this->dodgeCooldownTimer += _deltaTime * 1000;
	this->moveCooldownTimer += _deltaTime * 1000;
	this->UpdateUnitTimers(_deltaTime);

	switch (this->genState) {
	case (Entry):
		Entrance(_deltaTime);
		break;
	case (Logic):
		switch (this->uniqueLogicState) {
		case(Idle):
		{
			UniqueLogicStates _nextState = this->SetFireState();

			switch (_nextState) {
			case Idle:
				this->FindNewIdlePos();
				this->CalculateAcc(_deltaTime, 1);
				break;
			case Gunfire:
				this->fireTime = 250;
				this->bulletCount = 0;
				this->changeStateTimer = 0;
				this->RandomiseParity();
				this->animState = GunfireAnim;
				this->uniqueLogicState = Gunfire;
				break;
			case Rocketfire:
				this->fireTime = 250;
				this->bulletCount = 0;
				this->changeStateTimer = 0;
				this->RandomiseParity();
				this->animState = RocketfireAnim;
				this->uniqueLogicState = Rocketfire;
				break;
			}
			break;
		}
		case(Dodge):
			this->CalculateDodgeAcc();
			this->blurOffset = sf::Vector2f(0, -acc.y * 2);

			if (this->moveDistance.y <= 0) {
				this->drawBlur = false;
				this->dodgeCooldownTimer = 0;
				this->startedMoveTimer = false;
				this->moveCooldownInMs /= 4;

				this->hitboxActive = true;
				this->uniqueLogicState = Idle;
			}
			break;
		case(Gunfire):
			this->CalculateAcc(_deltaTime, 0.5);

			this->fireTime += _deltaTime * 1000;
			if (fireTime >= 250 && this->bulletCount < 3) {
				fireTime -= 250;
				FireBullets();
				this->bulletCount++;
			}

			if (this->changeStateTimer >= 750) {
				this->startedMoveTimer = false;
				this->moveCooldownInMs /= 2;
				this->animState = IdleAnim;
				this->uniqueLogicState = Idle;
			}

			break;
		case(Rocketfire):
			this->CalculateAcc(_deltaTime, 0.5);

			if (this->changeStateTimer >= 850 && this->changeStateTimer <= 2100) {
				this->fireTime += _deltaTime * 1000;
				if (fireTime >= 250 && this->bulletCount < 5) {
					fireTime -= 250;
					FireBullets();
					this->bulletCount++;
					this->audioPlayer->PlaySoundId(RocketeerRocketfire, PlayerPrefs::EFFECTS * 0.1f, 0.1f);
				}
			}

			if (this->changeStateTimer >= 2250) {
				this->startedMoveTimer = false;
				this->moveCooldownInMs /= 2;
				this->animState = IdleAnim;
				this->uniqueLogicState = Idle;
			}
			break;
		}
		break;
	case (Death):
		this->animState = ExplosionAnim;
		break;
	}

	this->UpdatePosition(_deltaTime);
}

void EnemyRocketeer::Entrance(float _deltaTime)
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());

	if (!this->entranceSet) {
		std::uniform_int_distribution<std::mt19937::result_type> _posxDist(273, 432);
		this->entrancePos.x = _posxDist(_rng);
		this->entranceSet = true;
		this->drawBlur = true;
	}

	if (entrancePos.x < this->fixedPosition.x) {
		acc.x -= this->speed * _deltaTime * 4;
		this->acc.x = std::clamp(acc.x, -accLimit * 1.25f, 0.f);
		this->blurOffset = sf::Vector2f(-2.5f * acc.x, 0);
	}
	else {
		this->genState = Logic;
		this->uniqueLogicState = Idle;
		this->drawBlur = false;
	}
}

void EnemyRocketeer::CalculateAcc(float _deltaTime, float _accModifier)
{
	float _rad = atan2f(moveDistance.y, moveDistance.x);
	float _yModif = abs(sin(_rad)) * _accModifier;
	float _xModif = abs(cos(_rad)) * _accModifier;

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
}

void EnemyRocketeer::CalculateDodgeAcc()
{
	this->acc.y = this->GetSign(targetPos.y - this->position.y) * accLimit * 2;
}

void EnemyRocketeer::UpdatePosition(float _deltaTime)
{
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

void EnemyRocketeer::FindNewIdlePos()
{
	if (this->moveDistance.x <= 0 && this->moveDistance.y <= 0 && abs(acc.x) < 0.1f && abs(acc.y) < 0.1f && !startedMoveTimer) {
		this->moveCooldownTimer = 0;
		this->startedMoveTimer = true;
	}

	if (this->moveCooldownTimer > this->moveCooldownInMs && startedMoveTimer) {
		this->startedMoveTimer = false;

		std::random_device _dev;
		std::mt19937 _rng(_dev());
		std::uniform_int_distribution<std::mt19937::result_type> _posXDist(210, 400);
		std::uniform_int_distribution<std::mt19937::result_type> _posYDist(34, 240);
		std::uniform_int_distribution<std::mt19937::result_type> _distanceDist(150, 250);

		do {
			this->targetPos = sf::Vector2f(_posXDist(_rng), _posYDist(_rng));
			this->moveDistance = sf::Vector2f(abs(this->position.x - targetPos.x), abs(this->position.y - targetPos.y));

			int _distance = _distanceDist(_rng);
			if (moveDistance.x > 0 && moveDistance.y > 0);
			_distance *= 0.707f;

			this->moveDistance = sf::Vector2f(std::clamp((int)moveDistance.x, 0, _distance), std::clamp((int)moveDistance.y, 0, _distance));
		} while (sqrt(pow(this->moveDistance.x, 2) + pow(this->moveDistance.y, 2) < 50));

		std::uniform_int_distribution<std::mt19937::result_type> _timerDist(500 * this->difficultyModifier, 2000 * this->difficultyModifier);
		this->moveCooldownInMs = _timerDist(_rng);
		this->fireMoveCount++;
	}
}

void EnemyRocketeer::FindDodgePos()
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _directionDist(0, 1);

	this->moveDistance.y = 50;

	if (_directionDist(_rng) == 0) {
		this->targetPos.y = this->position.y + moveDistance.y;

		if (this->targetPos.y > 220) {
			this->targetPos.y = this->position.y - moveDistance.y;
			this->targetPos.y = 0;
		}
		else
			this->targetPos.y = 270;
	}
	else {
		this->targetPos.y = this->position.y - moveDistance.y;

		if (this->targetPos.y < 54) {
			this->targetPos.y = this->position.y + moveDistance.y;
			this->targetPos.y = 270;
		}
		else
			this->targetPos.y = 0;
	}

	this->fireMoveCount += 2;
}

bool EnemyRocketeer::EnableDodgeInDanger(sf::Vector2f _pos)
{
	if (sqrt(pow(this->fixedHitboxPos.x - _pos.x, 2) + pow(this->fixedHitboxPos.y - _pos.y, 2)) <= 40 && this->uniqueLogicState != Dodge && (this->genState == Logic || (this->genState == Entry && this->fixedHitboxPos.x < 433))) {

		if (this->dodgeCooldownTimer > this->dodgeCooldown || firstTimeDodge) {
			if (this->genState == Entry)
				this->genState = Logic;

			this->firstTimeDodge = false;
			this->animState = IdleAnim;
			this->uniqueLogicState = Dodge;
			this->drawBlur = true;
			this->acc = sf::Vector2f(0, 0);
			this->moveDistance = sf::Vector2f(0, 0);
			this->targetPos = this->position;
			this->hitboxActive = false;

			this->audioPlayer->PlaySoundId(RocketeerDodge, PlayerPrefs::EFFECTS * 0.1f, 0.1f);
			this->FindDodgePos();
			return false;
		}
	}

	return false;
}

EnemyRocketeer::UniqueLogicStates EnemyRocketeer::SetFireState()
{
	if (this->fireMoveCount >= this->fireMoveThreshold && this->moveDistance.x <= 0 && this->moveDistance.y <= 0 && abs(acc.x) < 0.1f && abs(acc.y) < 0.1f) {
		this->fireMoveCount = 0;

		std::random_device _dev;
		std::mt19937 _rng(_dev());
		std::uniform_int_distribution<std::mt19937::result_type> _moveDist(3 / this->difficultyModifier, 5 / this->difficultyModifier);
		this->fireMoveThreshold = _moveDist(_rng);

		//if player in front -> gunfire
		//else -> rocketfire
		if (this->playerPos.x < this->fixedHitboxPos.x && this->playerPos.y < this->fixedHitboxPos.y + 55 && this->playerPos.y > this->fixedHitboxPos.y - 55)
			return UniqueLogicStates::Gunfire;
		else
			return UniqueLogicStates::Rocketfire;
	}
	return UniqueLogicStates::Idle;
}


void EnemyRocketeer::Animate()
{
	if (this->animState > animationData.size())
		return;

	//animation one way switching
	if (this->animState == ExplosionAnim && this->animationFrame == animationData.at(ExplosionAnim).second) {
		this->animState = Nothing;
		this->animationFrame = animationData.at(Nothing).first;
	}

	//looping
	if (this->animState == IdleAnim) {
		this->AnimateLooping(animState);
	}

	//single instance animationss
	else if (this->animState == RocketfireAnim || this->animState == GunfireAnim || this->animState == ExplosionAnim)
		this->AnimateStraight(animState);

	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));
}

void EnemyRocketeer::RandomiseParity()
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _parityDist(0, 1);

	this->parityCheck = _parityDist(_rng);
}

void EnemyRocketeer::FireBullets()
{
	if (this->uniqueLogicState == Gunfire) {
		SinusoidBullet* _bll = new SinusoidBullet(5, 5, sf::Vector2f(this->position.x + 16, this->position.y + 24), Bullet::RocketeerB, 0);
		this->newBullets.push_back(std::move(_bll));
		_bll = new SinusoidBullet(5, 5, sf::Vector2f(this->position.x + 16, this->position.y + 24), Bullet::RocketeerB, 1);
		this->newBullets.push_back(std::move(_bll));

		this->audioPlayer->PlaySoundId(RocketeerGunfire, PlayerPrefs::EFFECTS * 0.1f, 0.1f);
	}
	else if (this->uniqueLogicState == Rocketfire) {
		RocketBullet* _bll = new RocketBullet(7, 5, sf::Vector2f(this->position.x + 34 + this->bulletCount, this->position.y + 20 + this->bulletCount), Bullet::RocketeerR, 8.f, 5.f / difficultyModifier);
		this->newBullets.push_back(std::move(_bll));

		this->audioPlayer->PlaySoundId(RocketeerRocketfire, PlayerPrefs::EFFECTS * 0.1f);
	}
}

void EnemyRocketeer::UniqueOnDamageLogic(bool _wasContact)
{
	this->acc.x *= 0.5f;
	this->acc.y *= 0.5f;
}

void EnemyRocketeer::OnDeathLogic()
{
	this->acc = sf::Vector2f(0, 0);
	this->audioPlayer->StopSound(RocketeerDodge);
	this->audioPlayer->StopSound(RocketeerGunfire);

	std::chrono::steady_clock::time_point _timeZero = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point _currTime;
	float _time = 0;

	do {
		_currTime = std::chrono::high_resolution_clock::now();
		_time = (float)std::chrono::duration_cast<std::chrono::milliseconds>(_currTime - _timeZero).count();
	} while (!this->terminateDeathThread && _time <= 2000);

	this->allowDestory = true;
}