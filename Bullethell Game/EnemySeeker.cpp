#include "EnemySeeker.h"

EnemySeeker::EnemySeeker(int _id, const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds, sf::Vector2f _position, const sf::Texture* _texture, float _difficulty) : Enemy(_id, _enemySounds, 64, 32, _position, _texture, sf::Vector2(35, 20), sf::Vector2(16, 8))
{
	this->type = Seeker;
	this->animState = IdleAnim;
	this->difficultyModifier = _difficulty;
	this->moneyWorth = 275;
	this->contactDamage = 22 * _difficulty;
	this->targetingFireThreshold /= _difficulty;
	this->targetingLaserThreshold /= _difficulty;

	//health
	this->SetHealth(60 * _difficulty);

	this->LoadSounds(_enemySounds);

	this->animationData.insert(std::make_pair(IdleAnim, std::make_pair(1, 8)));
	this->animationData.insert(std::make_pair(FireballAnim, std::make_pair(9, 16)));
	this->animationData.insert(std::make_pair(LaserPrepAnim, std::make_pair(17, 24)));
	this->animationData.insert(std::make_pair(LaserFireAnim, std::make_pair(25, 28)));
	this->animationData.insert(std::make_pair(LaserEndAnim, std::make_pair(29, 35)));
	this->animationData.insert(std::make_pair(ExplosionAnim, std::make_pair(36, 40)));
	this->animationData.insert(std::make_pair(Nothing, std::make_pair(41, 41)));

	this->RandomiseAcc();
}

void EnemySeeker::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _enemySounds)
{
	//unique death sound here
	this->audioPlayer->LoadSound(EnemyDeath, &(_enemySounds.at(SeekerDeath)), 50, 1.15f);

	//load unique sound here
	this->audioPlayer->LoadSound(EnemyHit, &(_enemySounds.at(DroneHit)), 40, 1.35f);
	this->audioPlayer->LoadSound(SeekerFireball, &(_enemySounds.at(SeekerFireball)), 80, 0.8f);
	this->audioPlayer->LoadSound(SeekerLaser, &(_enemySounds.at(SeekerLaser)), 100, 0.85f);
}

void EnemySeeker::OnUpdate(float _deltaTime)
{
	//timer update
	this->UpdateUnitTimers(_deltaTime);
	this->stateChangeTimer += _deltaTime * 1000;


	switch (this->genState) {
	case (Entry):
		Entrance(_deltaTime);
		break;
	case (Logic):
		switch (this->uniqueLogicState) {
		case(Idle):
			this->CalculateAcc(_deltaTime, 1, true);

			if (playerPos.y <= this->fixedHitboxPos.y + 15 && playerPos.y >= this->fixedHitboxPos.y - 15 && !startedTargeting) {
				this->startedTargeting = true;
			}

			//if player stays in front ->laser
			//if not ->fireball
			if (startedTargeting) {
				this->targetingTimer += _deltaTime * 1000;

				//if player stays in front
				if (targetingTimer > targetingLaserThreshold) {
					this->stateChangeTimer = 0;
					this->audioPlayer->PlaySoundId(SeekerLaser, PlayerPrefs::EFFECTS * 0.1f);
					this->dangerZone->SetVisible(true);
					this->dangerZone->SetFrameState(0);
					this->animState = LaserPrepAnim;
					this->uniqueLogicState = LaserPrep;
				}
				else if (playerPos.y > this->fixedHitboxPos.y + 45 || playerPos.y < this->fixedHitboxPos.y - 45) {
					if (targetingTimer > targetingFireThreshold) {
						this->fired = false;
						this->stateChangeTimer = 0;
						this->animState = FireballAnim;
						this->uniqueLogicState = Fireball;
					}
					else {
						this->fired = false;
						this->stateChangeTimer = 0;
						this->targetingTimer = 0;
						this->startedTargeting = false;
					}
				}
			}

			break;
		case(Fireball):
			this->CalculateAcc(_deltaTime, 1, false);

			if (stateChangeTimer > 375 && !fired) {
				this->FireBullets();
				this->fired = true;
			}
			if (stateChangeTimer > 1000) {
				this->targetingTimer = 0;
				this->stateChangeTimer = 0;
				this->startedTargeting = false;
				this->animState = IdleAnim;
				this->uniqueLogicState = Idle;
			}
			break;

		case(LaserPrep):
			this->CalculateAcc(_deltaTime, 0.75, true);
			if (stateChangeTimer > 1000) {
				this->stateChangeTimer = 0;
				this->dangerZone->Activate(true);
				this->dangerZone->SetFrameState(1);
				this->animState = LaserFireAnim;
				this->uniqueLogicState = Laser;
			}
			break;
		case(Laser):
			this->CalculateAcc(_deltaTime, 0.5, true);
			if (stateChangeTimer > 3750) {
				this->stateChangeTimer = 0;
				this->dangerZone->Activate(false);
				this->dangerZone->SetVisible(true);
				this->dangerZone->SetFrameState(2);
				this->animState = LaserEndAnim;
				this->uniqueLogicState = EndLaser;
			}
			break;
		case(EndLaser):
			this->CalculateAcc(_deltaTime, 0.5, false);
			if (stateChangeTimer > 875) {
				this->targetingTimer = 0;
				this->stateChangeTimer = 0;
				this->startedTargeting = false;
				this->dangerZone->SetVisible(false);
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

	if (dangerZone != nullptr) {
		dangerZone->UpdatePosition(this->position + sf::Vector2f(15.5, 16.5));
	}
}

void EnemySeeker::Entrance(float _deltaTime)
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());

	if (!this->entranceSet) {
		std::uniform_int_distribution<std::mt19937::result_type> _posxDist(436, 464);
		this->entrancePos.x = _posxDist(_rng);
		this->entranceSet = true;
		this->drawBlur = true;
	}

	if (entrancePos.x < this->fixedPosition.x) {
		acc.x -= _deltaTime;
		this->acc.x = std::clamp(acc.x, -accLimit, 0.f);
		this->blurOffset = sf::Vector2f(-2.5f * acc.x, 0);
	}
	else {
		this->genState = Logic;
		this->uniqueLogicState = Idle;
		this->RandomiseAcc();
		this->drawBlur = false;
	}
}

void EnemySeeker::CalculateAcc(float _deltaTime, float _modifier, bool _seek)
{
	if (_seek && abs(acc.x) < 0.01f && (playerPos.y > this->fixedHitboxPos.y + 15 || playerPos.y < this->fixedHitboxPos.y - 15)) {
		this->acc.y += this->GetSign(playerPos.y - this->fixedHitboxPos.y) * _deltaTime * this->speed * _modifier;
		this->acc.y = std::clamp(acc.y, -accLimit * _modifier, accLimit * _modifier);
	}
	else {
		if (acc.y >= 0) {
			acc.y -= this->GetSign(acc.y) * _deltaTime * 0.4f * this->speed * _modifier;
			this->acc.y = std::clamp(acc.y, 0.f, accLimit);
		}
		else {
			acc.y -= this->GetSign(acc.y) * _deltaTime * 0.4f * this->speed * _modifier;
			this->acc.y = std::clamp(acc.y, -accLimit, 0.f);
		}
	}

	if (this->genState != Entry) {
		if (acc.x >= 0) {
			acc.x -= this->GetSign(acc.x) * _deltaTime * 1.f;
			this->acc.x = std::clamp(acc.x, 0.f, accLimit);
		}
		else {
			acc.x -= this->GetSign(acc.x) * _deltaTime * 1.f;
			this->acc.x = std::clamp(acc.x, -accLimit, 0.f);
		}
	}
}

void EnemySeeker::UpdatePosition(float _deltaTime)
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

void EnemySeeker::RandomiseAcc()
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _accDist(35, 65);
	std::uniform_int_distribution<std::mt19937::result_type> _speedDist(175, 225);
	this->accLimit = (float)_accDist(_rng) * difficultyModifier / 100.f;
	this->speed = (float)_speedDist(_rng) * difficultyModifier / 100.f;
	this->randomised = true;
}

void EnemySeeker::Animate()
{
	if (this->animState > animationData.size())
		return;

	//animation one way switching
	if (this->animState == ExplosionAnim && this->animationFrame == animationData.at(ExplosionAnim).second) {
		this->animState = Nothing;
		this->animationFrame = animationData.at(Nothing).first;
	}

	//looping
	if (this->animState == IdleAnim || this->animState == LaserFireAnim) {
		this->AnimateLooping(animState);
	}

	//single instance animationss
	else if (this->animState == FireballAnim || this->animState == LaserPrepAnim || this->animState == LaserEndAnim || this->animState == ExplosionAnim)
		this->AnimateStraight(animState);

	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));

	if (this->dangerZone != nullptr && this->dangerZone->IsVisible())
		this->dangerZone->Animate();
}

void EnemySeeker::FireBullets()
{
	FireballBullet* _bll = new FireballBullet(10, 10, sf::Vector2f(this->position.x + 16.5, this->position.y + 17.5), Bullet::SeekerB, this->difficultyModifier, this->playerPos);
	this->newBullets.push_back(std::move(_bll));

	this->audioPlayer->PlaySoundId(SeekerFireball, PlayerPrefs::EFFECTS * 0.1f);
}

void EnemySeeker::LoadDangerZone(const sf::Texture* _texture) {

	this->dangerZone = new DangerZone(DangerZone::Laser, 480, 12, this->position, _texture, this->difficultyModifier);
}

void EnemySeeker::UniqueOnDamageLogic(bool _wasContact)
{
	this->acc.x *= 0.33f;
	this->acc.y *= 0.1f;
}

void EnemySeeker::OnDeathLogic()
{
	this->acc = sf::Vector2f(0, 0);

	this->audioPlayer->StopSound(SeekerLaser);
	this->dangerZone->Activate(false);

	std::chrono::steady_clock::time_point _timeZero = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point _currTime;
	float _time = 0;

	do {
		_currTime = std::chrono::high_resolution_clock::now();
		_time = (float)std::chrono::duration_cast<std::chrono::milliseconds>(_currTime - _timeZero).count();
	} while (!this->terminateDeathThread && _time <= 2000);

	this->allowDestory = true;
}
