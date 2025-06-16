#include "Player.h"

Player::Player(const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds, const sf::Texture* _texture, std::pair<Weapon::WeaponType, Weapon::WeaponType> _weapons) : Unit(0, 64, 32, sf::Vector2f(40, 103), _texture, sf::Vector2(43, 18), sf::Vector2(9, 8))
{
	//Entrance
	//this->animationData.push_back(std::tuple(Entrance, 1, 37));
	this->animationData.insert(std::make_pair(Entrance, std::make_pair(1, 37)));
	//Idle
	this->animationData.insert(std::make_pair(Idle, std::make_pair(39, 46)));
	//Down
	this->animationData.insert(std::make_pair(Down, std::make_pair(48, 51)));
	//Up
	this->animationData.insert(std::make_pair(Up, std::make_pair(53, 56)));
	//Dying
	this->animationData.insert(std::make_pair(Dying, std::make_pair(57, 63)));

	this->animState = Entrance;
	this->animationFrame = 1;

	this->dodgeCooldownTimer = 2000;
	this->shieldRegenCooldownTimer = 0;
	this->shieldRegenTickTimer = 0;
	this->damageInvTimer = 0;


	//player stats
	this->maxHealth = 225;
	this->health = maxHealth;

	this->maxShield = 100;
	this->shield = maxShield;

	this->contactDamage = 20;

	//weapons
	this->weapons.first = new Weapon(_weapons.first, _playerSounds);
	this->weapons.second = new Weapon(_weapons.second, _playerSounds);

	//sounds
	this->LoadSounds(_playerSounds);
	this->audioPlayer->PlaySoundId(PlEntrance, PlayerPrefs::EFFECTS * 0.1f);
}

Player::~Player()
{
	delete this->weapons.first;
	delete this->weapons.second;
	delete this->audioPlayer;
}

void Player::LoadSounds(const std::unordered_map<SoundId, sf::SoundBuffer>& _playerSounds)
{
	this->audioPlayer->LoadSound(PlEntrance, &(_playerSounds.at(PlEntrance)), 60, 0.87f);

	this->audioPlayer->LoadSound(PlDodge1, &(_playerSounds.at(PlDodge1)), 80, 1.25f);
	this->audioPlayer->LoadSound(PlDodge2, &(_playerSounds.at(PlDodge2)), 80, 1.25f);
	this->audioPlayer->LoadSound(PlDodge3, &(_playerSounds.at(PlDodge3)), 80, 1.25f);

	this->audioPlayer->LoadSound(PlHitShield, &(_playerSounds.at(PlHitShield)), 80, 1.f);
	this->audioPlayer->LoadSound(PlHitHealth, &(_playerSounds.at(PlHitHealth)), 80, 1.f);
	this->audioPlayer->LoadSound(PlShieldDown, &(_playerSounds.at(PlShieldDown)), 100, 1.f);
	this->audioPlayer->LoadSound(PlShieldRegain, &(_playerSounds.at(PlShieldRegain)), 100, 1.f);

	this->audioPlayer->LoadSound(PlDeath, &(_playerSounds.at(PlDeath)), 80, 1.f);
}

void Player::OnUpdate(float _deltaTime)
{
	this->UpdateUnitTimers(_deltaTime);

	switch (logicState) {
	case Logic:
		if (!blockControls) {
			this->directionVector = GetDirectionInput();
			this->CalculateAcceleration(_deltaTime);
			this->ExecuteDodgeInput(_deltaTime);
			this->AutomaticWeaponsFireInput();
			this->weapons.first->OnUpdate(_deltaTime);
			this->weapons.second->OnUpdate(_deltaTime);
			this->FireWeapons();
			this->RegenShield(_deltaTime);
		}
		this->UpdatePosition(_deltaTime);

		if (this->health <= 0) {
			this->blockControls = true;
			this->animState = Dying;
			this->logicState = Destroyed;
			this->SetHitboxActive(false);
			this->audioPlayer->PlaySoundId(PlDeath, PlayerPrefs::EFFECTS);
			this->deathEffectTimer = 0;
		}
		break;
	case Destroyed: {
		this->directionVector = sf::Vector2i(0, 0);
		this->CalculateAcceleration(_deltaTime);
		this->UpdatePosition(_deltaTime);

		this->deathEffectTimer += _deltaTime * 1000;

		if (deathEffectTimer > this->deathTimeMs) {
			this->logicState = Gone;
			this->isVisible = false;
		}
		break;
	}
	case Gone:
		break;
	default:
		break;
	}
}


void Player::CalculateAcceleration(float _deltaTime)
{
	//Calculated during dodge motion
	if (dodgeMotion) {
		this->acc.x = this->dodgeVector.x * accLimit;
		this->acc.y = this->dodgeVector.y * accLimit;
	}
	//Regular calculations
	else {
		//Modifiers - userd when player quickly chanches directio to give a small boost to their acceleration
		float _xModif = 1;
		float _yModif = 1;

		if (this->directionVector.x != 0) {
			if (GetSign(directionVector.x) != GetSign(acc.x))
				_xModif = 2;

			this->acc.x += this->directionVector.x * this->speed * _xModif * _deltaTime;
			this->acc.x = std::clamp(acc.x, -accLimit, accLimit);
		}
		else { //When x direction is null
			if (acc.x >= 0) {
				acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 0.75;
				this->acc.x = std::clamp(acc.x, 0.f, accLimit);
			}
			else {
				acc.x -= this->GetSign(acc.x) * this->speed * _deltaTime * 0.75;
				this->acc.x = std::clamp(acc.x, -accLimit, 0.f);
			}
		}

		if (this->directionVector.y != 0) {
			if (GetSign(directionVector.y) != GetSign(acc.y))
				_yModif = 2;

			this->acc.y += this->directionVector.y * this->speed * _yModif * _deltaTime;
			this->acc.y = std::clamp(acc.y, -accLimit, accLimit);
		}
		else { //When y direction is null
			if (acc.y >= 0) {
				acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 0.75;
				this->acc.y = std::clamp(acc.y, 0.f, accLimit);
			}
			else {
				acc.y -= this->GetSign(acc.y) * this->speed * _deltaTime * 0.75;
				this->acc.y = std::clamp(acc.y, -accLimit, 0.f);
			}
		}
	}
}

void Player::UpdatePosition(float _deltaTime)
{
	//during dodge motion
	if (dodgeMotion) {
		float _modifier = 1;
		if (abs(dodgeVector.x == 1) && abs(dodgeVector.y == 1))
			_modifier = 0.707f;

		this->position.x += this->dodgeVector.x * _deltaTime * dodgeStrength * _modifier * 100;
		this->position.y += this->dodgeVector.y * _deltaTime * dodgeStrength * _modifier * 100;
	}
	//regular movement
	else {
		//calculate angle and limit diagnal speed
		float _angleRad = atan2f(this->acc.y, this->acc.x);
		float _xModif = abs(cosf(_angleRad));
		float _yModif = abs(sinf(_angleRad));

		//funky way to prevent "floaty" feeling, not very logical but works
		this->position.x += (this->acc.x * 2 / 3 + this->acc.x * _xModif / 3) * _deltaTime * 100;
		this->position.y += (this->acc.y * 2 / 3 + this->acc.y * _yModif / 3) * _deltaTime * 100;
	}

	this->position.x = std::clamp(position.x, 0.f - this->hitboxOffset.x, 480.f - hitboxOffset.x - (float)hitbox.x);
	this->position.y = std::clamp(position.y, 0.f - this->hitboxOffset.y, 270.f - hitboxOffset.y - (float)hitbox.y);

	Unit::UpdatePosition(_deltaTime);
}

sf::Vector2<int> Player::GetDirectionInput()
{
	sf::Vector2<int> _vect = sf::Vector2<int>(0, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		_vect.x += -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		_vect.x += 1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		_vect.y += -1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		_vect.y += 1;
	return _vect;
}

void Player::AutomaticWeaponsFireInput()
{
	if (this->weapons.first->IsAutomatic() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		this->shootLeft = true;
	if (this->weapons.second->IsAutomatic() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
		this->shootRight = true;
}


void Player::ActionInput(int _inputId)
{
	if (this->blockControls)
		return;

	switch (_inputId) {
	case 0:
		this->dodge = true;
		break;
	case 1:
		this->shootLeft = true;
		break;
	case 2:
		this->shootRight = true;
		break;
	}
}

bool Player::CheckIfDodge() const
{
	return this->dodgeMotion;
}

sf::Vector2<int> Player::GetDodgeVector() const
{
	return this->dodgeVector;
}

void Player::ExecuteDodgeInput(float _deltaTime) {
	this->dodgeCooldownTimer += _deltaTime * 1000;

	if (!dodgeMotion && dodge && this->dodgeCooldownTimer > dodgeCooldownInMs) {

		dodgeCooldownTimer = 0;

		//dodge logic
		this->dodgeMotion = true;
		this->hitboxActive = false;

		this->dodgeVector = GetDirectionInput();
		if (dodgeVector.x == 0 && dodgeVector.y == 0)
			this->dodgeVector = sf::Vector2<int>(1, 0);
		this->sprite.setColor(sf::Color(255, 255, 255, 128));

		//play sound
		std::vector<SoundId> _dodgeSounds{ PlDodge1,PlDodge2,PlDodge3 };
		this->audioPlayer->PlaySoundId(_dodgeSounds, PlayerPrefs::EFFECTS * 0.1f, 0.12f);
	}
	else if (dodgeMotion && this->dodgeCooldownTimer > dodgeDurationInMs) {
		//disable dodge logic
		this->hitboxActive = true;
		this->dodgeMotion = false;

		this->dodgeVector = sf::Vector2<int>(0, 0);
		this->sprite.setColor(sf::Color(255, 255, 255, 255));
	}

	this->dodge = false;
}

void Player::FireWeapons()
{
	if (this->shootLeft || (sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->weapons.first->IsAutomatic()))
	{
		shootLeft = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			this->weapons.first->ForceReload();
		else if (this->weapons.first->IsReady()) {
			this->weapons.first->Fire();

			//create new bullet
			if (this->newBullets.first != nullptr) {
				delete this->newBullets.first;
				this->newBullets.first = nullptr;
			}
			else {
				this->newBullets.first = CreateNewBullet(this->weapons.first);
			}
		}
	}

	if (this->shootRight || (sf::Mouse::isButtonPressed(sf::Mouse::Right) && this->weapons.second->IsAutomatic()))
	{
		shootRight = false;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			this->weapons.second->ForceReload();
		else if (this->weapons.second->IsReady()) {
			this->weapons.second->Fire();

			if (this->newBullets.second != nullptr) {
				delete this->newBullets.second;
				this->newBullets.second = nullptr;
			}
			else {
				this->newBullets.second = CreateNewBullet(this->weapons.second);
			}
		}
	}
}

Bullet* Player::CreateNewBullet(const Weapon* _weapon)
{
	switch (_weapon->GetWeaponType()) {
	case Weapon::Blaster:
		return std::move(new Bullet(15, 7, this->position, Bullet::BlasterB));
	case Weapon::Spitter:
		return std::move(new Bullet(5, 5, this->position, Bullet::SpitterB));
	case Weapon::RailPiercer:
		return std::move(new Bullet(17, 7, this->position, Bullet::RailB));
	case Weapon::FoeSeeker:
		return std::move(new RocketBullet(8, 5, this->position, Bullet::RocketB, 8.f, 3.f));
	default:
		return nullptr;
	}
}


void Player::TakeDamage(int _dmgValue, bool _useTimer)
{
	if (_useTimer && !this->DamageTimerCheck())
		return;

	this->damageHighlightTimer = 0;

	this->audioPlayer->StopSound(PlShieldRegain);

	if (this->shield > _dmgValue) {
		this->shield -= _dmgValue;
		this->audioPlayer->PlaySoundId(PlHitShield, PlayerPrefs::EFFECTS * 0.1f);

		//lower acceleration if shield damaged
		this->acc.x *= 0.75f;
		this->acc.y *= 0.75f;
	}
	else if (this->shield == _dmgValue) {
		this->shield = 0;
		this->audioPlayer->PlaySoundId(PlHitShield, PlayerPrefs::EFFECTS * 0.1f);
		this->audioPlayer->PlaySoundId(PlShieldDown, PlayerPrefs::EFFECTS * 0.1f);

		//lower acceleration if shield damaged
		this->acc.x *= 0.75f;
		this->acc.y *= 0.75f;
	}
	else if (this->shield < _dmgValue && this->shield > 0) {
		int _hpDmgCarry = _dmgValue - this->shield;
		this->shield = 0;
		this->health -= _hpDmgCarry;
		this->audioPlayer->PlaySoundId(PlHitHealth, PlayerPrefs::EFFECTS * 0.1f);
		this->audioPlayer->PlaySoundId(PlShieldDown, PlayerPrefs::EFFECTS * 0.1f);

		//lower acceleration even more
		this->acc.x *= 0.5f;
		this->acc.y *= 0.5f;
	}
	else {
		this->health -= _dmgValue;
		this->audioPlayer->PlaySoundId(PlHitHealth, PlayerPrefs::EFFECTS * 0.1f);

		//lower acceleration even more
		this->acc.x *= 0.5f;
		this->acc.y *= 0.5f;
	}

	this->shieldRegenCooldownTimer = 0;
	shieldRegenSound = false;
}

bool Player::IsEntryOver() const
{
	return this->readyForAction;
}

Player::PlayerState Player::GetState() const
{
	return this->logicState;
}

void Player::FullRegenShield()
{
	if (this->shield == maxShield)
		return;

	this->shield = maxShield;
}

void Player::RegenShield(float _deltaTime) {

	if (this->shield >= this->maxShield)
		return;

	this->shieldRegenCooldownTimer += _deltaTime * 1000;
	this->shieldRegenTickTimer += _deltaTime * 1000;

	//check if cooldowns are ready
	//shorter time per tick if shield low

	if (shieldRegenCooldownTimer > this->shieldRegenTimeMs - this->audioPlayer->GetSoundLength(PlShieldRegain) && !shieldRegenSound) {
		if (this->shield < this->maxShield)
			this->audioPlayer->PlaySoundId(PlShieldRegain, PlayerPrefs::EFFECTS * 0.1f);

		shieldRegenSound = true;
	}

	if (shieldRegenCooldownTimer > this->shieldRegenTimeMs && shieldRegenTickTimer > this->shieldRegenTickTimeMs * (0.5f + 0.5f * (float)(shield / maxShield))) {
		this->shieldRegenTickTimer = 0;
		this->shield++;
	}
}

bool Player::CheckDodgeReadyHighlight() const
{
	if (this->dodgeCooldownTimer > dodgeCooldownInMs && this->dodgeCooldownTimer <= dodgeCooldownInMs + 100 && !this->blockControls)
		return true;
	else
		return false;
}

bool Player::CheckShield() const
{
	if (this->shield > 0)
		return true;
	else
		return false;
}

void Player::Animate()
{
	if (this->animState > animationData.size())
		return;

	if (this->animState == Idle || this->animState == Down || this->animState == Up) {

		bool _skipFrame = false;

		//Handle transition frames, update new animation loop
		if (this->directionVector.y > 0 && (animState == Idle || animState == Up)) {
			this->animState = Down;
			this->animationFrame = animationData.at(Down).first - 1;
			_skipFrame = true;
		}
		else if (this->directionVector.y < 0 && (animState == Idle || animState == Down)) {
			this->animState = Up;
			this->animationFrame = animationData.at(Up).first - 1;
			_skipFrame = true;
		}
		else if (this->directionVector.y == 0) {
			if (animState == Down) {
				this->animState = Idle;
				this->animationFrame = animationData.at(Down).first - 1;
				_skipFrame = true;
			}
			else if (animState == Up) {
				this->animState = Idle;
				this->animationFrame = animationData.at(Up).first - 1;
				_skipFrame = true;
			}
		}

		//looping animations
		if (!_skipFrame) {
			std::pair<int, int> _currRange = this->animationData.at(animState);
			int _botFrame = _currRange.first;
			int _topFrame = _currRange.second;

			if (animationFrame >= _botFrame && animationFrame < _topFrame)
				this->animationFrame++;
			else
				this->animationFrame = _botFrame;
		}
	}
	else
	{
		//single instance animations
		if (this->animState == Entrance) {
			this->animationFrame++;
			if (animationFrame > this->animationData.at(Entrance).second) {
				this->animState = Idle;
				this->blockControls = false;
				this->readyForAction = true;
				this->logicState = Logic;
			}
		}
		else if (this->animState == Dying) {
			if (animationFrame < this->animationData.at(Dying).first)
				this->animationFrame = this->animationData.at(Dying).first;
			else if (animationFrame <= this->animationData.at(Dying).second) {
				this->animationFrame++;
			}
		}
	}
	this->sprite.setTextureRect(sf::IntRect((width * (animationFrame - 1)), 0, width, height));
}

std::tuple<float, float, int, int, int, int> Player::GetData() const
{
	float _shieldProc = (float)this->shield / (float)maxShield;
	float _hpProc = (float)this->health / (float)maxHealth;

	std::pair<int, int> _weaponDataL, _weaponDataR;
	_weaponDataL = this->weapons.first->GetAmmoData();
	_weaponDataR = this->weapons.second->GetAmmoData();

	return std::tuple<float, float, int, int, int, int>(_shieldProc, _hpProc, _weaponDataL.first, _weaponDataL.second, _weaponDataR.first, _weaponDataR.second);
}

std::pair<Bullet*, Bullet*> Player::GetNewBullets()
{
	std::pair<Bullet*, Bullet*> _bullets = std::make_pair(std::move(this->newBullets.first), std::move(this->newBullets.second));
	this->newBullets.first = nullptr;
	this->newBullets.second = nullptr;
	return std::move(_bullets);
}

std::pair<int, int> Player::GetWeapons() const
{
	return std::make_pair(this->weapons.first->GetWeaponType(), this->weapons.second->GetWeaponType());
}
