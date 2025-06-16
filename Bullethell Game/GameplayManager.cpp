#include "GameplayManager.h"

GameplayManager::GameplayManager(sf::RenderWindow* _windowPtr)
{
	this->windowPtr = _windowPtr;

	//load textures
	this->LoadTexture(PlayerTxt, "Sprites\\Player.png");

	//bullets
	this->LoadTexture(BlasterBullet, "Sprites\\Blaster_Bullet.png");
	this->LoadTexture(SpitterBullet, "Sprites\\Spitter_Bullet.png");
	this->LoadTexture(PiercerBullet, "Sprites\\Piercer_Bullet.png");
	this->LoadTexture(LauncherBullet, "Sprites\\Launcher_Rocket.png");

	this->LoadTexture(DroneBullet, "Sprites\\Drone_Bullet.png");
	this->LoadTexture(GunnerBullet, "Sprites\\Gunner_Bullet.png");
	this->LoadTexture(RocketeerBullet, "Sprites\\Rocketeer_Bullet.png");
	this->LoadTexture(RocketeerRocket, "Sprites\\Rocketeer_Rocket.png");
	this->LoadTexture(SeekerBullet, "Sprites\\Seeker_Fireball.png");
	//enemies
	this->LoadTexture(DroneTxt, "Sprites\\Enemy_Drone.png");
	this->LoadTexture(GunnerTxt, "Sprites\\Enemy_Gunner.png");
	this->LoadTexture(RocketeerTxt, "Sprites\\Enemy_Rocketeer.png");
	this->LoadTexture(SeekerTxt, "Sprites\\Enemy_Seeker.png");

	//light ray
	this->LoadTexture(Ray, "Sprites\\Light_Ray.png");

	//danger zones
	this->LoadTexture(FlamethrowerZone, "Sprites\\Gunner_Flame.png");
	this->LoadTexture(LaserZone, "Sprites\\Seeker_Laser.png");

	//Background
	this->LoadTexture(BackgroundTxt, "Sprites\\Background.png");

	this->LoadTexture(CloudSmall, "Sprites\\Cloud_Small.png");
	this->LoadTexture(CloudMedium, "Sprites\\Cloud_Medium.png");
	this->LoadTexture(CloudLarge, "Sprites\\Cloud_Large.png");

	//load player sounds
	this->LoadPlayerSound(PlEntrance, "Sounds\\player_entrance.ogg");

	this->LoadPlayerSound(PlDodge1, "Sounds\\dodge1.ogg");
	this->LoadPlayerSound(PlDodge2, "Sounds\\dodge2.ogg");
	this->LoadPlayerSound(PlDodge3, "Sounds\\dodge3.ogg");

	this->LoadPlayerSound(PlHitShield, "Sounds\\hit_shield.ogg");
	this->LoadPlayerSound(PlHitHealth, "Sounds\\hit_health.ogg");
	this->LoadPlayerSound(PlShieldDown, "Sounds\\shield_down.ogg");
	this->LoadPlayerSound(PlShieldRegain, "Sounds\\shield_regain_on.ogg");

	//weapon sounds
	this->LoadPlayerSound(WpReloadComplete, "Sounds\\reload_complete.ogg");
	this->LoadPlayerSound(WpBlasterFire, "Sounds\\blaster_fire.ogg");
	this->LoadPlayerSound(WpSpitterFire, "Sounds\\spitter_fire.ogg");
	this->LoadPlayerSound(WpPiercerFire, "Sounds\\railpiercer_fire.ogg");
	this->LoadPlayerSound(WpSeekerFire, "Sounds\\foeseeker_rocket.ogg");

	//death
	this->LoadPlayerSound(PlDeath, "Sounds\\player_death.ogg");

	//enemy sounds
	this->LoadEnemySound(EnemyDeath, "Sounds\\enemy_death.ogg");
	this->LoadEnemySound(DroneHit, "Sounds\\drone_hit.ogg");
	this->LoadEnemySound(DroneFire, "Sounds\\drone_fire.ogg");
	this->LoadEnemySound(DroneRam, "Sounds\\drone_ram.ogg");

	this->LoadEnemySound(GunnerDeploy, "Sounds\\gunner_deploy.ogg");
	this->LoadEnemySound(GunnerFire, "Sounds\\gunner_fire.ogg");
	this->LoadEnemySound(GunnerInterrupt, "Sounds\\gunner_interrupt.ogg");
	this->LoadEnemySound(GunnerFlamethrower, "Sounds\\gunner_flamethrower.ogg");

	this->LoadEnemySound(RocketeerDodge, "Sounds\\rocketeer_dodge.ogg");
	this->LoadEnemySound(RocketeerGunfire, "Sounds\\rocketeer_gunfire.ogg");
	this->LoadEnemySound(RocketeerRocketfire, "Sounds\\rocketeer_rocketfire.ogg");

	this->LoadEnemySound(SeekerDeath, "Sounds\\seeker_death.ogg");
	this->LoadEnemySound(SeekerFireball, "Sounds\\seeker_fireball.ogg");
	this->LoadEnemySound(SeekerLaser, "Sounds\\seeker_laser.ogg");

	this->ClearEnemyQueue();

	this->background = new Background(&this->textures.at(BackgroundTxt));

	this->cloudsMtx.lock();
	this->ConstructClouds(true);
	this->cloudsMtx.unlock();
}

GameplayManager::~GameplayManager()
{
	this->ClearEnemyQueue();

	this->windowPtr = nullptr;

	std::future<void> _destroyBll = std::async(std::launch::async, &GameplayManager::DestroyBullets, this);
	std::future<void> _destroyEn = std::async(std::launch::async, &GameplayManager::DestroyEnemies, this);
	std::future<void> _destroyRay = std::async(std::launch::async, &GameplayManager::DestroyRays, this);
	std::future<void> _destroyClouds = std::async(std::launch::async, &GameplayManager::DestroyClouds, this);

	this->playerMtx.lock();
	delete this->player;
	this->playerMtx.unlock();

	this->bgMtx.lock();
	delete this->background;
	this->bgMtx.unlock();

	_destroyBll.get();
	_destroyEn.get();
	_destroyRay.get();
	_destroyClouds.get();
}

void GameplayManager::DestroyBullets() {
	this->bulletsMtx.lock();
	std::list<Bullet*>::iterator _blIt;
	for (_blIt = bullets.begin(); _blIt != this->bullets.end(); ++_blIt) {
		delete (*_blIt);
	}
	this->bullets.clear();
	this->bulletsMtx.unlock();
}

void GameplayManager::DestroyEnemies() {
	this->enemyMtx.lock();
	std::list<Enemy*>::iterator _enIt;
	for (_enIt = enemies.begin(); _enIt != this->enemies.end(); ++_enIt) {
		delete (*_enIt);
	}
	this->enemies.clear();
	this->enemyMtx.unlock();
}

void GameplayManager::DestroyRays() {
	this->raysMtx.lock();
	std::list<LightRay*>::iterator _rayIt;
	for (_rayIt = deathRays.begin(); _rayIt != this->deathRays.end(); ++_rayIt) {
		delete (*_rayIt);
	}
	this->deathRays.clear();
	this->raysMtx.unlock();
}

void GameplayManager::DestroyClouds() {
	this->cloudsMtx.lock();

	for (int i = 0; i < 10; i++) {
		if (this->clouds[i] != nullptr) {
			delete clouds[i];
			clouds[i] = nullptr;
		}
	}
	this->cloudsMtx.unlock();
}

void GameplayManager::UpdateWindowPtr(sf::RenderWindow* _window)
{
	this->windowPtr = _window;
}

void GameplayManager::NewGame(UIManager* _uiManagerPtr, std::pair<int, int> _weapons)
{
	this->isPaused = false;

	this->playerMtx.lock();
	if (player != nullptr) {
		delete this->player;
		this->player = nullptr;
	}

	//creating new player with equipped weapons
	this->player = new Player(this->playerSounds, &textures.at(PlayerTxt), std::make_pair((Weapon::WeaponType)_weapons.first, (Weapon::WeaponType)_weapons.second));
	this->playerMtx.unlock();

	//selecting player weapons
	_uiManagerPtr->SelectGunSprites(_weapons.first + 1, _weapons.second + 1);


	this->unitFrameTimer = 0;
	this->logicState = Logic;

	this->ClearEnemyQueue();

	//variables
	playerDodge = false;
	playerShootLeft = false;
	playerShootRight = false;
	statDistanceFloat = 0;
	statDistance = 0;
	statKills = 0;
	statEarned = 0;
	statWave = 0;
	difficultyModifier = 1.f;
	waveTotalCapacity = 0;
	waveAtOnceCapacity = 0;
	currUsedCapacity = 0;
	currEnemyId = 1;

	//death effect
	this->rayTimeMs = 425;
}

void GameplayManager::LoadPlayerSound(SoundId _name, std::string _path) {
	sf::SoundBuffer _buffer;
	_buffer.loadFromFile(_path);

	this->playerSounds.insert(std::make_pair(_name, std::move(_buffer)));
}

void GameplayManager::LoadEnemySound(SoundId _name, std::string _path) {
	sf::SoundBuffer _buffer;
	_buffer.loadFromFile(_path);

	this->enemySounds.insert(std::make_pair(_name, std::move(_buffer)));
}


void GameplayManager::LoadTexture(TextureId _name, std::string _path)
{
	sf::Texture _texture;
	_texture.loadFromFile(_path);
	this->textures.insert(std::make_pair(_name, std::move(_texture)));
}

//On Update
void GameplayManager::HandleGameLogic(float _deltaTime, UIManager* _uiManagerPtr)
{
	this->pauseTimer += _deltaTime * 1000;

	if (this->isPaused)
		return;

	this->spawnDelayTimer += _deltaTime * 1000;
	this->addRayTimer += _deltaTime * 1000;

	if (this->logicState == Logic) {
		//ASYNC FUNCTIONS

		//spawn enemies
		//if none start next wave
		std::future<void> waveLogic = std::async(std::launch::async, &GameplayManager::EnemyWaveLogic, this);

		//update ui
		std::future<void> manageUi = std::async(std::launch::async, &GameplayManager::ManageUi, this, _deltaTime, _uiManagerPtr);

		//LINEAR LOGIC HERE
		//update mouse position
		this->UpdateMousePos();

		//Player input stuff
		this->playerMtx.lock();
		this->player->OnUpdate(_deltaTime);
		this->playerMtx.unlock();

		//bullets
		this->bulletsMtx.lock();
		this->ManageBullets(_deltaTime);
		this->bulletsMtx.unlock();

		//enemies
		this->enemyMtx.lock();
		this->ManageEnemies(_deltaTime);
		this->enemyMtx.unlock();

		Player::PlayerState _plState = this->player->GetState();

		if (_plState == Player::Destroyed || _plState == Player::Gone) {
			this->logicState = PlDead;
			this->ManageUi(_deltaTime, _uiManagerPtr);
			this->addRayTimer = 0;
			this->isPaused = false;
		}

		waveLogic.get();
		manageUi.get();
	}
	else if (this->logicState == PlDead) {
		this->player->OnUpdate(_deltaTime);

		//update rays
		this->raysMtx.lock();
		std::list<LightRay*>::iterator _rayIt;
		for (_rayIt = deathRays.begin(); _rayIt != this->deathRays.end(); ++_rayIt) {
			(*_rayIt)->OnUpdate(_deltaTime);
		}
		this->raysMtx.unlock();

		//add new death ray if timer
		if (this->addRayTimer > this->rayTimeMs) {
			if (rayTimeMs > 25)
				rayTimeMs -= 25;
			else if (rayTimeMs != 10)
				rayTimeMs = 10;

			this->addRayTimer = 0;
			this->AddLightRay(this->player->GetFixedPos());
		}

		if (this->player->GetState() == Player::Gone)
		{
			this->logicState = ShowReturn;

			//uninitialize objects
			std::future<void> _destroyBll = std::async(std::launch::async, &GameplayManager::DestroyBullets, this);
			std::future<void> _destroyEn = std::async(std::launch::async, &GameplayManager::DestroyEnemies, this);
			std::future<void> _destroyRay = std::async(std::launch::async, &GameplayManager::DestroyRays, this);

			_destroyBll.get();
			_destroyEn.get();
			_destroyRay.get();
		}
	}
}

void GameplayManager::SpawnCloud(int _layer, bool _randX)
{
	if (this->clouds[_layer] != nullptr)
		return;

	sf::Vector2f _pos;
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _posYDist(0, 270);
	std::uniform_int_distribution<std::mt19937::result_type> _sizeDist(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> _flipDist(0, 1);

	if (_randX) {
		std::uniform_int_distribution<std::mt19937::result_type> _posXDist(0, 685);
		_pos = sf::Vector2f((int)_posXDist(_rng) - 200, (int)_posYDist(_rng));
	}
	else
		_pos = sf::Vector2f(485, (int)_posYDist(_rng));


	int _size = _sizeDist(_rng);
	bool _isFlipped = _flipDist(_rng);

	switch (_size) {
	case(1):
		this->clouds[_layer] = new Cloud(323, 89, _pos, &this->textures.at(CloudSmall), _layer, _isFlipped);
		break;
	case(2):
		this->clouds[_layer] = new Cloud(381, 159, _pos, &this->textures.at(CloudMedium), _layer, _isFlipped);
		break;
	case(3):
		this->clouds[_layer] = new Cloud(447, 118, _pos, &this->textures.at(CloudLarge), _layer, _isFlipped);
		break;
	}



}

void GameplayManager::AddLightRay(sf::Vector2f _pos)
{
	LightRay* _new = new LightRay(_pos, &this->textures.at(Ray));
	this->deathRays.push_back(std::move(_new));
}


void GameplayManager::ManageUi(float _deltaTime, UIManager* _uiManagerPtr)
{
	sf::Vector2f _playerPosition = this->player->GetPosition();
	if ((_playerPosition.x <= 175 && _playerPosition.y >= 218) || (_playerPosition.x <= 84 && _playerPosition.y >= 187))
		_uiManagerPtr->MakeBotGameUiTransparent(true);
	else
		_uiManagerPtr->MakeBotGameUiTransparent(false);

	if ((_playerPosition.x <= 153 && _playerPosition.y <= 28))
		_uiManagerPtr->MakeTopGameUiTransparent(true);
	else
		_uiManagerPtr->MakeTopGameUiTransparent(false);

	this->UpdateStats(_deltaTime);
	_uiManagerPtr->UpdateGameUiData(this->player->GetData());
	_uiManagerPtr->UpdateStatUiData(statDistance, statKills, statEarned, statWave);
}


void GameplayManager::UpdateStats(float _deltaTime)
{
	//check enemy deaths
	//add kills
	//add money

	const float kmPerSec = 1;
	if (this->player->IsEntryOver()) {
		if (this->statWave == 0) {
			statDistanceFloat = 1.f;
		}

		this->statDistanceFloat += kmPerSec * _deltaTime;
		this->statDistance = std::floor(statDistanceFloat);
	}
}

void GameplayManager::ManageBullets(float _deltaTime)
{
	//EXISTING BULLETS
	std::list<Bullet*>::iterator _bulIt;
	for (_bulIt = bullets.begin(); _bulIt != this->bullets.end();) {

		std::vector<sf::Vector2f> _positionChecks;
		sf::Vector2i _hitbox = (*_bulIt)->GetHitbox();


		//update bullet position and get position checks
		_positionChecks = (*_bulIt)->UpdatePosition(_deltaTime);

		if ((*_bulIt)->GetOwner() == Bullet::Player) {
			(*_bulIt)->UpdateTargetPos(this->mousePos);

			//enemy hit detection

			std::list<Enemy*>::iterator _enIt;

			for (_enIt = enemies.begin(); _enIt != this->enemies.end(); _enIt++) {
				int _id = (*_enIt)->GetId();

				if ((*_enIt)->IsHitboxActive() && !(*_bulIt)->CheckIfIdHit(_id, _deltaTime))
					for (sf::Vector2f& _pos : _positionChecks) {

						if ((*_enIt)->CheckIfHit(_pos, _hitbox)) {
							(*_enIt)->TakeDamage((*_bulIt)->GetDamage(), false);
							(*_bulIt)->LowerPen();
							(*_bulIt)->AddId(_id);
							break;
						}
						//unique to rocketter -> set dodge to true
						else if ((*_enIt)->GetType() == Enemy::EnemyType::Rocketeer && (dynamic_cast<EnemyRocketeer*>(*_enIt))->EnableDodgeInDanger(_pos))
							break;
					}
			}
		}
		else {
			int _id = this->player->GetId();
			(*_bulIt)->UpdateTargetPos(this->player->GetFixedHitbox());

			if (this->player->IsHitboxActive() && !(*_bulIt)->CheckIfIdHit(_id, _deltaTime))
				for (sf::Vector2f& _pos : _positionChecks) {

					if (player->CheckIfHit(_pos, _hitbox)) {
						player->TakeDamage((*_bulIt)->GetDamage(), false);
						(*_bulIt)->LowerPen();
						(*_bulIt)->AddId(_id);
						break;
					}
				}
		}

		_positionChecks.clear();

		if ((*_bulIt)->CheckDespawn()) {
			delete(*_bulIt);
			_bulIt = bullets.erase(_bulIt);
		}
		else
			_bulIt++;
	}

	this->CreateNewBullets();
}

void GameplayManager::CreateNewBullets()
{
	//NEW PLAYER BULLETS
	//calculate bullet offsets
	sf::Vector2f _playerPos = this->player->GetFixedPos();
	sf::Vector2f _playerPosTop = _playerPos, _playerPosBot = _playerPos;
	_playerPosTop.y += -8;
	_playerPosBot.y += 5;

	std::pair<Bullet*, Bullet*> _newPlayerBullets = this->player->GetNewBullets();

	//left weapon bullet
	if (_newPlayerBullets.first != nullptr) {
		_newPlayerBullets.first->LoadData(_playerPosTop, this->mousePos, true);
		_newPlayerBullets.first->LoadTextures(this->textures);
		_newPlayerBullets.first->SetVisible(true);
		this->bullets.push_back(std::move(_newPlayerBullets.first));
	}

	//right weapon bullet
	if (_newPlayerBullets.second != nullptr) {
		_newPlayerBullets.second->LoadData(_playerPosBot, this->mousePos, true);
		_newPlayerBullets.second->LoadTextures(this->textures);
		_newPlayerBullets.second->SetVisible(true);
		this->bullets.push_back(std::move(_newPlayerBullets.second));
	}


	//enemy bullets
	std::list<Enemy*>::iterator _enIt;

	for (_enIt = enemies.begin(); _enIt != this->enemies.end(); _enIt++) {
		std::vector<Bullet*> _newEnemyBullets = (*_enIt)->GetNewBullets();

		for (int i = 0; i < _newEnemyBullets.size(); i++) {

			Bullet* _new = std::move(_newEnemyBullets[i]);
			_new->LoadTextures(this->textures);
			_new->SetVisible(true);
			_new->ApplyModifier(this->difficultyModifier);

			this->bullets.push_back(std::move(_new));
		}
	}
}


void GameplayManager::ConstructClouds(bool _inConstructor)
{
	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _layerDist(0, 9);

	int _count;
	if (_inConstructor) {
		std::uniform_int_distribution<std::mt19937::result_type> _countDist(2, 4);
		_count = _countDist(_rng);
	}
	else
		_count = 1;

	const int _maxTries = 10;
	for (int i = 0; i < _count; i++) {
		int _currTries = 0;
		bool _success = false;

		do {
			int _layer = _layerDist(_rng);
			if (this->clouds[_layer] == nullptr) {
				_success = true;
				this->SpawnCloud(_layer, _inConstructor);
			}
			else {
				_currTries++;
			}

		} while (_currTries < _maxTries && !_success);
	}
}

void GameplayManager::HandleBackgroundLogic(float _deltaTime, UIManager* _uiManagerPtr)
{
	if (this->isPaused)
		return;

	this->spawnCloudTimer += _deltaTime * 1000;
	float _yOffset = 119;

	this->playerMtx.lock();
	if (this->player != nullptr)
		_yOffset = player->GetFixedPos().y;
	this->playerMtx.unlock();


	this->cloudsMtx.lock();
	int _count = 0;

	for (int i = 0; i < 10; i++) {
		if (this->clouds[i] != nullptr) {
			this->clouds[i]->UpdateOffset(_yOffset);
			this->clouds[i]->OnUpdate(_deltaTime);
			_count++;

			if (this->clouds[i]->CheckDelete()) {
				delete clouds[i];
				clouds[i] = nullptr;
			}
		}
	}

	//add new clouds
	this->currSpawntime += _deltaTime * 1000;
	if (this->currSpawntime >= spawnThreshold * _count) {
		this->ConstructClouds(false);
		this->currSpawntime -= spawnThreshold * _count;
	}

	this->cloudsMtx.unlock();
}


void GameplayManager::UpdateAnimator(float _deltaTime)
{
	if (this->isPaused)
		return;

	this->unitFrameTimer += _deltaTime * 1000;

	if (this->unitFrameTimer >= 125) {
		this->unitFrameTimer = 0;

		//animate enemies and bullets
		std::future<void> animBullets = std::async(std::launch::async, &GameplayManager::AnimateBullets, this);
		std::future<void> animEnemies = std::async(std::launch::async, &GameplayManager::AnimateEnemies, this);

		//animate player
		this->playerMtx.lock();
		if (this->player != nullptr)
			this->player->Animate();
		this->playerMtx.unlock();

		//animate background
		if (this->background != nullptr) {
			this->bgMtx.lock();
			this->background->Animate();
			this->bgMtx.unlock();
		}

		animBullets.get();
		animEnemies.get();
	}
}

void GameplayManager::AnimateBullets() {
	this->bulletsMtx.lock();
	std::list<Bullet*>::iterator _bll;
	for (_bll = bullets.begin(); _bll != this->bullets.end(); ++_bll) {
		(*_bll)->Animate();
	}
	this->bulletsMtx.unlock();
}

void GameplayManager::AnimateEnemies() {
	this->enemyMtx.lock();
	std::list<Enemy*>::iterator _enIt;
	for (_enIt = enemies.begin(); _enIt != this->enemies.end(); ++_enIt) {
		(*_enIt)->Animate();
	}
	this->enemyMtx.unlock();
}

void GameplayManager::UpdateMousePos()
{
	sf::Vector2<int> _pos = sf::Mouse::getPosition(*this->windowPtr);
	sf::Vector2f _worldPos = windowPtr->mapPixelToCoords(_pos);
	this->mousePos = _worldPos;
}


void GameplayManager::PassPlayerInputEvent(PlayerInput _input)
{
	this->player->ActionInput(_input);
}

//Render Thread
void GameplayManager::DrawBackgroundObjects(sf::RenderWindow* _windowPtr)
{
	this->bgMtx.lock();
	_windowPtr->draw(this->background->GetDrawData());
	this->bgMtx.unlock();

	this->cloudsMtx.lock();
	for (int i = 9; i >= 0; i--) {
		if (this->clouds[i] != nullptr) {
			this->windowPtr->draw(clouds[i]->GetDrawData());
		}
	}
	this->cloudsMtx.unlock();
}

void GameplayManager::DrawGameObjects(sf::RenderWindow* _windowPtr)
{
	this->raysMtx.lock();
	//draw light rays
	if (this->logicState == PlDead) {
		std::list<LightRay*>::iterator _rayIt;

		for (_rayIt = deathRays.begin(); _rayIt != this->deathRays.end(); ++_rayIt) {
			sf::Sprite _bulletSprite = (*_rayIt)->GetDrawData();
			_windowPtr->draw((*_rayIt)->GetDrawData());
		}
	}
	this->raysMtx.unlock();

	//draw enemies
	this->enemyMtx.lock();
	std::list<Enemy*>::iterator _enIt;
	for (_enIt = enemies.begin(); _enIt != this->enemies.end(); ++_enIt) {
		sf::Sprite _enemySprite = (*_enIt)->GetDrawData();

		if ((*_enIt)->CheckDamagedHighlight()) {
			_enemySprite.setColor(sf::Color(255, 0, 68));;
		}

		if ((*_enIt)->CheckBlur()) {
			sf::Vector2f _offset = (*_enIt)->GetBlurOffset();
			sf::Sprite _cloneSprite;
			_cloneSprite = _enemySprite;
			_cloneSprite.setColor(sf::Color(_cloneSprite.getColor().r, _cloneSprite.getColor().g, _cloneSprite.getColor().b, 32));

			for (int i = 1; i <= 3; i++) {
				_cloneSprite.move(_offset);
				_windowPtr->draw(_cloneSprite);
			}
		}
		_windowPtr->draw(_enemySprite);

		DangerZone* _dngPtr = (*_enIt)->GetDangerZonePtr();
		if (_dngPtr != nullptr && _dngPtr->IsVisible()) {
			_windowPtr->draw((*_enIt)->GetDangerZonePtr()->GetDrawData());
		}

	}
	this->enemyMtx.unlock();

	//draw player
	this->playerMtx.lock();
	if (this->player != nullptr && this->player->IsVisible()) {
		sf::Sprite _playerSprite = this->player->GetDrawData();

		if (this->player->CheckIfDodge()) {
			sf::Vector2<int> _cloneDirection = this->player->GetDodgeVector();
			sf::Vector2f _positionOffset;

			sf::Sprite _playerClone;
			for (int i = 6; i <= 18; i += 6) {
				_positionOffset.x = -_cloneDirection.x * i;
				_positionOffset.y = -_cloneDirection.y * i;

				if (abs(_cloneDirection.x) == 1 && abs(_cloneDirection.y) == 1) {
					_positionOffset.x *= 0.707f;
					_positionOffset.y *= 0.707f;
				}
				_playerClone = _playerSprite;
				_playerClone.setColor(sf::Color(255, 255, 255, 32));

				_playerClone.move(_positionOffset);
				_windowPtr->draw(_playerClone);
			}
			_windowPtr->draw(_playerSprite);
		}
		else if (this->player->CheckDamagedHighlight()) {
			if (this->player->CheckShield())
				_playerSprite.setColor(sf::Color(44, 232, 245));
			else
				_playerSprite.setColor(sf::Color(255, 0, 68));

			_windowPtr->draw(_playerSprite);
		}
		else if (this->player->CheckDodgeReadyHighlight()) {

			_playerSprite.setColor(sf::Color::Green);
			_windowPtr->draw(_playerSprite);
		}
		else
			_windowPtr->draw(_playerSprite);
	}
	this->playerMtx.unlock();

	//draw bullets
	this->bulletsMtx.lock();
	std::list<Bullet*>::iterator _blIt;

	for (_blIt = bullets.begin(); _blIt != this->bullets.end(); ++_blIt) {
		sf::Sprite _bulletSprite = (*_blIt)->GetDrawData();
		float _angleRad = (*_blIt)->GetRadRotation();

		for (int i = 2; i <= 8; i += 2) {
			sf::Sprite _bulletClone = _bulletSprite;
			_bulletClone.setColor(sf::Color(255, 255, 255, 64));
			_bulletClone.move(-sf::Vector2f(cosf(_angleRad) * i, sinf(_angleRad) * i));

			_windowPtr->draw(_bulletClone);
		}
		_windowPtr->draw(_bulletSprite);
	}
	this->bulletsMtx.unlock();
}

void GameplayManager::NewWave()
{
	this->statWave++;
	std::cout << "wave: " << statWave << std::endl;
	this->difficultyModifier = this->statWave * log(this->statWave) * 0.01425 + 1;

	this->waveTotalCapacity = pow(this->statWave * 25, (9.f / 14.f)) - 1.f;
	this->waveAtOnceCapacity = 0.5f * this->waveTotalCapacity;

	std::cout << "curr difficulty modifier: " << difficultyModifier << std::endl;
	std::cout << "curr total capacity: " << waveTotalCapacity << std::endl;
	std::cout << "curr at once capacity: " << waveAtOnceCapacity << std::endl;



	//enemy type to capacity
	//larger number - more frequent
	int _droneRarity = 100;
	int _gunnerRarity = 45;
	int _rocketHeliRarity = 50;
	int _laserRarity = 35;

	//capacity - how many enemies with weight
	int _checkCap = this->waveTotalCapacity;
	int _spawnDelayMs = 0;

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _delayDist(0, std::clamp((int)(5000 * (1.f / pow(this->difficultyModifier, 2))), 0, 5000));

	//sum rari
	int _raritySum = _droneRarity;
	//if (this->statWave >= 2)
	if (this->statWave >= 2)
		_raritySum = _gunnerRarity += _raritySum;
	//if (this->statWave >= 3) {
	if (this->statWave >= 3) {
		_raritySum = _rocketHeliRarity += _raritySum;
	}
	//if (this->statWave >= 4) {
	if (this->statWave >= 4) {
		_raritySum = _laserRarity += _raritySum;
	}

	std::uniform_int_distribution<std::mt19937::result_type> _enemyTypeDist(1, _raritySum);
	Enemy::EnemyType _selectedType = Enemy::Drone;

	while (_checkCap > 0) {
		int _enemyType = _enemyTypeDist(_rng);

		if (_enemyType <= _droneRarity && _checkCap > this->enemyCap.at(Enemy::Drone))
			_selectedType = Enemy::Drone;

		else if (_enemyType <= _gunnerRarity && _checkCap > this->enemyCap.at(Enemy::Gunner))
			_selectedType = Enemy::Gunner;

		else if (_enemyType <= _rocketHeliRarity && _checkCap > this->enemyCap.at(Enemy::Rocketeer))
			_selectedType = Enemy::Rocketeer;

		else if (_enemyType <= _laserRarity && _checkCap > this->enemyCap.at(Enemy::Seeker))
			_selectedType = Enemy::Seeker;

		else
			_selectedType = Enemy::Drone;

		_checkCap -= this->enemyCap.at(_selectedType);

		_spawnDelayMs = _delayDist(_rng);

		this->enemyQueue.push(std::make_pair(_selectedType, _spawnDelayMs));
	}
}

void GameplayManager::ClearEnemyQueue()
{
	std::queue<std::pair<Enemy::EnemyType, float>> empty;
	std::swap(this->enemyQueue, empty);
}

void GameplayManager::EnemyWaveLogic()
{
	if (this->enemyQueue.size() == 0 && this->player->IsEntryOver() && this->currUsedCapacity == 0) {
		this->NewWave();
		this->player->FullRegenShield();
		this->spawntimerLock = false;
		return;
	}

	if (this->enemyQueue.size() > 0) {
		std::pair<Enemy::EnemyType, float> _data = this->enemyQueue.front();
		int _capCalc = this->currUsedCapacity;
		//int _capCalc = this->currUsedCapacity + this->enemyCap.at(_data.first);

		if (this->spawnDelayTimer >= _data.second && this->waveAtOnceCapacity > _capCalc) {
			std::pair<Enemy::EnemyType, float> _data = this->enemyQueue.front();

			this->currUsedCapacity += this->enemyCap.at(_data.first);
			std::cout << "currcap new: " << this->currUsedCapacity << std::endl;
			this->spawnDelayTimer = 0;

			this->SpawnEnemy(_data.first);
			this->enemyQueue.pop();
		}
	}
	return;
}

void GameplayManager::SpawnEnemy(Enemy::EnemyType _type)
{
	Enemy* _new = nullptr;

	std::random_device _dev;
	std::mt19937 _rng(_dev());
	std::uniform_int_distribution<std::mt19937::result_type> _posyDist(0, 238);

	switch (_type) {
	case Enemy::Drone:
		_new = new EnemyDrone(currEnemyId++, this->enemySounds, sf::Vector2f(530, _posyDist(_rng)), &textures.at(DroneTxt), difficultyModifier);
		break;
	case Enemy::Gunner:
		_new = new EnemyGunner(currEnemyId++, this->enemySounds, sf::Vector2f(530, _posyDist(_rng)), &textures.at(GunnerTxt), difficultyModifier);

		_new->LoadDangerZone(&textures.at(FlamethrowerZone));
		break;
	case Enemy::Rocketeer:
		_new = new EnemyRocketeer(currEnemyId++, this->enemySounds, sf::Vector2f(530, _posyDist(_rng)), &textures.at(RocketeerTxt), difficultyModifier);
		break;
	case Enemy::Seeker:
		_new = new EnemySeeker(currEnemyId++, this->enemySounds, sf::Vector2f(530, _posyDist(_rng)), &textures.at(SeekerTxt), difficultyModifier);
		_new->LoadDangerZone(&textures.at(LaserZone));
		break;
	default:
		break;
	}

	if (_new != nullptr) {
		this->enemies.push_back(std::move(_new));
	}
}

void GameplayManager::ManageEnemies(float _deltaTime)
{
	std::list<Enemy*>::iterator _enIt;
	for (_enIt = enemies.begin(); _enIt != this->enemies.end();) {
		//adding money on death
		int _addMoney = (*_enIt)->GetMoney();
		if (_addMoney != 0) {
			this->statEarned += _addMoney;
			this->statKills++;
		}

		//remove objects if destroy timer passed
		if ((*_enIt)->AllowDestroy()) {
			this->currUsedCapacity -= this->enemyCap.at((*_enIt)->GetType());
			std::cout << "currcap kill: " << this->currUsedCapacity << std::endl;
			delete((*_enIt));
			_enIt = enemies.erase(_enIt);
		}
		//normal update stuff
		else {
			//check collision with player
			if ((*_enIt)->IsHitboxActive() && this->player->IsHitboxActive() && (*_enIt)->CheckIfHit(player->GetFixedHitbox(), player->GetHitbox())) {
				(*_enIt)->TakeDamage(this->player->GetContactDamage(), true);
				this->player->TakeDamage((*_enIt)->GetContactDamage(), true);
			}
			//unique to rocketter -> set dodge to true
			else if ((*_enIt)->GetType() == Enemy::EnemyType::Rocketeer && (dynamic_cast<EnemyRocketeer*>(*_enIt))->EnableDodgeInDanger(player->GetFixedHitbox()))
				break;

			//check danger zone collision
			if ((*_enIt)->GetDangerZonePtr() != nullptr && this->player->IsHitboxActive() && (*_enIt)->CheckDangerZoneCollision(player->GetFixedHitbox(), player->GetHitbox())) {
				this->player->TakeDamage((*_enIt)->GetDangerZoneDamage(), true);
			}


			//normal update stuff
			(*_enIt)->UpdatePlayerPos(this->player->GetFixedHitbox());
			(*_enIt)->OnUpdate(_deltaTime);

			//unique update stuff

			++_enIt;
		}
	}
}

GameplayManager::GameplayState GameplayManager::GetState() const
{
	return this->logicState;
}

bool GameplayManager::IsGamePaused() const
{
	return this->isPaused;
}

void GameplayManager::Pause()
{
	if (this->logicState != Logic || this->pauseTimer < 250)
		return;

	if (this->isPaused) {
		this->isPaused = false;
		this->pauseTimer = 0;
		std::cout << "UNPAUSED GAME" << std::endl;
	}
	else {
		this->isPaused = true;
		this->pauseTimer = 0;
		std::cout << "PAUSED GAME" << std::endl;
	}
}

std::tuple<int&, int&, int&, int&> const GameplayManager::GetGameOverStats()
{
	return std::tuple<int&, int&, int&, int&>(this->statDistance, this->statKills, this->statEarned, this->statWave);
}